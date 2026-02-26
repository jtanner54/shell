#include <bits/stdc++.h>
#include <sys/wait.h>

#include <filesystem>
using namespace std;
namespace fs = filesystem;

vector<string> split(const string& str, char delimiter) {
  vector<string> internal;
  stringstream ss(str);  // Turn the string into a stream.
  string tok;

  while (getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

string searchExecutable(string name) {
  const char* path = getenv("PATH");

  if (path != nullptr) {
    vector<string> paths = split(path, ':');

    for (string path : paths) {
      string executable = path + "/" + name;

      if (fs::exists(executable) && access(executable.c_str(), X_OK) == 0) {
        return executable;
      }
    }
  }

  return "";
}

void execFile(string name, vector<string> args) {
  string executable = searchExecutable(name);

  if (executable != "") {
    // need char** for execv
    vector<char*> c_args;
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
    cout << name << ": command not found" << endl;
  }
}

vector<string> tokenize(const string& input) {
  vector<string> tokens;
  string current;
  bool in_single_quote = false;

  for (int i = 0; i < input.size(); i++) {
    char c = input[i];

    if (in_single_quote) {
      if (c == '\'') {
        in_single_quote = false;
      } else {
        current += c;
      }
    } else {
      if (c == '\'') {
        in_single_quote = true;
      } else if (c == ' ') {
        if (!current.empty()) {
          tokens.push_back(current);
          current.clear();
        }
      } else {
        current += c;
      }
    }
  }

  if (in_single_quote) {
    cerr << "Error: unmatched single quote" << endl;
    return {};
  }

  if (!current.empty()) {
    tokens.push_back(current);
  }

  return tokens;
}

int main() {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;

  // function type that returns nothing and takes in paramteer of
  // vector<string>& directly w/o copy
  unordered_map<string, function<void(const vector<string>&)>> builtins = {
      {"exit", [](auto tokens) { exit(0); }},
      {"echo",
       [](auto tokens) {
         for (int i = 1; i < tokens.size(); i++) {
           cout << tokens[i];
           if (i >= 1) cout << " ";
         }
         cout << endl;
       }},
      {"type",
       [&builtins](auto tokens) {
         if (tokens.size() > 1) {
           for (int i = 1; i < tokens.size(); i++) {
             string path = searchExecutable(tokens[i]);

             if (builtins.count(tokens[i])) {
               cout << tokens[i] << " is a shell builtin" << endl;
             } else if (path != "") {
               cout << tokens[i] << " is " << path << endl;
             } else {
               cout << tokens[i] << ": not found" << endl;
             }
           }
         } else {
           cout << "type: no arguments" << endl;
         }
       }},
      {"pwd",
       [](auto tokens) {
         // .string() returns type path as a native string
         cout << fs::current_path().string() << endl;
       }},
      {"cd",
       [](auto tokens) {
         // only two args
         if (tokens.size() == 2) {
           fs::path path = tokens[1];

           if (fs::is_directory(path)) {
             try {
               fs::current_path(path);
             } catch (const fs::filesystem_error& e) {
               cerr << "cd: " << e.what() << endl;
             }

             return;
           } else if (tokens[1] == "~") {
             try {
               fs::current_path(fs::path(getenv("HOME")));
             } catch (const fs::filesystem_error& e) {
               cerr << "cd: " << e.what() << endl;
             }

             return;
           } else {
             cerr << "cd: " << path.string() << ": No such file or directory"
                  << endl;
             return;
           }
         }

         cerr << "cd: too many arguments" << endl;
       }},
  };

  while (true) {
    cout << "$ ";

    string input;
    getline(cin, input);
    vector<string> tokens = tokenize(input);

    if (tokens.empty()) continue;
    if (builtins.count(tokens[0])) {
      builtins[tokens[0]](tokens);
    } else {
      execFile(tokens[0], tokens);
    }
  }

  return 0;
}
