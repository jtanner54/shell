#include <iostream>

#include <readline/readline.h>

#include "completion.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"

int main() {
  // Flush after every cout / cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  init_completion();

  while (true) {
    char* line = readline("$ ");
    if (!line) break;  // EOF (Ctrl+D)
    std::string input(line); // direct initialization
    if (input.empty()) {
      free(line);
      continue;
    }

    ParseResult result = tokenize(input);

#ifndef NDEBUG
    std::cerr << "Tokens: " << result.tokens << std::endl;
#endif

    while (result.state != ParseState::COMPLETE) {
      std::cout << "> ";
      std::string more;
      std::getline(std::cin, more);
      input += "\n" + more;
      result = tokenize(input);
    }

    if (result.tokens.empty()) continue;

    Parser parser(result.tokens);
    auto ast = parser.parse();
    if (ast) ast->execute();
  }

  return 0;
}