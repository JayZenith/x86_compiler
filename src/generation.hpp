#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "parser.hpp"

class Generator {
public:
    explicit Generator(const std::vector<std::unique_ptr<Node>>& prog) : m_prog(prog) {}

    std::string generate() {
        // _start as entry point for NASM 
        m_output << "global _start\n_start:\n";

        //.get() coming from std::unique_ptr and returns raw pointer to object 
        for (const auto& stmt : m_prog) {
            gen_stmt(stmt.get());
        }

        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";
        return m_output.str();
    }

private:
    const std::vector<std::unique_ptr<Node>>& m_prog;
    std::stringstream m_output;
    std::unordered_map<std::string, int> m_vars;
    int m_stack_offset = 0;

    void gen_stmt(Node* node) {
        switch (node->type) {
            case NodeType::Let: {
                auto let_node = static_cast<NodeLet*>(node);
                gen_expr(let_node->expr.get());
                m_output << "    push rax\n"; // push onto stack
                m_vars[let_node->name] = m_stack_offset++;
                break;
            }
            case NodeType::Exit: {
                auto exit_node = static_cast<NodeExit*>(node);
                gen_expr(exit_node->expr.get());
                m_output << "    mov rdi, rax\n";
                m_output << "    mov rax, 60\n";
                m_output << "    syscall\n";
                break;
            }
            default: break;
        }
    }

    void gen_expr(Node* node) {
        switch (node->type) {
            case NodeType::IntLit: {
                auto lit = static_cast<NodeIntLit*>(node);
                m_output << "    mov rax, " << lit->value << "\n"; //move into rax
                break;
            }
            case NodeType::Ident: {
                auto ident = static_cast<NodeIdent*>(node);
                int offset = m_vars[ident->name];
                // load variable at stack slot (rsp + offset*8)
                m_output << "    mov rax, [rsp + " << offset * 8 << "]\n";
                break;
            }
            case NodeType::BinExpr: {
                auto bin = static_cast<NodeBinExpr*>(node);
                gen_expr(bin->lhs.get());
                m_output << "    push rax\n";
                gen_expr(bin->rhs.get());
                m_output << "    pop rbx\n";
                if (bin->op == "+") m_output << "    add rax, rbx\n";
                if (bin->op == "-") m_output << "    sub rbx, rax\n    mov rax, rbx\n";
                if (bin->op == "*") m_output << "    imul rax, rbx\n";
                if (bin->op == "/") {
                    m_output << "    mov rdx, 0\n";
                    m_output << "    div rbx\n";
                }
                break;
            }
            default: break;
        }
    }
};

