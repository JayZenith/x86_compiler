
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>


enum class NodeType { IntLit, Ident, BinExpr, Let, Exit };

//Forward declare Node for recursive pointers
struct Node;

//Payloads 
struct NodeIntLit { int value; };
struct NodeIdent { std::string name; };
struct NodeBinExpr {
    std::string op;
    std::unique_ptr<Node> lhs; //all expr's are referred to by Node*
    std::unique_ptr<Node> rhs;
};

struct NodeLet {
    std::string name;
    std::unique_ptr<Node> expr; 
};

struct NodeExit {
    std::unique_ptr<Node> expr;
};

// Tagged union node
struct Node {
    NodeType type; //The tag 
    union { //All occupy the same memory space with taking largest possible 
        NodeIntLit int_lit;
        NodeIdent ident;
        NodeBinExpr bin_expr;
        NodeLet let_stmt;
        NodeExit exit_stmt;
    };

    // Constructors where i pass value/ref to contructor to another constructor
    // std::move transfers ownership, not copy and sets old ptr to nullptr 
    // and old string object in empty or unspecified state 
    Node(int v) : type(NodeType::IntLit), int_lit{v} {}
    Node(const std::string& n) : type(NodeType::Ident), ident{n} {}
    Node(std::string op, std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs)
        : type(NodeType::BinExpr), bin_expr{std::move(op), std::move(lhs), std::move(rhs)} {}
    Node(std::string name, std::unique_ptr<Node> expr)
        : type(NodeType::Let), let_stmt{std::move(name), std::move(expr)} {}
    Node(std::unique_ptr<Node> expr)
        : type(NodeType::Exit), exit_stmt{std::move(expr)} {}


    // Destructor: manually destroy active member
    ~Node() {
        switch (type) {
        case NodeType::IntLit: int_lit.~NodeIntLit(); break;
        case NodeType::Ident: ident.~NodeIdent(); break;
        case NodeType::BinExpr: bin_expr.~NodeBinExpr(); break;
        case NodeType::Let: let_stmt.~NodeLet(); break;
        case NodeType::Exit: exit_stmt.~NodeExit(); break;
        }
    }

     // Disable copy (for now)
    // Any attempt for Node a = b; or a = b; fails as 
    // node contains union-like members and copying unions
    // safely is tricky as you need to know which member is active.
    // = delete removes default copy constructor generation
    Node(const Node&) = delete; //Specifically delete passing a Node in constructor
    Node& operator=(const Node&) = delete; //Specifically rid of assigning nodes 

    // Enable move constructor
    /* Constructs new Node by "stealing" resources from other instead 
    of copying them. Uses placement new to construct active union member in place.
    std::move(other.xxx) allows internal members (like std::string, std::vector) to 
    transfer ownserhip instead of copying

    Node a(NodeIntLit(42));
    Node b(std::move(a)); // ✅ Move constructor: b now owns the IntLit, a is in a valid but unspecified state
    */
    Node(Node&& other) noexcept : type(other.type) {
        switch (type) {
        case NodeType::IntLit: new(&int_lit) NodeIntLit(std::move(other.int_lit)); break;
        case NodeType::Ident: new(&ident) NodeIdent(std::move(other.ident)); break;
        case NodeType::BinExpr: new(&bin_expr) NodeBinExpr(std::move(other.bin_expr)); break;
        case NodeType::Let: new(&let_stmt) NodeLet(std::move(other.let_stmt)); break;
        case NodeType::Exit: new(&exit_stmt) NodeExit(std::move(other.exit_stmt)); break;
        }
    }
    //This avoids expensive deep copies of strings, AST nodes, vectors, etc.

    //Move assignment
    Node& operator=(Node&& other) noexcept {
        if (this != &other) { //prevent self-move (which would otherwise destroy the object before moving).
            this->~Node();   // Destroy current contents
            new (this) Node(std::move(other)); // Move-construct in place
        }
        return *this;
    }
    /* 
    Node a(NodeIntLit(5));
    Node b(NodeIdent("x"));

    b = std::move(a); // ✅ b now becomes IntLit(5), a is in a valid but unspecified state
    */
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

    std::vector<Node> parse_program() {
        std::vector<Node> stmts;
        while (!match(TokenType::Eof)) {
            //For let x = 2; I get Let Node with string pointing to ident, and expr pointing to int lit
            if (check(TokenType::Let)) stmts.push_back(parse_let()); //Will be a Node with a ident string and Node ptr to expression
            else if (check(TokenType::Exit)) stmts.push_back(parse_exit()); //Just a Node with a Node ptr to expression
        }
        return stmts;
    }

private:
    size_t m_index = 0;
    const std::vector<Token>& m_tokens;

    bool match(TokenType t) { return m_tokens[m_index].type == t; }
    bool check(TokenType t) { return m_tokens[m_index].type == t; }
    Token consume() { return m_tokens[m_index++]; }

    Node parse_expr() { //Specifically to look for unary or binary expression
        auto lhs = std::make_unique<Node>(parse_primary()); //deduces to std::unique_ptr<Node>
        while (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star) || check(TokenType::Slash)) {
            std::string op = consume().value;
            auto rhs = std::make_unique<Node>(parse_primary());
            // make_unique creates a std:unique_ptr<NodeBinExpr>
            // C++ defines converting move constructor and assignment for 
            // unique_ptr when pointee type is convertible. 
            lhs = std::make_unique<Node>(Node(op, std::move(lhs), std::move(rhs)));
        }
        return std::move(*lhs); //have to dereference since returning: Node
    }

    Node parse_primary() {
        if (check(TokenType::IntLit)) return Node(std::stoi(consume().value)); 
        if (check(TokenType::Ident)) return Node(consume().value);

        throw std::runtime_error("Unexpected token in primary");
    }

    Node parse_let() {
        consume(); // let
        std::string name = consume().value;
        consume(); // =
        auto expr = std::make_unique<Node>(parse_expr()); //Node that expr's start use of pointers 
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



