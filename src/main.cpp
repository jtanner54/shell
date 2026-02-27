#include <iostream>

#include "builtins.h"
#include "debug.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"

int main() {
  // Flush after every cout / cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

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

    Command cmd = parse_tokens(result.tokens);

    if (cmd.args.empty()) continue;

    if (is_builtin(cmd.args[0])) {
      execute_builtin(cmd);
    } else {
      executeFile(cmd);
    }
  }

  return 0;
}