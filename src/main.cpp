#include <bits/stdc++.h>
using namespace std;

void handle_exit(const vector<string>& tokens) { exit(0); }

// want &args and not *args b/c far simpler
void handle_echo(const vector<string>& tokens) {
  for (int i = 1; i < tokens.size(); i++) {
    cout << tokens[i] << " ";
  }
  cout << endl;
}

int main() {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;

  // function type that returns nothing and takes in paramteer of
  // vector<string>& directly w/o copy
  unordered_map<string, function<void(const vector<string>&)>> builtins = {
      {"exit", handle_exit},
      {"echo", handle_echo},
  };

  builtins["type"] = [&builtins](const vector<string>& tokens) {
    if (tokens.size() > 1) {
      for (int i = 1; i < tokens.size(); i++) {
        if (builtins.count(tokens[i])) {
          cout << tokens[i] << " is a shell builtin" << endl;
        } else {
          cout << tokens[i] << ": not found" << endl;
        }
      }
    }
  };
  while (true) {
    cout << "$ ";

    string input;
    getline(cin, input);
    stringstream ss(input);
    vector<string> tokens;
    string token;

    while (getline(ss, token, ' ')) {
      tokens.push_back(token);  // get list of arguments
    }

    if (tokens[0] == "exit") {
      handle_exit(tokens);
    } else if (tokens[0] == "echo") {
      handle_echo(tokens);
    } else if (tokens[0] == "type") {
      builtins["type"](tokens);
      continue;
    }

    cout << input << ": command not found" << endl;
  }

  return 0;
}
