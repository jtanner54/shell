# shell

A POSIX-compliant Unix shell written from scratch in C++, built alongside the [CodeCrafters "Build Your Own Shell"](https://app.codecrafters.io/courses/shell/overview) automated test suite. Implements a full compiler-style frontend — hand-written lexer, recursive descent parser, and AST executor — with pipe chaining, I/O redirection, and GNU Readline tab autocompletion.

---

## Architecture

```
stdin (readline)
     │
     ▼
  Lexer (tokenize)
     │  ParseResult { tokens[], ParseState }
     ▼
  Parser (recursive descent)
     │  std::unique_ptr<ASTNode>
     ▼
  AST Executor
     ├─ Builtins  → dup/dup2 save-restore in parent
     ├─ External  → fork / execv / waitpid
     └─ Pipeline  → pipe() + fork all stages + waitpid all
```

---

## Components

### Lexer (`lexer.cpp`)

Single-pass character scanner implementing POSIX quoting rules precisely:

- **Single quotes**: all characters literal, no escape processing
- **Double quotes**: only `"`, `\`, `$`, `` ` ``, `\n` are escapable after a backslash — everything else the backslash passes through literally (POSIX spec)
- **Backslash outside quotes**: escapes the next character unconditionally
- **Redirect operators**: `>`, `>>`, `1>`, `2>`, `1>>`, `2>>` recognized as atomic tokens; `1` and `2` merge with a following `>`/`>>` in-place
- **Pipe operator**: `|` emitted as its own token

Returns a `ParseResult` with a `ParseState` enum:

```cpp
enum class ParseState {
  COMPLETE,
  NEED_SINGLE_QUOTE,
  NEED_DOUBLE_QUOTE,
  NEED_ESCAPE
};
```

This drives **multi-line continuation** in `main.cpp` — unclosed quotes or escapes prompt `>` and re-tokenize until `COMPLETE`.

---

### Parser (`parser.cpp`)

Recursive descent parser with explicit precedence levels for future extension:

```
parse()
  └─ parse_list()          // &&, || (logical operators)
       └─ parse_pipeline() // | (pipe chains)
            └─ parse_command()
```

`parse_command()` builds a `SimpleCommandNode` consuming:

| Token | Effect |
|---|---|
| `>` / `1>` | stdout redirect, truncate |
| `>>` / `1>>` | stdout redirect, append |
| `2>` | stderr redirect, truncate |
| `2>>` | stderr redirect, append |
| `<` | stdin redirect |

`parse_pipeline()` builds a `PipelineNode` by collecting `SimpleCommandNode`s separated by `|`.

All nodes owned by `std::unique_ptr<ASTNode>` — no manual memory management.

---

### AST / Executor (`ast.cpp`)

#### Builtin commands
Run in the **parent process**. Redirection uses `dup`/`dup2` with explicit save-and-restore so the shell's own file descriptors are not permanently modified:

```
saved_fd = dup(target_fd)      // save original
dup2(new_fd, target_fd)        // redirect
execute_builtin(args)          // run in parent
dup2(saved_fd, target_fd)      // restore
close(saved_fd)
```

#### External commands
Standard POSIX process model:
1. `fork()` — child inherits file descriptors
2. Child sets up redirection via `dup2`, calls `execv()` with null-terminated `char**`
3. Child uses `_exit()` on failure (not `exit()`) to avoid flushing parent stdio buffers
4. Parent `waitpid()` + `WEXITSTATUS()` for exit code

Exit codes: **127** command not found, **126** `execv` failure.

#### Pipe chains (`cmd1 | cmd2 | cmd3`)
All stages forked concurrently. For N commands, N-1 `pipe()` pairs are created upfront. Each child:
- Closes the read end of its write-side pipe
- Closes the write end of its read-side pipe
- `dup2`s pipe ends onto stdin/stdout
- Calls `execv`

Parent closes all pipe ends and `waitpid`s all children, collecting the last stage's exit code.

---

### Autocompletion (`completion.cpp`)

Tab completion registered via GNU Readline's `rl_completion_entry_function`. `init_completion()` sets up the callback at startup.

---

### REPL (`main.cpp`)

```
readline("$ ")
  → tokenize()
  → while state != COMPLETE: prompt "> ", append input, re-tokenize
  → Parser::parse() → unique_ptr<ASTNode>
  → ASTNode::execute()
```

`std::unitbuf` on `cout`/`cerr` ensures unbuffered output for correct interleaving with Readline.

---

## Building

Requires: `cmake`, `readline`, `vcpkg`

```bash
cmake -B build
cmake --build build
./your_program.sh
```

---

## Features

| Feature | Status |
|---|---|
| POSIX quoting (single, double, backslash) | ✅ |
| Stdout redirect (`>`, `>>`, `1>`, `1>>`) | ✅ |
| Stderr redirect (`2>`, `2>>`) | ✅ |
| Stdin redirect (`<`) | ✅ |
| Pipe chaining (`\|`) | ✅ |
| Builtin commands (cd, pwd, echo, type, exit) | ✅ |
| External command execution (fork/execv) | ✅ |
| Tab autocompletion (GNU Readline) | ✅ |
| Multi-line input continuation | ✅ |
| Logical operators (`&&`, `\|\|`) | 🚧 Planned |

---

Validated against the [CodeCrafters](https://codecrafters.io) automated test suite.