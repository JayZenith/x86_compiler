# x86 Compiler

A C++ compiler for a custom language targeting x86 architecture.  
Demonstrates low-level systems programming, parsing, AST construction, and code generation.

## Features
- Custom programming language with syntax inspired by C
- Parsing and AST generation
- x86 assembly code output
- Basic optimization logic for expressions and control flow

## Tech Stack
- C++17
- CMake build system

## Getting Started
1. Clone the repository:
   ```bash
   git clone https://github.com/JayZenith/x86_compiler.git
   cd x86_compiler

Building
---

Just run CMake like this:

```sh
# in build dir
cmake <src-dir>
```

and then

```sh
cmake --build . --target main
```


Usage
---

Run a simple example:

```sh
main "file"
```

### Why This Project Matters
- Showcases systems thinking and low-level programming skills
- Demonstrates understanding of parsing, ASTs, and compiler design
- Highlights ability to bridge high-level logic with machine-level execution