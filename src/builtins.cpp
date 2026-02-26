#include "builtins.h"

#include <filesystem>
#include <iostream>
#include <unordered_map>

#include "executor.h"  // Needed for 'type' command to search paths

namespace fs = std::filesystem;

// forward declare the builtins so we can put them in the registry
bool is_builtin(const std::string& name);

// internal to this file, not exposed to other files
namespace {
int builtin_exit(const std::vector<std::string>& args) {
  int code = 0;
  if (args.size() > 1) {
    code = stoi(args[1]);
  }
  exit(code);
  return code;
}

int builtin_echo(const std::vector<std::string>& args) {
  for (size_t i = 1; i < args.size(); i++) {
    std::cout << args[i];
    if (i >= 1) std::cout << " ";
  }
  std::cout << std::endl;
  return 0;
}

int builtin_type(const std::vector<std::string>& args) {
  if (args.size() > 1) {
    // size_t not int because we want to compare it with args.size() which is
    // size_t
    for (size_t i = 1; i < args.size(); i++) {
      std::string path = searchExecutable(args[i]);

      if (is_builtin(args[i])) {
        std::cout << args[i] << " is a shell builtin" << std::endl;
      } else if (path != "") {
        std::cout << args[i] << " is " << path << std::endl;
      } else {
        std::cout << args[i] << ": not found" << std::endl;
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
  // only two args
  if (args.size() == 2) {
    fs::path path = args[1];

    if (fs::is_directory(path)) {
      try {
        fs::current_path(path);
      } catch (const fs::filesystem_error& e) {
        std::cerr << "cd: " << e.what() << std::endl;

        return 1;
      }
    } else if (args[1] == "~") {
      try {
        fs::current_path(fs::path(getenv("HOME")));
      } catch (const fs::filesystem_error& e) {
        std::cerr << "cd: " << e.what() << std::endl;
      }
    } else {
      std::cerr << "cd: " << path.string() << ": No such file or directory"
                << std::endl;
      return 1;
    }

    return 0;
  }

  std::cerr << "cd: too many arguments" << std::endl;
  return 1;
}

// function type that returns nothing and takes in paramteer of
// vector<string>& directly w/o copy
using BuiltinFunc = int (*)(const std::vector<std::string>&);

const std::unordered_map<std::string, BuiltinFunc> registry = {
    {"exit", builtin_exit}, {"echo", builtin_echo}, {"pwd", builtin_pwd},
    {"cd", builtin_cd},     {"type", builtin_type},
};
}  // namespace

bool is_builtin(const std::string& name) { return registry.count(name); }

int execute_builtin(const std::vector<std::string>& args) {
  if (args.empty()) return 0;

  // iterator
  auto it = registry.find(args[0]);

  if (it != registry.end()) {
    return it->second(args);
  }

  return 127;
}