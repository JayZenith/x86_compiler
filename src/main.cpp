#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

#include "tokenization.hpp"  
#include "parser.hpp"        
#include "generation.hpp"    

#include <chrono>

std::vector<Token> generate_tokens(int num_vars);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file> [--benchmark]\n";
        return 1;
    }

    std::string filename = argv[1];
    bool benchmark = false;

    // Check optional benchmark flag
    if (argc >= 3 && std::string(argv[2]) == "--benchmark") {
        benchmark = true;
    }

    std::string contents;
    { 
        std::stringstream ss;
        std::ifstream input(filename);
        ss << input.rdbuf();
        contents = ss.str();
    } // input's destructor runs to close file 


    Tokenizer tokenizer(std::move(contents));

    std::vector<Token> tokens;
    if (benchmark) {
        int num_vars = 1000000; // scale as needed for benchmarking
        tokens = generate_tokens(num_vars);
    } else {
        tokens = tokenizer.tokenize();
    }

    // Parse into AST
    Parser parser(tokens); 
    
    auto start = std::chrono::high_resolution_clock::now();
    auto program = parser.parse_program();
    // NASM assembly backend 
    Generator gen(program);  
    std::string asm_code = gen.generate();
    auto end = std::chrono::high_resolution_clock::now();

    if (benchmark) {
        std::cout << "Tagged-union AST time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms\n";
    }

    // Write to file
    std::ofstream out("nasm_out.s");
    out << asm_code;
    out.close();

    std::cout << "Assembly written to nasm_out.s\n";
    std::cout << "Run with:\n";
    std::cout << "  nasm -f elf64 nasm_out.s -o nasm_out.o && ld nasm_out.o -o nasm_out && ./nasm_out\n";
    return 0;
}

std::vector<Token> generate_tokens(int num_vars) {
    std::vector<Token> tokens;

    // Generate let statements: let x0 = 1 + 2; let x1 = x0 + 3; ...
    for (int i = 0; i < num_vars; ++i) {
        tokens.push_back({TokenType::Let, "let"}); // let
        tokens.push_back({TokenType::Ident, "x" + std::to_string(i)}); // identifier
        tokens.push_back({TokenType::Eq, "="}); // =

        // left operand
        if (i == 0)
            tokens.push_back({TokenType::IntLit, "2"});
        else
            tokens.push_back({TokenType::Ident, "x" + std::to_string(i - 1)});

        tokens.push_back({TokenType::Plus, "+"}); // operator
        tokens.push_back({TokenType::IntLit, std::to_string(3 + i)});// right operand
        tokens.push_back({TokenType::Semi, ";"}); // ;
    }

    // exit statement: exit xN;
    tokens.push_back({TokenType::Exit, "exit"});
    tokens.push_back({TokenType::Ident, "x" + std::to_string(num_vars - 1)});
    tokens.push_back({TokenType::Semi, ";"});

    tokens.push_back({TokenType::Eof, ""});
    return tokens;
}




