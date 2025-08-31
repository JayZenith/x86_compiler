#pragma once

#include <variant>
#include "./tokenize.hpp"
#include "./arena.hpp"

//Terminals dont need a pointer 
struct NodeTermIntLit{
    Token int_lit;
};

struct NodeTermStringLit{
    Token string_lit;
};

struct NodeTermIdent{
    Token ident;
};

//Need to allude its formal definition as we use in BineExprAdd
//and BInExprMulti which they are used in NodeBinExpr 
struct NodeExpr;

//without the pointers, we would have to know the exact size of the 
//data types which is not possible, thus the pointers bypasses this as 
//we know theyre pointers to structs 

struct BinExprAdd{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

// struct BinExprMulti{    
//     NodeExpr* lhs;
//     NodeExpr* rhs;
// };

// struct NodeBinExpr{
//     std::variant<BinExprAdd*, BinExprMulti*> var;
// };

struct NodeBinExpr{
    BinExprAdd* add;
};

struct NodeTerm{
    std::variant<NodeTermIntLit*, NodeTermIdent*, NodeTermStringLit*> var;
};

struct NodeExpr{ //[expr] is either int_lit or IDENTIFIER
    std::variant<NodeTerm*, NodeBinExpr*> var;
};


struct NodeStmtLeave{  
    NodeExpr* expr;
};

struct NodeStmtLet{ 
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt{  
    std::variant<NodeStmtLeave*, NodeStmtLet*> var;
};

struct NodeRoot{  //[program]
    std::vector<NodeStmt*> stmts; //[statements]

};

class Parser{
public:
    Parser(std::vector<Token> &x)
        : theTokens(std::move(x)),
        allocator(1024 * 1024 * 4) //the arena 
        {}

