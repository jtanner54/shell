#include "completion.h"

#include <cstring>
#include <string>
#include <vector>

#include <readline/readline.h>

#include "builtins.h"

static char* builtin_generator(const char* text, int state) {
  static size_t index;
  static size_t len;
  const auto& names = get_builtin_names();

  if (!state) {
    index = 0;
    len = strlen(text);
  }

  while (index < names.size()) {
    const std::string& name = names[index++];
    if (name.compare(0, len, text) == 0) {
      return strdup(name.c_str());
    }
  }

  return nullptr;
}

static char** command_completion(const char* text, int start, int /*end*/) {
  rl_attempted_completion_over = 1;

  if (start == 0) {
    return rl_completion_matches(text, builtin_generator);
  }

  return nullptr;
}

void init_completion() {
  // global func pointer for readline to call when it needs to do completion
  rl_attempted_completion_function = command_completion;
}
