#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include "parser.hpp" // Node, NodeType, NodeBinExpr, NodeLet, NodeExit, etc.

class Generator {
public:
    explicit Generator(const std::vector<Node>& program) : m_program(program) {}

    std::string generate() {
        std::ostringstream out;

        // 1) Emit .data section for all variables
        out << "section .data\n";
        for (const auto& stmt : m_program) collect_vars(stmt); //if variables exist
        for (const auto& kv : m_vars) { //go though variables 
            out << kv.first << " dq 0\n";
        }

        // 2) Emit .text section
        out << "\nsection .text\n"
            << "global _start\n\n"
            << "_start:\n";

        for (const auto& stmt : m_program) {
            gen_stmt(out, stmt);
        }

        return out.str(); // no extra exit needed; handled by Exit nodes
    }

private:
    const std::vector<Node>& m_program;
    std::unordered_map<std::string, bool> m_vars; // track variable names

    // collect variable names from let statements
    void collect_vars(const Node& node) {
        switch (node.type) {
        case NodeType::Let:
            m_vars[node.let_stmt.name] = true;
            collect_vars(*node.let_stmt.expr);
            break;
        case NodeType::BinExpr:
            collect_vars(*node.bin_expr.lhs);
            collect_vars(*node.bin_expr.rhs);
            break;
        case NodeType::Exit:
            collect_vars(*node.exit_stmt.expr);
            break;
        default:
            break;
        }
    }

    void gen_expr(std::ostringstream& out, const Node& node) {
    switch (node.type) {
        case NodeType::IntLit:
            out << "    mov rax, " << node.int_lit.value << "\n";
            break;
        case NodeType::Ident:
            out << "    mov rax, [" << node.ident.name << "]\n";
            break;
        case NodeType::BinExpr:
            gen_binexpr(out, node.bin_expr);
            break;
        case NodeType::Let:
            gen_stmt(out, node);   // allow let as expression
            break;
        case NodeType::Exit:
            gen_stmt(out, node);   // allow exit as expression
            break;
        default:
            break;
        }
    }


    void gen_stmt(std::ostringstream& out, const Node& node) {
        switch (node.type) {
        case NodeType::Let:
            gen_expr(out, *node.let_stmt.expr);   // rax = result
            out << "    mov [" << node.let_stmt.name << "], rax\n";
            break;
        case NodeType::Exit:
            gen_expr(out, *node.exit_stmt.expr);  // rax = result
            out << "    mov rdi, rax\n"
                << "    mov rax, 60\n"
                << "    syscall\n";
            break;
        default:
            gen_expr(out, node);
            break;
        }
    }

    void gen_binexpr(std::ostringstream& out, const NodeBinExpr& expr) {
        // lhs → rax
        gen_expr(out, *expr.lhs);
        out << "    mov rbx, rax\n"; // store lhs in rbx

        // rhs → rax
        gen_expr(out, *expr.rhs);

        // combine: rbx (lhs) op rax (rhs)
        if (expr.op == "+")
            out << "    add rax, rbx\n"; // rax = rhs + lhs
        else if (expr.op == "-")
            out << "    mov rcx, rax\n"   // rhs
                << "    mov rax, rbx\n"   // lhs
                << "    sub rax, rcx\n"; // lhs - rhs
        else if (expr.op == "*")
            out << "    imul rax, rbx\n"; 
        else if (expr.op == "/") {
            out << "    mov rcx, rax\n"  // rhs
                << "    mov rax, rbx\n"  // lhs
                << "    cqo\n"
                << "    idiv rcx\n";
        } else
            out << "    ; unknown op '" << expr.op << "'\n";
    }

};



// #pragma once
// #include <string>
// #include <sstream>
// #include <vector>
// #include <unordered_map>
// #include "parser.hpp"

// class Generator {
// public:
//     explicit Generator(const std::vector<std::unique_ptr<Node>>& prog) : m_prog(prog) {}

//     std::string generate() {
//         // _start as entry point for NASM 
//         m_output << "global _start\n_start:\n";

//         //.get() coming from std::unique_ptr and returns raw pointer to object 
//         for (const auto& stmt : m_prog) {
//             gen_stmt(stmt.get());
//         }

//         m_output << "    mov rax, 60\n";
//         m_output << "    mov rdi, 0\n";
//         m_output << "    syscall\n";
//         return m_output.str();
//     }

// private:
//     const std::vector<std::unique_ptr<Node>>& m_prog;
//     std::stringstream m_output;
//     std::unordered_map<std::string, int> m_vars;
//     int m_stack_offset = 0;

//     void gen_stmt(Node* node) {
//         switch (node->type) {
//             case NodeType::Let: {
//                 auto let_node = static_cast<NodeLet*>(node);
//                 gen_expr(let_node->expr.get());
//                 m_output << "    push rax\n"; // push onto stack
//                 m_vars[let_node->name] = m_stack_offset++;
//                 break;
//             }
//             case NodeType::Exit: {
//                 auto exit_node = static_cast<NodeExit*>(node);
//                 gen_expr(exit_node->expr.get());
//                 m_output << "    mov rdi, rax\n";
//                 m_output << "    mov rax, 60\n";
//                 m_output << "    syscall\n";
//                 break;
//             }
//             default: break;
//         }
//     }

//     void gen_expr(Node* node) {
//         switch (node->type) {
//             case NodeType::IntLit: {
//                 auto lit = static_cast<NodeIntLit*>(node);
//                 m_output << "    mov rax, " << lit->value << "\n"; //move into rax
//                 break;
//             }
//             case NodeType::Ident: {
//                 auto ident = static_cast<NodeIdent*>(node);
//                 int offset = m_vars[ident->name];
//                 // load variable at stack slot (rsp + offset*8)
//                 m_output << "    mov rax, [rsp + " << offset * 8 << "]\n";
//                 break;
//             }
//             case NodeType::BinExpr: {
//                 auto bin = static_cast<NodeBinExpr*>(node);
//                 gen_expr(bin->lhs.get());
//                 m_output << "    push rax\n";
//                 gen_expr(bin->rhs.get());
//                 m_output << "    pop rbx\n";
//                 if (bin->op == "+") m_output << "    add rax, rbx\n";
//                 if (bin->op == "-") m_output << "    sub rbx, rax\n    mov rax, rbx\n";
//                 if (bin->op == "*") m_output << "    imul rax, rbx\n";
//                 if (bin->op == "/") {
//                     m_output << "    mov rdx, 0\n";
//                     m_output << "    div rbx\n";
//                 }
//                 break;
//             }
//             default: break;
//         }
//     }
// };