    std::optional<NodeBinExpr*> parse_bin_expr(){
        if(auto lhs = parse_expr()){
            auto bin_expr = allocator.alloc<NodeBinExpr>();
            //if(peek().has_value() && peek().value().type == TokenType::PLUS){
            if(try_eat(TokenType::PLUS)){
                auto bin_expr_add = allocator.alloc<BinExprAdd>();
                bin_expr_add->lhs = lhs.value();
                //eat(); //consume addition token, ATE DURING try_eat
                //eat();
                if(auto rhs = parse_expr()){
                    bin_expr_add->rhs = rhs.value();
                    bin_expr->add = bin_expr_add;
                    return bin_expr;
                } else {
                    std::cerr << "check rhs" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cerr << "check lhs" << std::endl;
                exit(EXIT_FAILURE);
            }
            
        } else {
            return {};
        }
    }


    std::optional<NodeTerm*> parse_term(){
        //if(peek().value().type == TokenType::NUMBER){
        if(auto aterm = try_eat(TokenType::NUMBER)){
            auto term_int_lit = allocator.alloc<NodeTermIntLit>(); //alloc space of this struct within arena 
            //term_int_lit->int_lit = eat(); //must -> since its a pointer to the type 
            term_int_lit->int_lit = aterm.value(); //must -> since its a pointer to the type 
            auto term= allocator.alloc<NodeTerm>();
            term->var = term_int_lit; 
            return term;
            //return NodeExpr { .var = node_expr_int_lit };
            //return NodeExpr{ .var = NodeExprIntLit { .int_lit = eat() } }; //eating token
        }
        //else if(peek().value().type == TokenType::STRING){
            if(auto aterm = try_eat(TokenType::STRING)){
            auto term_string_lit = allocator.alloc<NodeTermStringLit>();
            //term_string_lit->string_lit = eat();
            term_string_lit->string_lit = aterm.value();
            auto term = allocator.alloc<NodeTerm>();
            term->var = term_string_lit;
            return term;
            //return NodeExpr{ .var = NodeExprStringLit { .string_lit = eat() } }; //eating token
        }
        //else if(peek().value().type == TokenType::IDENTIFIER){
        else if(auto aterm = try_eat(TokenType::IDENTIFIER)){
            auto term_ident = allocator.alloc<NodeTermIdent>();
            //term_ident->ident = eat();
            term_ident->ident = aterm.value();
            auto term = allocator.alloc<NodeTerm>();
            term->var = term_ident;
            return term;
            //return NodeExpr{ .var = NodeExprIdent { .ident = eat() } };
        } else {
            return {};
        }
    }

    //obtaining the terminals 
    std::optional<NodeExpr*> parse_expr(){ 
        if(auto term = parse_term()){
            if(peek().has_value() && peek().value().type == TokenType::PLUS){
                auto bin_expr = allocator.alloc<NodeBinExpr>();
                //if(peek().has_value() && peek().value().type == TokenType::PLUS){
                auto bin_expr_add = allocator.alloc<BinExprAdd>();
                auto lhs_expr = allocator.alloc<NodeExpr>();
                lhs_expr->var = term.value();
                bin_expr_add->lhs = lhs_expr;
                eat(); //consume addition token
                //eat();
                if(auto rhs = parse_expr()){
                    bin_expr_add->rhs = rhs.value();
                    bin_expr->add = bin_expr_add;
                    auto expr = allocator.alloc<NodeExpr>();
                    expr->var = bin_expr;
                    return expr;
                } else {
                    std::cerr << "check rhs" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            //} //otherwise
            else {
                auto expr = allocator.alloc<NodeExpr>();
                expr->var = term.value();
                return expr; //just return the term
            }
        } else {
            return {};
        }
        
        
    }
    
    //NodeStmt is a variant of the instruction types 
    std::optional<NodeStmt*> parse_stmt(){
        if(peek().value().type == TokenType::LEAVE && peek(1).has_value() 
            && peek(1).value().type == TokenType::LEFT_PAREN){
            eat();
            eat();
            //NodeStmtLeave stmt_leave; 
            auto stmt_leave = allocator.alloc<NodeStmtLeave>();

            if(auto node_expr = parse_expr()){
                //stmt_leave = {.expr = node_expr.value()}; //will eat() as well
                stmt_leave->expr = node_expr.value();
            }
            
            // if(peek().has_value() && peek().value().type == TokenType::RIGHT_PAREN){ 
            //     eat();
            // }
            try_eat(TokenType::RIGHT_PAREN);
            // if(peek().has_value() && peek().value().type == TokenType::SEMICOLON){
            //     eat();
            // }
            try_eat(TokenType::SEMICOLON);

            auto stmt = allocator.alloc<NodeStmt>();
            stmt->var = stmt_leave;
            return stmt;
            //return NodeStmt { .var = stmt_leave };

        } else if (peek().has_value() && peek().value().type == TokenType::LET &&
            peek(1).has_value() && peek(1).value().type == TokenType::IDENTIFIER && 
            peek(2).has_value() && peek(2).value().type == TokenType::EQUAL){
            eat(); //consume LET

            auto stmt_let = allocator.alloc<NodeStmtLet>();
            stmt_let->ident = eat();
            //auto stmt_let = NodeStmtLet { .ident = eat() }; //consume IDENTIFIER 
            
            eat(); //consume EQUAL 
            
            if (auto expr = parse_expr()){
                stmt_let->expr = expr.value();
                //stmt_let.expr = expr.value(); //had to .expr because has two data members in struct 
                //eat(); //dont we eat to look for SEMICOLON? no WE ATE DURING PARSING
            } else { //not an int val or IDENTIFIER?
                std::cerr << "Invalid parsed expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::SEMICOLON){
                eat();
            }
            auto stmt = allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
            //return NodeStmt { .var = stmt_let };
        } else{ 
            return {};
        }
        
    }

 
    std::optional<NodeRoot> parse_prog(){ 
        NodeRoot prog; //vector of stmts which holds variants
        //std::cout << "hi";

        while (peek().has_value()){ 
            if(auto stmt = parse_stmt()){
                prog.stmts.push_back(stmt.value());
                //std::cout << "hi";
            } else { //does not match current instructions supported 
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    std::vector<Token> theTokens;

    std::optional<Token> peek(int offset=0){
        if(idx + offset > theTokens.size() - 1){
            return {};
        } else { 
            return theTokens.at(idx+offset);
        }
    }

    Token eat(){ return theTokens.at(idx++); }

    void error(int line, std::string message){
        report(line, "", message);
    }

    void report(int line, std::string where, std::string message){
        std::cerr << "[line " << line << "] Error" << where << ": "
        << message << std::endl;
        hadError = true;
    }

    Token try_eat(TokenType type, std::string errMsg){
        if(peek().has_value() && peek().value().type == type){
            return eat();
        } else {
            std::cerr << errMsg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::optional<Token> try_eat(TokenType type){
        if(peek().has_value() && peek().value().type == type){
            return eat();
        } else {
            return {};
        }
    }


    size_t idx = 0;
    bool hadError = false;
    ArenaAllocator allocator;
};