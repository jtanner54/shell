#include <bits/stdc++.h>
using namespace std;

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
    stringstream ss(input);
    vector<string> tokens;
    string token;

    while (getline(ss, token, ' ')) {
      tokens.push_back(token);  // get list of arguments
    }

    if (builtins.count(tokens[0])) {
      builtins[tokens[0]](tokens);
    } else {
      cout << tokens[0] << ": command not found" << endl;
    }
  }

  return 0;
}
