#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>
#include <vector>

// operator overload for debugging purposes << (lexer)
inline std::ostream& operator<<(std::ostream& os,
                                const std::vector<std::string>& vec) {
  os << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    os << "\"" << vec[i] << "\"";
    if (i != vec.size() - 1) os << ", ";
  }
  os << "]";
  return os;
}

#endif