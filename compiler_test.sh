#!/bin/bash

# Modified unit test for direct file input rather than command-line arguments
# Use this script if your compiler directly reads from input file and writes to output file

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Set the filenames (adjust according to your compiler's expected names)
INPUT_FILE="test_input.txt"         # Your compiler expects this exact name
EXPECTED_OUTPUT="expected_assembly.asm"
ACTUAL_OUTPUT="build/out.asm"     # Your compiler outputs to this exact name

# Set the path to your compiler executable
COMPILER="./build/testy"  # Update this to match your compiler's actual executable name

# Directory where the test is running
CURRENT_DIR=$(pwd)
echo -e "${YELLOW}Current directory: $CURRENT_DIR${NC}"
echo -e "${YELLOW}Testing compiler: $COMPILER${NC}"

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}❌ TEST FAILED: Compiler executable not found at '$COMPILER'${NC}"
    echo -e "Please update the COMPILER variable in this script to point to your compiler."
    exit 1
fi

# Make sure the compiler is executable
chmod +x "$COMPILER" 2>/dev/null

# Create test input file
echo -e "Creating test input file: $INPUT_FILE"
cat > "$INPUT_FILE" << 'EOF'
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
EOF

# Create file with expected output
echo -e "Creating expected output file: $EXPECTED_OUTPUT"
cat > "$EXPECTED_OUTPUT" << 'EOF'
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
EOF

# Function to normalize whitespace for more reliable comparison
normalize_whitespace() {
    # Remove leading/trailing whitespace, compress multiple spaces into one
    sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e 's/[[:space:]]\+/ /g' "$1" > "$1.normalized"
}

# Run test
echo -e "\n${YELLOW}Running compiler unit test...${NC}"
echo -e "1. Running compiler with input file: $INPUT_FILE"

# Just run the compiler without arguments (assumes it reads input.txt and writes to output.asm)
echo -e "${YELLOW}Executing: $COMPILER${NC}"
OUTPUT=$($COMPILER "$INPUT_FILE" 2>&1)
RESULT=$?

# Check if compilation was successful
if [ $RESULT -ne 0 ]; then
    echo -e "${RED}❌ TEST FAILED: Compiler returned an error (exit code: $RESULT)${NC}"
    echo -e "${YELLOW}Compiler output:${NC}"
    echo "$OUTPUT"
    
    echo -e "\n${YELLOW}Troubleshooting steps:${NC}"
    echo "1. Verify that '$COMPILER' is the correct executable name"
    echo "2. Make sure the compiler is looking for '$INPUT_FILE' as input"
    echo "3. Make sure the compiler is writing to '$ACTUAL_OUTPUT' as output"
    echo "4. Try running your compiler manually: $COMPILER"
    echo "5. Check permissions: ls -la $COMPILER"
    
    exit 1
fi

# Check if output file was created
if [ ! -f "$ACTUAL_OUTPUT" ]; then
    echo -e "${RED}❌ TEST FAILED: Output file '$ACTUAL_OUTPUT' was not created${NC}"
    echo -e "${YELLOW}Compiler output:${NC}"
    echo "$OUTPUT"
    echo -e "${YELLOW}Current directory contents:${NC}"
    ls -la
    exit 1
fi

# Normalize whitespace in both files for more reliable comparison
echo "2. Normalizing output files for comparison..."
normalize_whitespace "$EXPECTED_OUTPUT"
normalize_whitespace "$ACTUAL_OUTPUT"

# Compare the expected and actual outputs
echo "3. Comparing compiler output with expected assembly..."
DIFF_OUTPUT=$(diff -B -w "$EXPECTED_OUTPUT.normalized" "$ACTUAL_OUTPUT.normalized")

if [ -z "$DIFF_OUTPUT" ]; then
    echo -e "${GREEN}✅ TEST PASSED: Compiler generated the expected assembly!${NC}"
    
    # Optional: Show a summary of key points tested
    echo -e "\nTest verified that the compiler correctly:"
    echo "- Handled variable declarations (y and x)"
    echo "- Processed arithmetic operations ((10 - 2 * 3) / 2)"
    echo "- Implemented control flow (if-elif-else structure)"
    echo "- Generated proper assembly for exit() function"
    echo "- Correctly ignored comments"
    
    exit 0
else
    echo -e "${RED}❌ TEST FAILED: Generated assembly doesn't match expected output.${NC}"
    echo -e "${YELLOW}Differences found:${NC}"
    echo "$DIFF_OUTPUT"
    
    # Show both files for manual inspection
    echo -e "\n${YELLOW}Expected output (first 10 lines):${NC}"
    head -n 10 "$EXPECTED_OUTPUT"
    
    echo -e "\n${YELLOW}Actual output (first 10 lines):${NC}"
    head -n 10 "$ACTUAL_OUTPUT"
    
    echo -e "\n${YELLOW}For complete comparison, check:${NC}"
    echo "Expected: $EXPECTED_OUTPUT"
    echo "Actual: $ACTUAL_OUTPUT"
    
    exit 1
fi
