#include <bits/stdc++.h>
using namespace std;

vector<string> split(const string& str, char delimiter) {
  vector<string> internal;
  stringstream ss(str);  // Turn the string into a stream.
  string tok;

  while (getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

bool searchExecutable(string name) {
  const char* path = getenv("PATH");

  if (path != nullptr) {
    vector<string> paths = split(path, ':');

    for (string path : paths) {
      string executable = path + "/" + name;

      if (filesystem::exists(executable)) {
        if (access(executable.c_str(), X_OK) == 0) {
          cout << name << " is " << path << endl;
          return true;
        }
      }
    }
  }
  return false;
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
           cout << tokens[i] << " ";
         }
         cout << endl;
       }},
      {"type",
       [&builtins](auto tokens) {
         if (tokens.size() > 1) {
           for (int i = 1; i < tokens.size(); i++) {
             if (builtins.count(tokens[i])) {
               cout << tokens[i] << " is a shell builtin" << endl;
             } else if (searchExecutable(tokens[i])) {
               return;
             } else {
               cout << tokens[i] << ": not found" << endl;
             }
           }
         } else {
           cout << "type: no arguments" << endl;
         }
       }},
  };

  while (true) {
    cout << "$ ";

    string input;
    getline(cin, input);
    vector<string> tokens = split(input, ' ');

    if (builtins.count(tokens[0])) {
      builtins[tokens[0]](tokens);
    } else {
      cout << tokens[0] << ": command not found" << endl;
    }
  }

  return 0;
}
