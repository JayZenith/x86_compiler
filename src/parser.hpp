#include "tokenization.hpp" // must define Token and TokenType

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>

//Valid values ex) NodeType::IntLit
enum class NodeType { IntLit, Ident, BinExpr, Let, Exit };

struct Node {
    NodeType type; // Stores which kind of node this is (e.g., NodeType::IntLit, NodeType::Ident, etc.)
};

struct NodeIntLit : Node {
    int value;
    NodeIntLit(int v) { type = NodeType::IntLit; value = v; } // Set the node’s kind to "integer literal"
};

struct NodeIdent : Node {
    std::string name;
    NodeIdent(const std::string& n) { type = NodeType::Ident; name = n; }
};

/*Polymorphism via base class
FOr ex case) (5 + x) * (3 - 1) and lhs and rhs respectively
Thus pionters are needed to base class Node 
- unique_ptr means this node exclusively owns children
- and when NodeBinExpr destroyed, lhs and rhs are as well
- eliminates manual memory management and avoid leaks 
*/
struct NodeBinExpr : Node {
    std::string op;
    std::unique_ptr<Node> lhs;
    std::unique_ptr<Node> rhs;
    NodeBinExpr(std::string o, std::unique_ptr<Node> l, std::unique_ptr<Node> r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) { type = NodeType::BinExpr; }
};

struct NodeLet : Node {
    std::string name;
    std::unique_ptr<Node> expr;
    NodeLet(const std::string& n, std::unique_ptr<Node> e) : name(n), expr(std::move(e)) { type = NodeType::Let; }
};

struct NodeExit : Node {
    std::unique_ptr<Node> expr;
    NodeExit(std::unique_ptr<Node> e) : expr(std::move(e)) { type = NodeType::Exit; }
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

    std::vector<std::unique_ptr<Node>> parse_program() {
        std::vector<std::unique_ptr<Node>> stmts;
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

    std::unique_ptr<Node> parse_expr() {
        auto lhs = parse_primary(); //deduces to std::unique_ptr<Node>
        while (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star) || check(TokenType::Slash)) {
            std::string op = consume().value;
            auto rhs = parse_primary();
            // make_unique creates a std:unique_ptr<NodeBinExpr>
            // C++ defines converting move constructor and assignment for 
            // unique_ptr when pointee type is convertible. 
            lhs = std::make_unique<NodeBinExpr>(op, std::move(lhs), std::move(rhs));
        }
        return lhs;
    }

    std::unique_ptr<Node> parse_primary() {
        if (check(TokenType::IntLit)) return std::make_unique<NodeIntLit>(std::stoi(consume().value));
        if (check(TokenType::Ident)) return std::make_unique<NodeIdent>(consume().value);
        return nullptr;
    }

    std::unique_ptr<Node> parse_let() {
        consume(); // let
        std::string name = consume().value;
        consume(); // =
        auto expr = parse_expr();
        consume(); // ;
        return std::make_unique<NodeLet>(name, std::move(expr));
    }

    std::unique_ptr<Node> parse_exit() {
        consume(); // exit
        auto expr = parse_expr();
        consume(); // ;
        return std::make_unique<NodeExit>(std::move(expr));
    }
};

