#pragma once

#include <string>
#include <vector>

enum class ParseState {
  COMPLETE,
  NEED_SINGLE_QUOTE,
  NEED_DOUBLE_QUOTE,
  NEED_ESCAPE
};

struct ParseResult {
  ParseState state = ParseState::COMPLETE;
  std::vector<std::string> tokens;
};

ParseResult tokenize(const std::string& input);
