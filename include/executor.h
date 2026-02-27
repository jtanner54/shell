#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
#include <vector>

#include "parser.h"

// std:: because we don't want to pollute the global namespace in the header
// file
std::string searchExecutable(const std::string& name);
void executeFile(const Command& cmd);

#endif  // EXECUTOR_H