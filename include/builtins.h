#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>

#include "parser.h"

bool is_builtin(const std::string& name);
int execute_builtin(const Command& args);

#endif  // BUILTINS_H