#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

struct Command {
  std::vector<std::string> args;
  std::string redirect_out;
};

Command parse_tokens(const std::vector<std::string>& tokens);

#endif  // PARSER