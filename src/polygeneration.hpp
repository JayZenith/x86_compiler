

#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "parser.hpp"

class PolyGenerator {
public:
    explicit PolyGenerator(const std::vector<std::unique_ptr<PolyNode>>& prog) : m_prog(prog) {}

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
    const std::vector<std::unique_ptr<PolyNode>>& m_prog;
    std::stringstream m_output;
    std::unordered_map<std::string, int> m_vars;
    int m_stack_offset = 0;

    void gen_stmt(PolyNode* PolyNode) {
        switch (PolyNode->type) {
            case PolyNodeType::Let: {
                auto let_PolyNode = static_cast<PolyNodeLet*>(PolyNode);
                gen_expr(let_PolyNode->expr.get());
                m_output << "    push rax\n"; // push onto stack
                m_vars[let_PolyNode->name] = m_stack_offset++;
                break;
            }
            case PolyNodeType::Exit: {
                auto exit_PolyNode = static_cast<PolyNodeExit*>(PolyNode);
                gen_expr(exit_PolyNode->expr.get());
                m_output << "    mov rdi, rax\n";
                m_output << "    mov rax, 60\n";
                m_output << "    syscall\n";
                break;
            }
            default: break;
        }
    }

    void gen_expr(PolyNode* PolyNode) {
        switch (PolyNode->type) {
            case PolyNodeType::IntLit: {
                auto lit = static_cast<PolyNodeIntLit*>(PolyNode);
                m_output << "    mov rax, " << lit->value << "\n"; //move into rax
                break;
            }
            case PolyNodeType::Ident: {
                auto ident = static_cast<PolyNodeIdent*>(PolyNode);
                int offset = m_vars[ident->name];
                // load variable at stack slot (rsp + offset*8)
                m_output << "    mov rax, [rsp + " << offset * 8 << "]\n";
                break;
            }
            case PolyNodeType::BinExpr: {
                auto bin = static_cast<PolyNodeBinExpr*>(PolyNode);
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
