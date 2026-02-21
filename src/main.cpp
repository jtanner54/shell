#include <iostream>
#include <string>

using namespace std;

int main() {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;

  cout << "$ ";

  string input = "";
  cin >> input;

  cout << input << ": command not found" << "\n";

  return 0;
}
