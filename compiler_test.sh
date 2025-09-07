#!/bin/bash
# Minimal unit test for compiler (direct file input/output) + coverage

GREEN='\033[0;32m'; RED='\033[0;31m'; YELLOW='\033[0;33m'; NC='\033[0m'

INPUT_FILE="textInput.txt"
EXPECTED_OUTPUT="expectedAssembly.asm"
ACTUAL_OUTPUT="nasm_out.s"
COMPILER="./build/testy"

[ ! -f "$COMPILER" ] && { echo -e "${RED}Compiler not found: $COMPILER${NC}"; exit 1; }
chmod +x "$COMPILER" 2>/dev/null

# Create test input
cat > "$INPUT_FILE" <<'EOF'
let x = 2 + 3;
exit x;
EOF

# Create expected output
cat > "$EXPECTED_OUTPUT" <<'EOF'
global _start
_start:
    mov rax, 2  
    push rax
    mov rax, 3
    pop rbx
    add rax, rbx
    push rax
    mov rax, [rsp + 0]
    mov rdi, rax
    mov rax, 60
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
EOF

# Normalize whitespace function
normalize() { sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e 's/[[:space:]]\+/ /g' "$1" > "$1.n"; }

# --- Run compiler ---
echo -e "${YELLOW}Running compiler...${NC}"
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

if diff -Bw "$EXPECTED_OUTPUT.n" "$ACTUAL_OUTPUT.n" >/dev/null; then
    echo -e "${GREEN}✅ TEST PASSED${NC}"
else
    echo -e "${RED}❌ TEST FAILED${NC}"
    diff -Bw "$EXPECTED_OUTPUT.n" "$ACTUAL_OUTPUT.n"
    exit 1
fi

