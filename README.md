# x86-64 Compiler: Polymorphic vs. Tagged-Union AST
A mini compiler in C++ that parses a small imperative language and generates x86-64 assembly, with a focus on comparing two AST architecture approaches.

### Features
#### Compiler Capabilities:
* Integer arithmetic (+, -, *, /)
* Variable bindings (let)
* Program exit statements
* Lexical scoping

### AST Architectures Compared:
1. Polymorphic AST – Classic OOP with virtual dispatch and unique_ptr for automatic memory management
2. Tagged-Union AST – Manual union-based design with explicit move semantics (used for code generation)

### Performance Insights
Benchmarking revealed:
* Polymorphic AST: Faster construction/destruction due to automatic cleanup
* Tagged-Union AST: Slower due to manual destructor overhead and union member moves
* Profiling (gprof) showed node destruction and std::map lookups dominate runtime

*Key Takeway*: AST design involves trade-offs between dynamic dispatch overhead, memory footprint, and move semantics complexity.

# Quick Start
## Requirements 
- Linux OS
- `nasm`,`ld`, `cmake` 

## Build & Run
```bash
mkdir build && cd build
cmake .. && make
./testy ../test_input_files/test_input_1.txt
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
section .data    ; data section
x dq 0           ; global var x (8 bytes) init. to 0

section .text    ; text section
global _start    ; global symbol _start

_start:
    mov rax, 2   ; Load value 2 into register RAX
    mov rbx, rax ; Copy RAX (2) into RBX
    mov rax, 3   ; Overwrite RAX WITH 3 
    add rax, rbx ; Add RBX (2) to RAX (3), Now: RAX = 5
    mov [x], rax ; Store value fo RAX (5), in global var x
    mov rax, [x] ; Load value of x (5) into RAX
    mov rdi, rax ; Copy RAX (5) into RDI vital as Linux sys 
                 ; calls pass 1st arg in RDI
    mov rax, 60  ; Linux x86-64 syscall number 60 = exit
    syscall      ; invoke syscall, termiante with exit code 5


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
