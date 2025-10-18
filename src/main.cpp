#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

#include "tokenization.hpp"  // must define Token + TokenType
#include "parser.hpp"        // your AST + Parser
#include "generation.hpp"     // we’ll assume you have a Generator class

// #include "polyparser.hpp"
// #include "polygeneration.hpp"
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

    // Tokenize
    Tokenizer tokenizer(std::move(contents));
    // auto tokens = tokenizer.tokenize();//deduced to vector<Token> w

    std::vector<Token> tokens;
    if (benchmark) {
        int num_vars = 100000; // scale as needed for benchmarking
        tokens = generate_tokens(num_vars);
    } else {
        tokens = tokenizer.tokenize();
    }
    // Parse into AST
    Parser parser(tokens); 
    
    // PolyParser polyparser(tokens);

    auto start = std::chrono::high_resolution_clock::now();
    //deduced to std::vector<PolyNode> 
    auto program = parser.parse_program();
    // NASM assembly backend 
    Generator gen(program);  // assume your generator takes AST PolyNodes
    std::string asm_code = gen.generate();
    auto end = std::chrono::high_resolution_clock::now();

    if (benchmark) {
        std::cout << "Tagged-union AST time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms\n";
    }

    // auto start2 = std::chrono::high_resolution_clock::now();
    // //deduced to std::vector<PolyNode> 
    // auto program2 = polyparser.parse_program();
    // // NASM assembly backend 
    // PolyGenerator gen2(program2);  // assume your generator takes AST PolyNodes
    // std::string asm_code2 = gen2.generate();
    // auto end2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed2 = end2 - start2;
    // std::cout << "Poly AST time: "
    //           << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count()
    //           << " ms\n";

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
        // let
        tokens.push_back({TokenType::Let, "let"});
        // identifier
        tokens.push_back({TokenType::Ident, "x" + std::to_string(i)});
        // =
        tokens.push_back({TokenType::Eq, "="});

        // left operand
        if (i == 0)
            tokens.push_back({TokenType::IntLit, "2"});
        else
            tokens.push_back({TokenType::Ident, "x" + std::to_string(i - 1)});

        // operator
        tokens.push_back({TokenType::Plus, "+"});

        // right operand
        tokens.push_back({TokenType::IntLit, std::to_string(3 + i)});

        // ;
        tokens.push_back({TokenType::Semi, ";"});
    }

    // exit statement: exit xN;
    tokens.push_back({TokenType::Exit, "exit"});
    tokens.push_back({TokenType::Ident, "x" + std::to_string(num_vars - 1)});
    tokens.push_back({TokenType::Semi, ";"});

    // EOF
    tokens.push_back({TokenType::Eof, ""});

    return tokens;
}

// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <memory>
// #include <vector>
// #include <string>
// #include <chrono>

// #include "tokenization.hpp"  // must define Token + TokenType
// #include "parser.hpp"        // your AST + Parser
// #include "generation.hpp"     // we’ll assume you have a Generator class
// // #include "llvm_generator.hpp"   // your LLVM-based Generator

// //LLVM headers 
// // #include "llvm/IR/LLVMContext.h"
// // #include "llvm/IR/Module.h"
// // #include "llvm/IR/IRBuilder.h"
// // #include "llvm/IR/Verifier.h"
// // #include "llvm/Support/raw_ostream.h"
// // #include "llvm/Support/FileSystem.h"



// int main(int argc, char* argv[]) {
//     if (argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <source_file> [--llvm]\n";
//         return 1;
//     }

//     std::string filename = argv[1];

//     bool useLLVM = false;
//     if (argc >= 3 && std::string(argv[2]) == "--llvm") {
//         useLLVM = true;
//     }


//     std::string contents;
//     {
//         std::stringstream ss;
//         std::ifstream input(filename);
//         ss << input.rdbuf();
//         contents = ss.str();
//     }

//     // 1. Tokenize
//     Tokenizer tokenizer(std::move(contents));
//     auto tokens = tokenizer.tokenize();  // you must have implemented this
//     tokens.push_back({TokenType::Eof, ""});


//     // 2. Parse into AST
//     Parser parser(tokens);
//     //deduced to std::vector<std::unique_ptr<PolyNode>> 

//        auto start = std::chrono::high_resolution_clock::now();
//     auto program = parser.parse_program();


//     if (useLLVM) {
//         // 4a. LLVM IR backend
//         // llvm::LLVMContext context;
//         // auto module = std::make_unique<llvm::Module>("toy_module", context);
//         // llvm::IRBuilder<> builder(context);

//         // LLVMGenerator gen(program, context, module.get(), builder);
//         // gen.codegen_program();

//         // // verify
//         // if (llvm::verifyModule(*module, &llvm::errs())) {
//         //     std::cerr << "LLVM IR verification failed!\n";
//         //     return 1;
//         // }
//         // // output
//         // std::error_code EC;
//         // llvm::raw_fd_ostream outFile("out.ll", EC, llvm::sys::fs::OF_None);
//         // if (EC) { std::cerr << "Error opening out.ll: " << EC.message() << "\n"; return 1; }
//         // module->print(outFile, nullptr);
//         // outFile.close();

//         // std::cout << "LLVM IR written to out.ll\n";
//         // std::cout << "Compile & run:\n";
//         // std::cout << "  llc -filetype=obj out.ll -o out.o\n";
//         // std::cout << "  clang out.o -o out\n";
//         // std::cout << "  ./out\n";
//     } else {

//         // NASM assembly backend 
//         Generator gen(program);  // assume your generator takes AST PolyNodes
//         std::string asm_code = gen.generate();

//             auto end = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double> elapsed = end - start;
//         std::cout << "Elapsed time: " << elapsed.count() << " s\n";

//         // 4. Write to file
//         std::ofstream out("nasm_out.s");
//         out << asm_code;
//         out.close();

//         std::cout << "Assembly written to nasm_out.s\n";
//         std::cout << "Run with:\n";
//         std::cout << "  nasm -f elf64 nasm_out.s -o nasm_out.o && ld nasm_out.o -o nasm_out && ./nasm_out\n";
//     }

//     return 0;
// }



