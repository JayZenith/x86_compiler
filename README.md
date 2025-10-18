# x86-64 Compiler: Tagged-Union AST
A mini compiler in C++ that parses a small imperative language and generates x86-64 assembly, with a focus on tagged-union AST design.

### Features
#### Compiler Capabilities:
* Integer arithmetic (+, -, *, /)
* Variable bindings (let)
* Program exit statements
* Lexical scoping

### AST Architecture
1. Uses a tagged-union AST: each node has a NodeType tag and a union of payloads.
2. Move semantics are used for node data management.
3. Manual destructors handle cleanup for union members.

# Quick Start
## Requirements 
- Linux OS
- `nasm`,`ld`, `cmake` 

## Build & Run
```bash
mkdir build && cd build
cmake .. && make
./testy ../test_input_files/test_input_1.txt --benchmark
```

## Run Unit Tests:
```bash
chmod +x compiler_test.sh
./compiler_test.sh

## Example
**Input (`testInput.txt`):**

let x = 2 + 3; 
exit x;
```

## Generated Assembly:

```bash
section .data
x dq 0           ; global variable

section .text
global _start

_start:
    mov rax, 2
    mov rbx, rax
    add rax, 3     ; RAX = 2 + 3
    mov [x], rax
    mov rdi, rax
    mov rax, 60    ; exit syscall
    syscall
```

## Directory Structure
```bash
project-root/
├── build/                  # CMake build directory
└── src/                    # Compiler source
├── compiler_test.sh        # Unit test script
├── profiling.sh            # gprof profiling 
├── test_input_files/       # Test cases
```
