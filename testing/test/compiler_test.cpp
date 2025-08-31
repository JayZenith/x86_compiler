#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>

// Include the compiler header if there's a C++ API
// #include "compiler.h"

namespace fs = std::filesystem;

class CompilerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Define the expected assembly output
        expected_assembly = R"(global _start
_start:
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
push QWORD [rsp + 0]
mov rax, 7
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
mov rax, 2
push rax
mov rax, 1
push rax
pop rax
pop rbx
add rax, rbx
push rax
push QWORD [rsp + 72]
mov rax, 60
pop rdi
syscall
mov rax, 60
mov rdi, 0
syscall)";

        // Normalize the expected output
        expected_assembly = normalizeAssembly(expected_assembly);
        
        // Create test files with the appropriate paths
        createTestFiles();
    }
    
    void TearDown() override {
        // Clean up temporary files
        if (fs::exists(input_file_path)) {
            fs::remove(input_file_path);
        }
        if (fs::exists(output_file_path)) {
            fs::remove(output_file_path);
        }
        if (fs::exists(obj_file_path)) {
            fs::remove(obj_file_path);
        }
    }
    
    std::string normalizeAssembly(const std::string& assembly) {
        std::istringstream stream(assembly);
        std::string line;
        std::ostringstream normalized;
        
        while (std::getline(stream, line)) {
            // Trim whitespace from the beginning and end of the line
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (!line.empty()) {
                normalized << line << '\n';
            }
        }
        return normalized.str();
    }
    
    void createTestFiles() {
        // Create unique temporary filenames
        std::string temp_dir = fs::temp_directory_path().string();
        input_file_path = temp_dir + "/compiler_test_input.forth";
        output_file_path = temp_dir + "/compiler_test_output.asm";
        obj_file_path = temp_dir + "/compiler_test_output.o";
        
        // Write input code to file
        std::ofstream input_file(input_file_path);
        if (input_file.is_open()) {
            input_file << "2 1 + dup 7 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + pick exit";
            input_file.close();
        } else {
            FAIL() << "Failed to create input file at: " << input_file_path;
        }
    }
    
    // Returns the path to the compiler executable
    std::string getCompilerPath() {
        // Try to find the compiler executable in common locations
        std::vector<std::string> possible_paths = {
            "./x86_compiler",
            "./build/x86_compiler",
            "../build/x86_compiler",
            "../src/main"
        };
        
        for (const auto& path : possible_paths) {
            if (fs::exists(path)) {
                return path;
            }
        }
        
        // If not found, assume it's in PATH
        return "x86_compiler";
    }
    
    std::string expected_assembly;
    std::string input_file_path;
    std::string output_file_path;
    std::string obj_file_path;
};

// Test to verify compiler output matches expected assembly
TEST_F(CompilerTest, CompilerOutputMatchesExpected) {
    // Build the command to run the compiler
    std::string compiler_path = getCompilerPath();
    std::string command = compiler_path + " \"" + input_file_path + "\" -o \"" + output_file_path + "\"";
    
    // Run the compiler
    int result = std::system(command.c_str());
    ASSERT_EQ(result, 0) << "Compiler execution failed with command: " << command;
    
    // Check if output file was created
    ASSERT_TRUE(fs::exists(output_file_path)) << "Output file was not created at: " << output_file_path;
    
    // Read the output file
    std::ifstream output_file(output_file_path);
    ASSERT_TRUE(output_file.is_open()) << "Failed to open output file at: " << output_file_path;
    
    std::stringstream buffer;
    buffer << output_file.rdbuf();
    std::string generated_assembly = buffer.str();
    
    // Normalize the generated assembly
    generated_assembly = normalizeAssembly(generated_assembly);
    
    // Compare with expected assembly
    EXPECT_EQ(generated_assembly, expected_assembly) << "Generated assembly does not match expected assembly";
}

// Test to verify the generated assembly can be assembled with NASM
TEST_F(CompilerTest, AssemblyCanBeAssembledWithNASM) {
    // First, create the assembly file using the expected assembly
    std::ofstream asm_file(output_file_path);
    ASSERT_TRUE(asm_file.is_open()) << "Failed to create assembly file at: " << output_file_path;
    asm_file << expected_assembly;
    asm_file.close();
    
    // Check if NASM is available
    int check_nasm = std::system("which nasm > /dev/null 2>&1");
    if (check_nasm != 0) {
        GTEST_SKIP() << "NASM not found, skipping assembly test";
    }
    
    // Try to assemble the file with NASM
    std::string nasm_command = "nasm -f elf64 \"" + output_file_path + "\" -o \"" + obj_file_path + "\"";
    int result = std::system(nasm_command.c_str());
    
    EXPECT_EQ(result, 0) << "NASM assembly failed with command: " << nasm_command;
    EXPECT_TRUE(fs::exists(obj_file_path)) << "Object file was not created at: " << obj_file_path;
    
    // Check if the object file has content
    EXPECT_GT(fs::file_size(obj_file_path), 0) << "Object file is empty";
}

// If there's a C++ API for the compiler, add a test for direct API usage
// TEST_F(CompilerTest, DirectApiTest) {
//     // Example of how to test using the compiler's C++ API
//     std::string input_code = "2 1 + dup 7 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + 2 1 + pick exit";
//     std::string generated_assembly = Compiler::compile(input_code);
//     generated_assembly = normalizeAssembly(generated_assembly);
//     EXPECT_EQ(generated_assembly, expected_assembly);
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}