#include "lexer.h"

#include <iostream>

ParseResult tokenize(const std::string& input) {
  /*vector<string> tokens;
  string current;
  bool in_single_quote = false;
  bool in_double_quote = false;

  for (int i = 0; i < input.size(); i++) {
    char c = input[i];

    if (in_single_quote) {
      if (c == '\'') {
        in_single_quote = false;
      } else {
        current += c;
      }
    } else if (in_double_quote) {
      if (c == '"') {
        in_double_quote = false;
      } else {
        current += c;
      }
    } else {
      if (c == '\'') {
        in_single_quote = true;
      } else if (c == '"') {
        in_double_quote = true;
      } else if (c == ' ') {
        if (!current.empty()) {
          tokens.push_back(current);
          current.clear();
        }
      } else if (c == '\\' && i + 1 < input.size()) {
        // Handle escaped characters
        current += input[i + 1];
        i++;  // Skip the next character
      } else {
        current += c;
      }
    }
  }

  if (in_single_quote) {
    cerr << "Error: unmatched single quote" << endl;
    return {};
  } else if (in_double_quote) {
    cerr << "Error: unmatched double quote" << endl;
    return {};
  }

  if (!current.empty()) {
    tokens.push_back(current);
  }

  return tokens;*/
  std::cout << input << std::endl;

  return ParseResult{ParseState::COMPLETE, {}};
}
