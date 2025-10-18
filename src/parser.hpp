#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>

enum class NodeType { IntLit, Ident, BinExpr, Let, Exit };
struct Node;

struct NodeIntLit { int value; };
struct NodeIdent { std::string name; };
struct NodeBinExpr { std::string op; std::unique_ptr<Node> lhs; std::unique_ptr<Node> rhs; };
struct NodeLet { std::string name; std::unique_ptr<Node> expr; };
struct NodeExit { std::unique_ptr<Node> expr; };

// Tagged union node where compiler dosent know which member is active
// Thus, manually call constructors and destructors 
struct Node { 
    NodeType type; 
    union { //All occupy the same memory space with taking largest possible 
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
    // Any attempt for Node a = b; or Node a(b) fails as node contains union-like members and copying unions safely is tricky as you need to know which member is active.
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete; 

    // Enable move constructor since Node contains expensive to copy members
    // Pass rvalue reference to another Node and copy tag (NodeType) so new node knows which payload to construct
    // new(&int_lit) constructs a NodeIntLit object in-place at that address.
    // std::move(other.int_lit) steals the resources from the other node.
    Node(Node&& other) noexcept : type(other.type) { //noexcept for standard contains to safely optimize reallocations (move instaed of copy knowing wont throw)
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
    // *this refers to b, this points to b so is &b. std::cout << this; prints the address of b.
    // std::cout << *this; prints the object b itself (invokes operator<< if overloaded, otherwise undefined formatting).
    // to access members of b through pointer, you'd uses this->member 
    // other is an lvalue reference to a, so other refers directly to a
    // &other is address of a so is &a
    // std::cout << other; prints value at a  and std::cout << &other prints address of a
    // std::cout << &&other; will not ocmpile as its not a valid expression
    Node& operator=(Node&& other) noexcept {
        if (this != &other) { //prevent self-move (which would otherwise destroy the object before moving).
            this->~Node();   // Free active payload, but object's memory still exists
            new (this) Node(std::move(other)); // “Use the same memory this object already lives in, but reinitialize it by moving from other.
        }
        return *this; //*this does not create a copy, just the object youre pointint to
        //if you want a copy, youd do Node copy = *this; to invoke copy constructor 
    }
    /*
    new (this) Node(std::move(other))
    -> calls Node(Node&& other)
        -> inside switch(type)
            -> calls new(&int_lit) NodeIntLit(std::move(other.int_lit))
    */

   //Final
   //this is a pointer to object so its a Node* 
   //*this is the object itself, but in C++, its an lvalue ref to the object, not a seperate copy
   //Node b;
   //Node& ref = *this; //ref refers to same object as *this
   //&*this would go back to the same address as this

};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

    std::vector<Node> parse_program() {
        std::vector<Node> stmts;
        while (!match(TokenType::Eof)) {
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

    Node parse_expr() { 
        //Below expands to something like std::unique_ptr<Node> lhs(new Node(parse_primary()));
        //So constructing a new Node no heap, using move constructor as parse_primary() returns a temp (rvalue)
        //triggers new(&int_lit) NodeIntLit(std::move(other.int_lit));
        //-> this pointers to newly allocated Node memory (inside make_unique)
        //-> other is temporary from parse_primary()
        //-> so heap-allocate NOde (lhs) has same payload (NodeIntLit) as one return from parse_primary()

        //fuck all that, std::make_unique<Node> calls new Node(parse_primary()) triggering the Node(Node&& other) move constructor
        //so compiler sees new Node(std::move(tempNode)); that switches on type and incurs placement new to steal 
        //payload from other.tempNode is in moved-from state and its destructor runs going out of scope but dosent destroy anything
        //meaningful as union payload was moved out.
        auto lhs = std::make_unique<Node>(parse_primary()); 
        while (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star) || check(TokenType::Slash)) {
            std::string op = consume().value;
            auto rhs = std::make_unique<Node>(parse_primary());
            // Below activates bin_expr as Payload for Node Construction.
            // so lhs is a unique_ptr<Node> lhs containing two sub-Nodes which could be a any payload 
            lhs = std::make_unique<Node>(Node(op, std::move(lhs), std::move(rhs)));
        }
        //*lhs returns reference to object it points to so &lhs or Node& ref = *lhs; so *lhs is Node&
        //std::move just says "treat reference as something you can move from" so Node&& (r value ref to Node)
        return std::move(*lhs); 
        //so have return type Node (by value) and expression returned is (Node&&) so use 
        //Node's move constructor calling Node(Node&& other) so move contents of *lhs to 
        //return value caller recieves
    }

    Node parse_primary() {
        // Ex) return Node(std::stoi("42"));
        // Calls constructor Node(int v) : type (NodeType::IntLit, int_lit{v}) {}
        // temporary Node object on stack of parse_primary() so temporary (rvalue)
        // lives long enough to move from it
        if (check(TokenType::IntLit)) return Node(std::stoi(consume().value)); 
        if (check(TokenType::Ident)) return Node(consume().value);

        throw std::runtime_error("Unexpected token in primary");
    }

    Node parse_let() {
        consume(); // let
        std::string name = consume().value; //value is a string member of Token Struct
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



