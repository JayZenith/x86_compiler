#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "tokenization.hpp"  // must define Token + TokenType
#include "parser.hpp"        // your AST + Parser
#include "generation.hpp"     // we’ll assume you have a Generator class

int main(int argc, char* argv[]) {

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    // 1. Tokenize
    Tokenizer tokenizer(std::move(contents));
    auto tokens = tokenizer.tokenize();  // you must have implemented this
    tokens.push_back({TokenType::Eof, ""});

    // 2. Parse into AST
    Parser parser(tokens);
    //deduced to std::vector<std::unique_ptr<Node>> 
    auto program = parser.parse_program();

    // 3. Generate assembly
    Generator gen(program);  // assume your generator takes AST nodes
    std::string asm_code = gen.generate();

    // 4. Write to file
    std::ofstream out("nasm_out.s");
    out << asm_code;
    out.close();

    std::cout << "Assembly written to nasm_out.s\n";
    std::cout << "Run with:\n";
    std::cout << "  nasm -f elf64 nasm_out.s -o nasm_out.o && ld nasm_out.o -o nasm_out && ./nasm_out\n";

    return 0;
}
