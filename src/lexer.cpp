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

    // backslash in '' don't matter
    if (in_single_quote) {
      if (c == '\'')
        in_single_quote = false;
      else
        current_token += c;
      continue;
    }

    if (c == '\\') {
      if (!in_double_quote) {
        escape_next = true;
        continue;
      } else {
        //  posix double quote logic
        if (i + 1 < input.size()) {
          char next_char = input[i + 1];
          if (next_char == '"' || next_char == '\\' || next_char == '$' ||
              next_char == '`' || next_char == '\n') {
            escape_next = true;
            continue;
          }
        }

        current_token += c;
        continue;
      }
    }

    if (c == '"') {
      in_double_quote = !in_double_quote;
      continue;
    }

    if (c == '\'' && !in_double_quote) {
      in_single_quote = !in_single_quote;
      continue;
    }

    if (std::isspace(static_cast<unsigned char>(c)) && !in_double_quote) {
      if (!current_token.empty()) {
        result.tokens.push_back(current_token);
        current_token.clear();
      }
    } else {
      current_token += c;
    }
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

  if (!current_token.empty()) {
    result.tokens.push_back(current_token);
  }

  return result;
}
