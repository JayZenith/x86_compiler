


#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "tokenization.hpp" // must define Token and TokenType


//Valid values ex) PolyNodeType::IntLit
enum class PolyNodeType { IntLit, Ident, BinExpr, Let, Exit };

struct PolyNode {
    PolyNodeType type; // Stores which kind of PolyNode this is (e.g., PolyNodeType::IntLit, PolyNodeType::Ident, etc.)
};

struct PolyNodeIntLit : PolyNode {
    int value;
    PolyNodeIntLit(int v) { type = PolyNodeType::IntLit; value = v; } // Set the PolyNode’s kind to "integer literal"
};

struct PolyNodeIdent : PolyNode {
    std::string name;
    PolyNodeIdent(const std::string& n) { type = PolyNodeType::Ident; name = n; }
};

/*Polymorphism via base class
FOr ex case) (5 + x) * (3 - 1) and lhs and rhs respectively
Thus pionters are needed to base class PolyNode 
- unique_ptr means this PolyNode exclusively owns children
- and when PolyNodeBinExpr destroyed, lhs and rhs are as well
- eliminates manual memory management and avoid leaks 
*/
struct PolyNodeBinExpr : PolyNode {
    std::string op;
    std::unique_ptr<PolyNode> lhs;
    std::unique_ptr<PolyNode> rhs;
    PolyNodeBinExpr(std::string o, std::unique_ptr<PolyNode> l, std::unique_ptr<PolyNode> r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) { type = PolyNodeType::BinExpr; }
};

struct PolyNodeLet : PolyNode {
    std::string name;
    std::unique_ptr<PolyNode> expr;
    PolyNodeLet(const std::string& n, std::unique_ptr<PolyNode> e) : name(n), expr(std::move(e)) { type = PolyNodeType::Let; }
};

struct PolyNodeExit : PolyNode {
    std::unique_ptr<PolyNode> expr;
    PolyNodeExit(std::unique_ptr<PolyNode> e) : expr(std::move(e)) { type = PolyNodeType::Exit; }
};

class PolyParser {
public:
    explicit PolyParser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

    std::vector<std::unique_ptr<PolyNode>> parse_program() {
        std::vector<std::unique_ptr<PolyNode>> stmts;
        while (!match(TokenType::Eof)) {
            if (check(TokenType::Let)) stmts.push_back(parse_let());
            else if (check(TokenType::Exit)) stmts.push_back(parse_exit());
        }
        return stmts;
    }

private:
    size_t m_index = 0;
    const std::vector<Token>& m_tokens;

    bool match(TokenType t) { return m_tokens[m_index].type == t; }
    bool check(TokenType t) { return m_tokens[m_index].type == t; }
    Token consume() { return m_tokens[m_index++]; }

    std::unique_ptr<PolyNode> parse_expr() {
        auto lhs = parse_primary(); //deduces to std::unique_ptr<PolyNode>
        while (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star) || check(TokenType::Slash)) {
            std::string op = consume().value;
            auto rhs = parse_primary();
            // make_unique creates a std:unique_ptr<PolyNodeBinExpr>
            // C++ defines converting move constructor and assignment for 
            // unique_ptr when pointee type is convertible. 
            lhs = std::make_unique<PolyNodeBinExpr>(op, std::move(lhs), std::move(rhs));
        }
        return lhs;
    }

    std::unique_ptr<PolyNode> parse_primary() {
        if (check(TokenType::IntLit)) return std::make_unique<PolyNodeIntLit>(std::stoi(consume().value));
        if (check(TokenType::Ident)) return std::make_unique<PolyNodeIdent>(consume().value);
        return nullptr;
    }

    std::unique_ptr<PolyNode> parse_let() {
        consume(); // let
        std::string name = consume().value;
        consume(); // =
        auto expr = parse_expr();
        consume(); // ;
        return std::make_unique<PolyNodeLet>(name, std::move(expr));
    }

    std::unique_ptr<PolyNode> parse_exit() {
        consume(); // exit
        auto expr = parse_expr();
        consume(); // ;
        return std::make_unique<PolyNodeExit>(std::move(expr));
    }
};

