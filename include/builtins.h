#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>

bool is_builtin(const std::string& name);
int execute_builtin(const std::vector<std::string>& args);
const std::vector<std::string>& get_builtin_names();

#endif  // BUILTINS_H