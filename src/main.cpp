#include <iostream>

#include "builtins.h"
#include "lexer.h"

int main() {
  // Flush after every cout / cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    ParseResult result = tokenize(input);

    while (result.state != ParseState::COMPLETE) {
      std::cout << "> ";
      std::string more;
      std::getline(std::cin, more);
      input += more;
      result = tokenize(input);
    }

    if (result.tokens.empty()) continue;

    if (is_builtin(result.tokens[0])) {
      execute_builtin(result.tokens);
    } else {
      std::cerr << result.tokens[0] << " command not found: " << std::endl;
    }
  }

  return 0;
}