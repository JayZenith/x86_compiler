# C++ Compiler Test Playground

A small C++ compiler project with a unit test script to verify output assembly generation. The compiler reads a source file, translates it to x86-64 assembly, and writes the result to an output file. This project demonstrates variable handling, arithmetic, control flow, and basic code generation.

## Directory Structure
```bash
project-root/
├── build/ # CMake build directory
├── compiler_test.sh # Bash unit test script
├── test_input.txt # Sample input source code
├── expected_assembly.asm # Expected assembly output
├── output.asm # Actual compiler output
├── CMakeLists.txt # Project build configuration
└── src/ # Compiler source files
```


## Building

Requires `nasm` and `ld` on a Linux operating system.


## Building the Compiler

1. Create a build directory (if not already created):

```bash
mkdir -p build
cd build
```

2. Run CMake to configure the project:

```bash
cmake ..
```

3. Build the project:
```bash
make
```

<!-- ```bash
git clone https://github.com/orosmatthew/hydrogen-cpp
cd hydrogen-cpp
mkdir build
cmake -S . -B build
cmake --build build
``` -->

Executable will be `testy` in the `build/` directory.


## Running the Unit Test
The provided `compiler_test.sh` script automates testing:
```bash
chmod +x compiler_test.sh
./compiler_test.sh
```

What it does
1. Creates a sample input file: test_input.txt.
2. Creates the expected assembly output: expected_assembly.asm.
3. Runs the compiler (./testy) on the input file.
4. Compares the generated output.asm against the expected output.
5. Reports success or failure with detailed differences.

Sample Input (test_input.txt)
```bash
let y = (10 - 2 * 3) / 2;
let x = 7; // first
// first
if (0) {
    x = 1;
} elif (0) {
    x = 2;
} else {
    x = 3;
}
exit(x);
/*
exit(4);
*/
```

Expected Output (expected_assembly.asm)

```bash
global _start
_start:
;; let
mov rax, 2
push rax
mov rax, 3
push rax
mov rax, 2
push rax
pop rax
pop rbx
mul rbx
push rax
mov rax, 10
push rax
pop rax
pop rbx
sub rax, rbx
push rax
pop rax
pop rbx
div rbx
push rax
;; /let
;; let
mov rax, 7
push rax
;; /let
;; if
mov rax, 0
push rax
pop rax
test rax, rax
jz label0
mov rax, 1
push rax
pop rax
mov [rsp + 0], rax
jmp label1
label0:
;; elif
mov rax, 0
push rax
pop rax
test rax, rax
jz label2
mov rax, 2
push rax
pop rax
mov [rsp + 0], rax
jmp label1
label2:
;; else
mov rax, 3
push rax
pop rax
mov [rsp + 0], rax
label1:
;; /if
;; exit
push QWORD [rsp + 0]
mov rax, 60
pop rdi
syscall
;; /exit
mov rax, 60
mov rdi, 0
syscall
```

## Notes

- The compiler script assumes the executable is named testy and that it - reads test_input.txt and outputs output.asm.
- You can adjust the paths in compiler_test.sh if your compiler or file names differ.
- The script normalizes whitespace to allow reliable comparisons.
- Designed for a local Linux environment; minor adjustments may be needed for Windows/MacOS.
