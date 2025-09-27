#!/bin/bash
# Minimal unit test for compiler (direct file input/output) + coverage

GREEN='\033[0;32m'; RED='\033[0;31m'; YELLOW='\033[0;33m'; NC='\033[0m'

INPUT_FILE="./test_input_files/test_input_1.txt"
EXPECTED_OUTPUT="./test_output_files/test_output_1.asm"
ACTUAL_OUTPUT="nasm_out.s"
COMPILER="./build/testy"

# Check that compiler exists
# [ ... ] is a Bash Test and -f "$COMPILER" checks if file exists
# echo -e -> print text w/ escape sequences (\n, colors, etc.)
[ ! -f "$COMPILER" ] && { echo -e "${RED}Compiler not found: $COMPILER${NC}"; exit 1; }
# Make compiler executable
# 2>&1 -> redirect stderr to stdout 
chmod +x "$COMPILER" 2>/dev/null

# Normalize whitespace function
normalize() { sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e 's/[[:space:]]\+/ /g' "$1" > "$1.n"; }

# --- Run compiler ---
echo -e "${YELLOW}Running compiler...${NC}"
# capture stdout and stderr into OUTPUT variable (2>&1 -> redirect stderr to stdout)
# [ $? -ne 0 ], $? stores exist status of last command
# if compiled errored, print mesasage in red and exit script
OUTPUT=$($COMPILER "$INPUT_FILE" 2>&1)
[ $? -ne 0 ] && { echo -e "${RED}Compiler error:${NC}\n$OUTPUT"; exit 1; }

# Check that output file was generated
if [ ! -f "$ACTUAL_OUTPUT" ]; then
    echo -e "${RED}Output file not generated yet: $ACTUAL_OUTPUT${NC}"
    echo -e "${YELLOW}Compiler output:${NC}\n$OUTPUT"
    exit 1
fi

# --- Compare outputs ---
normalize "$EXPECTED_OUTPUT"
normalize "$ACTUAL_OUTPUT"
#removes leading/trailing spaces and collapses multiple spaces for comparison


# diff -Bw -> ignore blank lines, ignore whitespace differences and hide output (only care about exigt code for if)
if diff -Bw "$EXPECTED_OUTPUT.n" "$ACTUAL_OUTPUT.n" >/dev/null; then
    echo -e "${GREEN}✅ TEST PASSED${NC}"
else
    echo -e "${RED}❌ TEST FAILED${NC}"
    diff -Bw "$EXPECTED_OUTPUT.n" "$ACTUAL_OUTPUT.n"
    exit 1
fi


# After the comparison
rm -f "$EXPECTED_OUTPUT.n" "$ACTUAL_OUTPUT.n"
