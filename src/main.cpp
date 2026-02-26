#include <iostream>

int main() {
  // Flush after every cout / cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";
  }

  return 0;
}
