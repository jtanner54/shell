#include "builtins.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <unordered_map>

#include "executor.h"

namespace fs = std::filesystem;

namespace {
int builtin_exit(const std::vector<std::string>& args) {
  int code = 0;
  if (args.size() > 1) {
    try {
      code = stoi(args[1]);
    } catch (const std::exception&) {
      std::cerr << "exit: numeric argument required" << std::endl;
      code = 2;
    }
  }
  exit(code);
  return code;
}

int builtin_echo(const std::vector<std::string>& args) {
  for (size_t i = 1; i < args.size(); i++) {
    if (i > 1) std::cout << " ";
    std::cout << args[i];
  }
  std::cout << std::endl;
  return 0;
}

int builtin_type(const std::vector<std::string>& args) {
  if (args.size() > 1) {
    for (size_t i = 1; i < args.size(); i++) {
      if (is_builtin(args[i])) {
        std::cout << args[i] << " is a shell builtin" << std::endl;
      } else {
        std::string path = searchExecutable(args[i]);
        if (!path.empty()) {
          std::cout << args[i] << " is " << path << std::endl;
        } else {
          std::cout << args[i] << ": not found" << std::endl;
        }
      }
    }
  } else {
    std::cout << "type: no arguments" << std::endl;
  }
  return 0;
}

int builtin_pwd(const std::vector<std::string>& /*args*/) {
  std::cout << fs::current_path().string() << std::endl;
  return 0;
}

int builtin_cd(const std::vector<std::string>& args) {
  if (args.size() > 2) {
    std::cerr << "cd: too many arguments" << std::endl;
    return 1;
  }

  if (args.size() == 2) {
    // handle ~ before checking is_directory
    if (args[1] == "~") {
      const char* home = getenv("HOME");
      if (home == nullptr) {
        std::cerr << "cd: HOME not set" << std::endl;
        return 1;
      }
      try {
        fs::current_path(fs::path(home));
      } catch (const fs::filesystem_error& e) {
        std::cerr << "cd: " << e.what() << std::endl;
        return 1;
      }
      return 0;
    }

    fs::path path = args[1];
    if (fs::is_directory(path)) {
      try {
        fs::current_path(path);
      } catch (const fs::filesystem_error& e) {
        std::cerr << "cd: " << e.what() << std::endl;
        return 1;
      }
    } else {
      std::cerr << "cd: " << path.string() << ": No such file or directory"
                << std::endl;
      return 1;
    }

    return 0;
  }

  return 0;
}

using BuiltinFunc = int (*)(const std::vector<std::string>&);

const std::unordered_map<std::string, BuiltinFunc> registry = {
    {"exit", builtin_exit}, {"echo", builtin_echo}, {"pwd", builtin_pwd},
    {"cd", builtin_cd},     {"type", builtin_type},
};
}  // namespace

bool is_builtin(const std::string& name) { return registry.count(name); }

int execute_builtin(const Command& cmd) {
  if (cmd.args.empty()) return 0;

  auto it = registry.find(cmd.args[0]);

  if (it == registry.end()) {
    return 127;
  }

  int saved_stdout = -1;

  // setup redirection
  if (!cmd.redirect_out.empty()) {
    // backup stdout / duplicate it
    saved_stdout = dup(STDOUT_FILENO);
    if (saved_stdout == -1) {
      perror("Failed to backup stdout");
      return 1;
    }

    int flags = O_WRONLY | O_CREAT;
    mode_t mode = 0644;  // rw-r--r--

    int fd = open(cmd.redirect_out.c_str(), flags, mode);
    if (fd == -1) {
      perror("Failed to open file for redirection");
      close(saved_stdout);
      return 1;
    }

    // replace stdout with the file descriptor
    if (dup2(fd, STDOUT_FILENO) == -1) {
      perror("Failed to redirect output");
      close(fd);
      close(saved_stdout);
      return 1;
    }

    close(fd);  // fd is now duplicated to stdout, decrease ref count
  }

  int status = it->second(cmd.args);

  if (saved_stdout != -1) {
    // flush stdout before restoring
    std::cout << std::flush;
    fflush(stdout);

    // restore original stdout
    if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
      perror("Failed to restore stdout");
    }

    close(saved_stdout);
  }

  return status;
}
