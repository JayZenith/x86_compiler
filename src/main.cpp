#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

#include "tokenization.hpp"  // must define Token + TokenType
#include "parser.hpp"        // your AST + Parser
#include "generation.hpp"     // we’ll assume you have a Generator class
#include "llvm_generator.hpp"   // your LLVM-based Generator

//LLVM headers 
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file> [--llvm]\n";
        return 1;
    }

    std::string filename = argv[1];

    bool useLLVM = false;
    if (argc >= 3 && std::string(argv[2]) == "--llvm") {
        useLLVM = true;
    }


    std::string contents;
    {
        std::stringstream ss;
        std::ifstream input(filename);
        ss << input.rdbuf();
        contents = ss.str();
    }

    // 1. Tokenize
    Tokenizer tokenizer(std::move(contents));
    auto tokens = tokenizer.tokenize();  // you must have implemented this
    tokens.push_back({TokenType::Eof, ""});

    // 2. Parse into AST
    Parser parser(tokens);
    //deduced to std::vector<std::unique_ptr<Node>> 
    auto program = parser.parse_program();


    if (useLLVM) {
        // 4a. LLVM IR backend
        llvm::LLVMContext context;
        auto module = std::make_unique<llvm::Module>("toy_module", context);
        llvm::IRBuilder<> builder(context);

        LLVMGenerator gen(program, context, module.get(), builder);
        gen.codegen_program();

        // verify
        if (llvm::verifyModule(*module, &llvm::errs())) {
            std::cerr << "LLVM IR verification failed!\n";
            return 1;
        }
        // output
        std::error_code EC;
        llvm::raw_fd_ostream outFile("out.ll", EC, llvm::sys::fs::OF_None);
        if (EC) { std::cerr << "Error opening out.ll: " << EC.message() << "\n"; return 1; }
        module->print(outFile, nullptr);
        outFile.close();

        std::cout << "LLVM IR written to out.ll\n";
        std::cout << "Compile & run:\n";
        std::cout << "  llc -filetype=obj out.ll -o out.o\n";
        std::cout << "  clang out.o -o out\n";
        std::cout << "  ./out\n";
    } else {

        // NASM assembly backend 
        Generator gen(program);  // assume your generator takes AST nodes
        std::string asm_code = gen.generate();

        // 4. Write to file
        std::ofstream out("nasm_out.s");
        out << asm_code;
        out.close();

        std::cout << "Assembly written to nasm_out.s\n";
        std::cout << "Run with:\n";
        std::cout << "  nasm -f elf64 nasm_out.s -o nasm_out.o && ld nasm_out.o -o nasm_out && ./nasm_out\n";
    }

    return 0;
}
