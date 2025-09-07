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

Executable will be `testy` in the `build/` directory.


## Running the Unit Test
The provided `compiler_test.sh` script automates testing:
```bash
chmod +x compiler_test.sh
./compiler_test.sh
```

What it does
1. Creates a sample input file: testInput.txt.
2. Creates the expected assembly output: expectedAssembly.asm.
3. Runs the compiler (./build/testy) on the input file.
4. Compares the generated output.asm against the expected output.
5. Reports success or failure with detailed differences.

Sample Input (testInput.txt)
```bash
let x = 2 + 3; 
exit x;
```

Expected Output (expected_assembly.asm)

```bash
global _start
_start:
    mov rax, 2        ; load lhs
    push rax          ; save 2 on stack
    mov rax, 3        ; load rhs
    pop rbx           ; restore lhs (2) into rbx
    add rax, rbx      ; rax = 2 + 3 = 5
    push rax          ; save result (5) as variable x

    mov rax, [rsp+0]  ; load variable x (5) into rax
    mov rdi, rax      ; set exit code = 5
    mov rax, 60       ; syscall number for exit
    syscall           ; exit(5)

    mov rax, 60       ; <- extra fallback exit
    mov rdi, 0
    syscall           ; exit(0)

```

## Notes

- The compiler script assumes the executable is named testy and that it - reads test_input.txt and outputs output.asm.
- You can adjust the paths in compiler_test.sh if your compiler or file names differ.
- The script normalizes whitespace to allow reliable comparisons.
- Designed for a local Linux environment; minor adjustments may be needed for Windows/MacOS.
