# cmpsh - Custom Minimal POSIX Shell

## Overview
`cmpsh` is a basic shell implemented in C that supports:
- Built-in commands: `exit`, `cd`, `pwd`, `paths`
- Execution of external programs using `execv`
- Output redirection using `>`
- Signal handling (Ctrl+C and Ctrl+Z)
- Interactive and non-interactive modes

## Build Instructions
Use `make` to compile:
```bash
make
