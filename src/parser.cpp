#include "parser.h"

#include <iostream>

Parser::Parser(const std::vector<std::string>& tokens)
    : tokens_(tokens), current_(0) {}

const std::string& Parser::peek() const {
  // static persists whole program
  static const std::string empty_str = "";
  if (is_at_end()) return empty_str;
  return tokens_[current_];
}

const std::string& Parser::previous() const {
  if (current_ == 0) return peek();  // no previous token, return current
  return tokens_[current_ - 1];
}

const std::string& Parser::advance() {
  if (!is_at_end()) current_++;
  return tokens_[current_ - 1];
}

bool Parser::match(const std::string& expected) {
  if (peek() == expected) {
    advance();
    return true;
  }

  return false;
}

bool Parser::is_at_end() const { return current_ >= tokens_.size(); }

std::unique_ptr<ASTNode> Parser::parse_command() {
  if (is_at_end()) return nullptr;

  auto node = std::make_unique<SimpleCommandNode>();

  // echo hello > out.txt

  while (peek() != "|" && peek() != "&&" && peek() != "||" && !is_at_end()) {
    if (peek() == ">" || peek() == "1>" || peek() == "2>") {
      if (peek() == "2>") node->redirect_out_fd = 2;

      advance();  // consume redirect

      if (is_at_end()) {
        std::cerr << "Syntax error: expected filename after '>'\n";
        return nullptr;
      }

      node->redirect_out = advance();  // consume filename
    } else if (peek() == ">>" || peek() == "1>>" || peek() == "2>>") {
      if (peek() == "2>>") node->redirect_out_fd = 2;

      advance();  // consume redirect

      if (is_at_end()) {
        std::cerr << "Syntax error: expected filename after '>>'\n";
        return nullptr;
      }

      node->redirect_out = advance();  // consume filename
      node->append_out = true;
    } else {
      node->args.push_back(advance());
    }
  }

  return node;
}

std::unique_ptr<ASTNode> Parser::parse_pipeline() { return parse_command(); }

std::unique_ptr<ASTNode> Parser::parse_list() { return parse_pipeline(); }

std::unique_ptr<ASTNode> Parser::parse() { return parse_list(); }