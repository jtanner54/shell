#pragma once

#include <string>
#include <vector>

bool is_builtin(const std::string& name);
int execute_builtin(const std::vector<std::string>& args);