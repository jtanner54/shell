#include <bits/stdc++.h>
using namespace std;

int main() {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;

  while(true) {
    cout << "$ ";

    string input;
    getline(cin, input);
    stringstream ss(input);
    vector<string> tokens;
    string token;
    
    while (getline(ss, token, ' ')) {
      tokens.push_back(token);
    }

    if (tokens[0] == "exit") {
      break;
    } else if (tokens[0] == "echo") {
      for (int i = 1; i < tokens.size(); i++) {
        cout << tokens[i] << " ";
      }
      cout << endl;
      continue;
    }

    cout << input << ": command not found" << endl;
  }

  return 0;
}
