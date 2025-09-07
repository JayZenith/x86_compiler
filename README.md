# C++ Compiler Test Playground

A C++ compiler that translates a minimal language into x86-64 assembly for Linux.  
Includes a unit test script to verify output. Demonstrates **AST construction, recursive descent parsing, stack-based variable management, arithmetic evaluation, control flow, and low-level code generation**.

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

- The compiler script assumes the executable is named testy and that it - reads test_input.txt and outputs nasm_out.asm.
- You can adjust the paths in compiler_test.sh if your compiler or file names differ.
- Whitespace is normalized for reliable output comparison.
- Designed for a local Linux environment; minor adjustments may be needed for Windows/MacOS.

## 💥 Wala — x86 Compiler.