#include "executor.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>

#include "parser.h"
#include "utils.h"

namespace fs = std::filesystem;

std::string searchExecutable(const std::string& name) {
  const char* path = getenv("PATH");

  if (path != nullptr) {
    std::vector<std::string> paths = split(path, ':');

    for (const auto& p : paths) {
      std::string executable = p + "/" + name;

      if (fs::exists(executable) && access(executable.c_str(), X_OK) == 0) {
        return executable;
      }
    }
  }

  return "";
}

void executeFile(const Command& cmds) {
  if (cmds.args.empty()) {
    return;
  }

  std::string executable = searchExecutable(cmds.args[0]);

  if (executable != "") {
    // need char** for execv
    std::vector<char*> c_args;
    // & cause just reference; no need for copying each string
    for (const auto& arg : cmds.args) {
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

      // output redirection
      if (!cmds.redirect_out.empty()) {
        int flags = O_WRONLY | O_CREAT;
        mode_t mode = 0644;  // rw-r--r--

        int fd = open(cmds.redirect_out.c_str(), flags, mode);
        if (fd == -1) {
          perror("Failed to open file for redirection");
          _exit(1);
        }

        // replace stdout with the file descriptor
        if (dup2(fd, STDOUT_FILENO) == -1) {
          perror("Failed to redirect output");
          _exit(1);
        }

        close(fd);
      }

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
    std::cerr << cmds.args[0] << ": command not found" << std::endl;
  }
}
