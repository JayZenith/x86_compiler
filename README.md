# x86-64 Compiler: Polymorphic vs. Tagged-Union AST

This project implements a mini compiler in C++ that:
* Parses a small imperative scripting language
* Builds an Abstract Syntax Tree (AST)
* Generates x86 assembly from the AST
* Compares two AST architectures:
1. Polymorphic AST – classic base-class polymorphism with unique_ptr child nodes.
2. Tagged-Union AST – union of node types with a manual destructor and move semantics.
    * It uses the Tagged-Union AST Implementation for actual assembly generation 

The compiler supports:
* Integer arithmetic (+, -, *, /)
* Variable bindings (let)
* Program exit (exit)
* Lexical scoping
The focus is memory layout, performance trade-offs, and low-level C++ engineering.


### AST Architectures
Polymorphic AST
* Base class PolyNode with derived types for literals, identifiers, expressions, let statements, and exit nodes.
* Uses std::unique_ptr for child nodes.
* Destructor automatically cleans up children — low overhead.

Tagged-Union AST
* Single Node struct with a NodeType tag and union of node types.
* Manual destructor handles active member.
* Move constructors and assignments carefully implemented for std::unique_ptr and strings.
* Avoids virtual dispatch but incurs destructor overhead.

### Performance & Profiling
Benchmarking shows:
* Tagged-Union AST: slower due to union destruction and string/unique_ptr moves.
* Polymorphic AST: faster for node construction/destruction.
Profiling via gprof confirms node destruction dominates runtime, with std::map lookups contributing significantly.
* Key takeaway: AST design in a compiler involves trade-offs between dynamic dispatch, memory footprint, and move semantics.

## Directory Structure
```bash
project-root/
├── build/ # CMake build directory
├── compiler_test.sh # Bash unit test script
├── profiling.sh # Run gprof 
├── testInput.txt # Sample input source code
├── expectedAssembly.asm # Expected assembly output
├── output.asm # Actual compiler output
├── CMakeLists.txt # Project build configuration
└── src/ # Compiler source files
```

## Requirements 
- Linux OS
- `nasm` and `ld` installed.

## Building the Compiler
```bash
mkdir -p build
cd build
cmake ..
make
./testy ../test_input_files/test_input_1.txt
```
* Outputs benchmark times for both AST implementations.
* Generates x86 assembly (nasm_out.s) for further inspection.

## Running the Unit Test
```bash
chmod +x compiler_test.sh
./compiler_test.sh
```

The script: 
1. Creates a sample input file (testInput.txt).
2. Creates the expected assembly output (expectedAssembly.asm).
3. Runs the compiler (./build/testy) on the input file.
4. Compares the generated nasm_out.s against the expected output.
5. Reports success or failure with detailed differences.

## Sample Input (testInput.txt)
```bash
let x = 2 + 3; 
exit x;
```

## Expected Output (expectedAssembly.asm)

```bash
section .data    ; data section
x dq 0           ; global var x (8 bytes) init. to 0

section .text    ; text section
global _start    ; global symbol _start

_start:
    mov rax, 2   ; Load value 2 into register RAX
    mov rbx, rax ; Copy RAX (2)into RBX
    mov rax, 3   ; Overwrite RAX WITH 3 
    add rax, rbx ; Add RBX (2) to RAX (3), Now: RAX = 5
    mov [x], rax ; Store value fo RAX (5), in global var x
    mov rax, [x] ; Load value of x (5) into RAX
    mov rdi, rax ; Copy RAX (5) into RDI vital as Linux sys 
                 ; calls pass 1st arg in RDI
    mov rax, 60  ; Linux x86-64 syscall number 60 = exit
    syscall      ; invoke syscall, termiante with exit code 5


```

## Notes
- The compiler script assumes the executable is named testy and that it - reads test_input.txt and outputs nasm_out.asm.
- You can adjust the paths in compiler_test.sh if your compiler or file names differ.
- Whitespace is normalized for reliable output comparison.
- Designed for a local Linux environment; minor adjustments may be needed for Windows/MacOS.

## x86 Compiler.