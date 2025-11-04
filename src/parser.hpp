#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>

enum class NodeType { IntLit, Ident, BinExpr, Let, Exit };
struct Node; //Forward Declare 

//Payloads
struct NodeIntLit { int value; };
struct NodeIdent { std::string name; };

struct NodeBinExpr { std::string op; std::unique_ptr<Node> lhs; std::unique_ptr<Node> rhs; };
struct NodeLet { std::string name; std::unique_ptr<Node> expr; };
struct NodeExit { std::unique_ptr<Node> expr; };

// Tagged union node where compiler dosent know which member is active
// so manually call constructors and destructors 
struct Node { 
    NodeType type; 
    union { //All occupy the same memory space with space taking largest possible 
        NodeIntLit int_lit;
        NodeIdent ident;
        NodeBinExpr bin_expr;
        NodeLet let_stmt;
        NodeExit exit_stmt;
    };

    // Explicitly coded multiple constructors 
    Node(int v) : type(NodeType::IntLit), int_lit{v} {}
    Node(const std::string& n) : type(NodeType::Ident), ident{n} {}
    Node(std::string op, std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs)
        : type(NodeType::BinExpr), bin_expr{std::move(op), std::move(lhs), std::move(rhs)} {}
    Node(std::string name, std::unique_ptr<Node> expr)
        : type(NodeType::Let), let_stmt{std::move(name), std::move(expr)} {}
    Node(std::unique_ptr<Node> expr)
        : type(NodeType::Exit), exit_stmt{std::move(expr)} {}

    ~Node() {
        switch (type) {
        case NodeType::IntLit: int_lit.~NodeIntLit(); break;
        case NodeType::Ident: ident.~NodeIdent(); break;
        case NodeType::BinExpr: bin_expr.~NodeBinExpr(); break;
        case NodeType::Let: let_stmt.~NodeLet(); break;
        case NodeType::Exit: exit_stmt.~NodeExit(); break;
        }
    }

    // Disable copy constructor and assignment (for now)
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete; 

    // Enable move constructor since Node 
    // Pass rvalue reference to another Node and copy tag (NodeType) so new node knows which payload to construct
    // new(&int_lit) constructs a NodeIntLit object in-place at that address.
    // std::move(other.int_lit) steals the resources from the other node.
    Node(Node&& other) noexcept : type(other.type) { //noexcept for standard containers to safely optimize reallocations (move instaed of copy knowing wont throw)
        switch (type) { // copied other's type above 
        case NodeType::IntLit: new(&int_lit) NodeIntLit(std::move(other.int_lit)); break;
        case NodeType::Ident: new(&ident) NodeIdent(std::move(other.ident)); break;
        case NodeType::BinExpr: new(&bin_expr) NodeBinExpr(std::move(other.bin_expr)); break;
        case NodeType::Let: new(&let_stmt) NodeLet(std::move(other.let_stmt)); break;
        case NodeType::Exit: new(&exit_stmt) NodeExit(std::move(other.exit_stmt)); break;
        }
    }

    //Move assignment
    /* 
    Node a(NodeIntLit(5));
    Node b(NodeIdent("x"));
    b = std::move(a); //b now becomes IntLit(5), a is in a valid but unspecified state
    */
    Node& operator=(Node&& other) noexcept {
        if (this != &other) { //prevent self-move (which would otherwise destroy the object before moving).
            this->~Node();   // Free active payload, but object's memory still exists
            new (this) Node(std::move(other)); // Use the same memory this object already lives in, but reinitialize it by moving from other.
        }
        return *this; 
    }
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

    std::vector<Node> parse_program() {
        std::vector<Node> stmts;
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

    Node parse_expr() { 
        //make_unique<Node> calls new Node(parse_primary()) triggering Node(Node&& other) as parse_primary returns temp rvalue Node
        //so compiler sees new Node(std::move(the temp Node)); 
        auto lhs = std::make_unique<Node>(parse_primary()); 
        while (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star) || check(TokenType::Slash)) {
            std::string op = consume().value;
            auto rhs = std::make_unique<Node>(parse_primary());
            lhs = std::make_unique<Node>(Node(op, std::move(lhs), std::move(rhs)));
        }
        // *lhs is a Node& (reference to the Node that lhs points to)
        // std::move(*lhs) casts that Node& to Node&& (an rvalue reference), meaning “we can move from it”
        // Returning std::move(*lhs) from a function that returns Node (by value)
        // invoked Node(Node&&)to build return value 
        return std::move(*lhs); 
    }

    Node parse_primary() {
        // Function creates temporary (rvalue) Node object on stack 
        if (check(TokenType::IntLit)) return Node(std::stoi(consume().value)); 
        if (check(TokenType::Ident)) return Node(consume().value);
        throw std::runtime_error("Unexpected token in primary");
    }

    Node parse_let() {
        consume(); // let
        std::string name = consume().value; 
        consume(); // =
        auto expr = std::make_unique<Node>(parse_expr()); 
        consume(); // ;
        return Node(std::move(name), std::move(expr));
    }

    Node parse_exit() {
        consume(); // exit
        auto expr = std::make_unique<Node>(parse_expr());
        consume(); // ;
        return Node(std::move(expr));    
    }
};



