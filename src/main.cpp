#include <iostream>
#include <string>

using namespace std;

int main() {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;

  cout << "$ ";

  return 0;
}
