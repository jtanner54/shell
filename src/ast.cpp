#include "ast.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "builtins.h"
#include "executor.h"
 
int SimpleCommandNode::execute() {
  if (args.empty()) return 0;

  // --- Builtin Execution Path ---
  if (is_builtin(args[0])) {
    int saved_out = -1;
    int saved_in = -1;

    // 1. Setup Redirection in Parent Process
    if (!redirect_out.empty()) {
      saved_out = dup(redirect_out_fd);
      if (saved_out == -1) {
        perror("dup");
        return 1;
      }

      int flags = O_WRONLY | O_CREAT | (append_out ? O_APPEND : O_TRUNC);
      mode_t mode = 0644;  // rw-r--r--

      int fd = open(redirect_out.c_str(), flags, mode);
      if (fd == -1) {
        perror("open");
        close(saved_out);
        return 1;
      }

      if (dup2(fd, redirect_out_fd) == -1) {
        perror("dup2");
        close(fd);
        close(saved_out);
        return 1;
      }

      close(fd);
    }

    /*if (!redirect_in.empty()) {
      saved_in = dup(STDIN_FILENO);
      int fd = open(redirect_in.c_str(), O_RDONLY);
      if (fd == -1) {
        perror("open input");
        if (saved_in != -1) close(saved_in);
        if (saved_out != -1) {
          dup2(saved_out, redirect_out_fd);
          close(saved_out);
        }
        return 1;
      }
      dup2(fd, STDIN_FILENO);
      close(fd);
    }*/

    // 2. Execute
    int status = execute_builtin(args);

    // 3. Teardown and Restore
    std::cout << std::flush;
    fflush(stdout);

    if (saved_out != -1) {
      dup2(saved_out, redirect_out_fd);
      close(saved_out);
    }
    if (saved_in != -1) {
      dup2(saved_in, STDIN_FILENO);
      close(saved_in);
    }

    return status;
  }

  // --- External Command Execution Path ---
  std::string executable = searchExecutable(args[0]);
  if (executable.empty()) {
    std::cerr << args[0] << ": command not found" << std::endl;
    return 127;
  }

  // need char** for execv
  std::vector<char*> c_args;
  // & cause just reference; no need for copying each string
  for (const auto& arg : args) {
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
    return 1;
  } else if (pid == 0) {
    // child process: only runs in the new process

    // output redirection
    if (!redirect_out.empty()) {
      int flags = O_WRONLY | O_CREAT | (append_out ? O_APPEND : O_TRUNC);
      mode_t mode = 0644;  // rw-r--r--

      int fd = open(redirect_out.c_str(), flags, mode);
      if (fd == -1) {
        perror("Failed to open file for redirection");
        _exit(1);
      }

      // replace stdout with the file descriptor
      if (dup2(fd, redirect_out_fd) == -1) {
        perror("Failed to redirect output");
        close(fd);
        _exit(1);
      }

      close(fd);
    }

    /*if (!redirect_in.empty()) {
      int fd = open(redirect_in.c_str(), O_RDONLY);
      if (fd == -1) {
        perror("Failed to open file for input redirection");
        _exit(1);
      }

      // replace stdin with the file descriptor
      if (dup2(fd, STDIN_FILENO) == -1) {
        perror("Failed to redirect input");
        close(fd);
        _exit(1);
      }

      close(fd);
    }*/

    execv(executable.c_str(), c_args.data());
    perror("execv");
    // use _exit for system call
    _exit(126);
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return 1;
  }
}