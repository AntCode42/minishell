*This project has been created as part of the 42 curriculum by agard and gchaillo.*

## Description

**Minishell** is a project from the 42 curriculum designed to recreate a basic, functional UNIX shell. The goal of this project is to provide a deep understanding of process creation, execution, and synchronization using C. It acts as a miniature version of `bash --posix`, capable of handling commands, arguments, environment variables, pipelines, and redirections.

Overview of features implemented:
*   A working prompt using the `readline` library.
*   Command execution using the `PATH` environment variable or relative/absolute paths.
*   Pipes (`|`) to securely connect the output of one command to the input of the next.
*   Redirections: `<` (input), `>` (output), `<<` (heredoc), and `>>` (append).
*   Environment variable expansion (e.g., `$USER` or `$?` for exit status).
*   Signal handling (`Ctrl-C`, `Ctrl-D`, `Ctrl-\`) mimicking standard `bash` behavior.
*   Custom builtins functionality:
    *   `echo` with the `-n` option
    *   `cd` with relative or absolute paths
    *   `pwd` to print the working directory
    *   `export` to set environment variables
    *   `unset` to unset environment variables
    *   `env` to list all environment variables
    *   `exit` to gracefully exit the shell

## Contributions

This was a two-person project. The split below reflects the git history and the file headers of the original repository.

| Area | Main contributor | Files |
|---|---|---|
| Execution engine: fork/exec, pipelines, `PATH` lookup | agard | `src/execution/fork_management.c`, `pipe_management.c`, `pipeline.c`, `sort_cmd.c`, `utils/` |
| Builtins: `echo`, `cd`, `pwd`, `exit` | agard | `src/execution/builtin/` |
| Redirections and heredoc | agard | `src/execution/redir_management.c`, `heredoc_management.c` |
| Lexer, parser, expander | gchaillo | `src/lexer/`, `src/parser/`, `src/expander/` |
| Environment (`env`, `export`, `unset`) and signals | gchaillo | `src/environnement/`, `src/signal/` |

Both contributors worked on the integration (`main.c`, headers, Makefile) and on bug fixing across the whole code base.

## Instructions

### Compilation

To compile Minishell, ensure you have `make`, `gcc`/`clang`, and the `readline` development library installed on your system.

```bash
# Navigate to the root directory of the project
cd minishell

# Compile the project using the Makefile
make
```

### Execution

Once compiled seamlessly, an executable named `minishell` will be generated at the root of the repository.

```bash
# Run minishell
./minishell
```

You will then be presented with the custom prompt, where you can start typing bash commands. To end the session, simply type `exit` or press `Ctrl-D`.

## Resources

### Classic References
*   [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
*   [GNU Readline Library documentation](https://tiswww.case.edu/php/chet/readline/rltop.html)
*   Linux Man pages for system calls (e.g., `man 2 fork`, `man 2 execve`, `man 2 pipe`, `man 2 dup2`, `man 2 sigaction`)
*   [Tutorial: Writing Your Own Shell](https://brennan.io/2015/01/16/write-a-shell-in-c/) by Stephen Brennan
*   [42 Cursus Guide: Minishell](https://web.archive.org/web/20260120081536/https://42-cursus.gitbook.io/guide/3-rank-03/minishell)
*   [How a shell works: What happens when 'ls -l' is typed](https://medium.com/@dkwok94/how-a-shell-works-what-happens-when-ls-l-is-typed-df4eb468a030)
*   [42 Minishell Guide](https://medium.com/@laamirimarouane8/42-minishell-guide-53600f49742b)

### AI Usage
*   **Documentation:** Assistance in writing and formatting this README.
*   **Educational Materials:** Creation of courses and tutorials on the various concepts addressed in this project (e.g., signals, processes).
*   **Brainstorming:** Brainstorming on the project's architecture and general creation process.
*   **Debugging:** Identifying and debugging specific edge cases and memory leaks.
*   **Automated Testing:** Creation of a customized tester to validate the shell's behavior.