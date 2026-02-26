#pragma once  // processor directive to ensure this header is only included once

#include <string>
#include <vector>

// std:: because we don't want to pollute the global namespace in the header
// file
std::string searchExecutable(const std::string& name);
void executeFile(const std::string& name, const std::vector<std::string>& args);