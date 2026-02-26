#include "lexer.h"

#include <iostream>

ParseResult tokenize(const std::string& input) {
  ParseResult result;
  std::string current_token;

  bool in_single_quote = false;
  bool in_double_quote = false;
  bool escape_next = false;

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    if (escape_next) {
      current_token += c;
      escape_next = false;
      continue;
    }

    if (c == '\\' && !in_single_quote && !in_double_quote) {
      escape_next = true;
      continue;
    }

    if (c == '\'' && !in_double_quote) {
      in_single_quote = !in_single_quote;
      continue;
    }

    if (c == '"' && !in_single_quote) {
      in_double_quote = !in_double_quote;
      continue;
    }

    if (c == ' ' && !in_single_quote && !in_double_quote) {
      if (!current_token.empty()) {
        result.tokens.push_back(current_token);
        current_token.clear();
      }
    } else {
      current_token += c;
    }
  }

  if (!current_token.empty()) {
    result.tokens.push_back(current_token);
  }

  if (in_single_quote) {
    result.state = ParseState::NEED_SINGLE_QUOTE;
  } else if (in_double_quote) {
    result.state = ParseState::NEED_DOUBLE_QUOTE;
  } else if (escape_next) {
    result.state = ParseState::NEED_ESCAPE;
  } else {
    result.state = ParseState::COMPLETE;
  }

  return result;
}
