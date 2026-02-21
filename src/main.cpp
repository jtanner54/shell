#include <iostream>
#include <string>

using namespace std;

int main() {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;

  while(true) {
    cout << "$ ";

    string input;
    getline(cin, input);

    if (input == "exit") {
      break;
    }

    cout << input << ": command not found" << endl;
  }
  

  return 0;
}
