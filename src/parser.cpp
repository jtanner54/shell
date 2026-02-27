#include "parser.h"

Command parse_tokens(const std::vector<std::string>& tokens) {
  Command cmd;

  for (size_t i = 0; i < tokens.size(); ++i) {
    if (tokens[i] == ">") {
      if (i + 1 < tokens.size()) {
        cmd.redirect_out = tokens[i + 1];
        ++i;  // Skip the filename token
      }
    } else {
      cmd.args.push_back(tokens[i]);
    }
  }
  return cmd;
}