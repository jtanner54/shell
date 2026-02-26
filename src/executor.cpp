#include "executor.h"

#include <sys/wait.h>

#include <filesystem>
#include <iostream>

#include "utils.h"

namespace fs = std::filesystem;

std::string searchExecutable(const std::string& name) {
  const char* path = getenv("PATH");

  if (path != nullptr) {
    std::vector<std::string> paths = split(path, ':');

    for (std::string p : paths) {
      std::string executable = p + "/" + name;

      if (fs::exists(executable) && access(executable.c_str(), X_OK) == 0) {
        return executable;
      }
    }
  }

  return "";
}

void executeFile(const std::string& name,
                 const std::vector<std::string>& args) {
  std::string executable = searchExecutable(name);

  if (executable != "") {
    // need char** for execv
    std::vector<char*> c_args;
    // & cause just reference; no need for copying each string
    for (auto& arg : args) {
      // c_str returns const char*
      c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);  // execv expects a null-terminated array

    pid_t pid = fork();  // makes carbon copy
    // child gets 0
    // parents gets id of child

    if (pid == -1) {
      // parent error
      perror("parent failed to fork");
    } else if (pid == 0) {
      // child process: only runs in the new process
      execv(executable.c_str(), c_args.data());
      perror("execv");
      // use _exit for system call
      _exit(1);
    } else {
      // parent process: only runs in the original process
      int status;
      // give specific pid (parent has this from fork() call)
      waitpid(pid, &status, 0);
    }
  } else {
    std::cout << name << ": command not found" << std::endl;
  }
}
