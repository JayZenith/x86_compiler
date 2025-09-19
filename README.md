# C++ Compiler Test Playground

A C++ compiler that translates a minimal language into x86-64 assembly for Linux.  
Includes a unit test script to verify output. Demonstrates **AST construction, recursive descent parsing, variable management, arithmetic evaluation, control flow, and low-level code generation**.

## Directory Structure
```bash
project-root/
├── build/ # CMake build directory
├── compiler_test.sh # Bash unit test script
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
```
The executable will be `testy` in the `build/` directory.


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