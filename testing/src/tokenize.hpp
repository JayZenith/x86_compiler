#pragma once

#include <optional>
#include <iostream>
#include <vector>
#include <string>
#include <map>

enum class TokenType{
    // Single-character TokenType.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,  SEMICOLON, 
    DOT, MINUS, PLUS, SLASH, STAR, COMMA,

    // One or two character TokenType.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    //literals.
    NUMBER, IDENTIFIER, STRING,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, LET, VAR, WHILE,
    LEAVE, 
    };

struct Token{
    TokenType type;
    std::optional<std::string> value {}; 
    int theLine; //hold lne where instruction is 
};

std::map<std::string, TokenType> HashMap = 
{ {"leave", TokenType::LEAVE},
  {"and", TokenType::AND},
  {"class", TokenType::CLASS},
  {"else", TokenType::ELSE},
  {"false", TokenType::FALSE},
  {"fun", TokenType::FUN},
  {"for", TokenType::FOR},
  {"if", TokenType::IF},
  {"nil", TokenType::NIL},
  {"or", TokenType::OR},
  {"print", TokenType::PRINT},
  {"return", TokenType::RETURN},
  {"super", TokenType::SUPER},
  {"this", TokenType::THIS},
  {"true", TokenType::TRUE},
  {"let", TokenType::LET},
  {"var", TokenType::VAR},
  {"while", TokenType::WHILE},
};

class Tokenize{
public:
    Tokenize(std::string&contents)
        : theContents(std::move(contents))
        {}

    std::vector<Token>tokenize(){
        std::vector<Token> TokenType;
        std::string buf = "";

        while(peek().has_value()){ //stops at eof, in which returns null 
            //assume lexeme starting with letter or underscore is an identifier 
            if(isalpha(peek().value())){ 
                identifier(buf, TokenType);
                /*
                while(peek().has_value() && isalnum(peek().value())){ //will figure out the start of a statement later on
                    buf.push_back(eat()); //push onto buffer and increment 
                }
                if(buf == "leave"){
                    TokenType.push_back({ .type = TokenType::LEAVE, .theLine=line});
                    buf.clear();
                    continue;
                } else if(buf == "let"){
                    TokenType.push_back({ .type = TokenType::LET, .theLine=line});
                    buf.clear();
                    continue;
                } else { //.value will hold the idnetifier symbol 
                    TokenType.push_back({ .type = TokenType::IDENTIFIER, .value=buf, .theLine=line });
                    buf.clear();
                    continue;
                }
                */
               buf.clear();
               continue;
            }
            
            else if(isspace(peek().value())){ //detects newlines
                if(peek().value() == '\n'){
                    line++; 
                }
                eat();
                //TokenType.push_back({ .type = TokenType::RIGHT_PAREN});
                buf.clear();
                continue;
            }
            else if(peek().value() == '"'){
                string(buf, TokenType);
                buf.clear();
                continue;
            }
            else if(peek().value() == '('){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::LEFT_PAREN, .theLine=line});
                buf.clear();
                continue;
            }
            else if(isdigit(peek().value())){
                number(buf, TokenType);
                /*
                while(isdigit(peek().value())){ //8, 24, 255
                    buf.push_back(eat());
                }
                TokenType.push_back({ .type = TokenType::NUMBER, .value=buf, .theLine=line});
                */
                buf.clear();
                continue;
            }
            else if(peek().value() == ')'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::RIGHT_PAREN, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '{'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::LEFT_BRACE, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '}'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::RIGHT_BRACE, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '*'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::STAR, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '+'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::PLUS, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '-'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::MINUS, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == ';'){
                buf.push_back(eat());
                TokenType.push_back({ .type = TokenType::SEMICOLON, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '='){
                buf.push_back(eat());
                match('=') ? TokenType.push_back({ .type = TokenType::EQUAL_EQUAL, .theLine=line})
                    : TokenType.push_back({ .type = TokenType::EQUAL, .theLine=line});
                buf.clear();

                continue;
            }
            else if(peek().value() == '!'){
                buf.push_back(eat()); //dont need to push back on buffer
                match('=') ? TokenType.push_back({ .type = TokenType::BANG_EQUAL, .theLine=line})
                    : TokenType.push_back({ .type = TokenType::BANG, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '<'){
                buf.push_back(eat()); //dont need to push back on buffer
                match('=') ? TokenType.push_back({ .type = TokenType::LESS_EQUAL, .theLine=line})
                    : TokenType.push_back({ .type = TokenType::LESS, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '>'){
                buf.push_back(eat()); //dont need to push back on buffer
                match('=') ? TokenType.push_back({ .type = TokenType::GREATER_EQUAL, .theLine=line})
                    : TokenType.push_back({ .type = TokenType::GREATER, .theLine=line});
                buf.clear();
                continue;
            }
            else if(peek().value() == '/'){
                buf.push_back(eat()); //dont need to push back on buffer
                if (match('/')){
                    //need to track newline carefully to also increement line 
                    //if does equal newline, we continue
                    while(peek().has_value() && peek().value() != '\n'){
                        eat();
                    }
                   
                } else {
                    TokenType.push_back({ .type = TokenType::SLASH, .theLine=line});
                }
                buf.clear();
                continue;
            }
        }
        idx = 0;
        return TokenType;
    }

private:
    std::string theContents = "";

    std::optional<char> peek(int offset=0){ //
        if(theContents[idx+offset]){
            return theContents[idx+offset];
        } else {
            return {};
        }
    }

    char eat(){
        return theContents[idx++];
    }

    bool match(char expected){
        //the previous eat() causes peek to already look at expected
        if(peek().has_value()){ //check for following character, but what if whitespace?
        //I think we are fine, because we ex out eof, but now specifically check for char
            if(peek().value() != expected){
                return false;
            }
        } else {
            return false;
        }

        eat(); //eat the expected char
        return true;
    }
    
    void string(std::string & bufa, std::vector<Token>&TokenType){
        eat(); //eat the " but dont put in buffer 
        while(peek().has_value() && peek(1).has_value() && peek(1).value() != '"'){
            if(peek().value() == '\n') line++;
            /*
            let x = "
            fsdfsdf";
            */
            bufa.push_back(eat());
        }

        if(!peek().has_value()){//we are at eof
        //but what if theres a start of another string
        //and then there would still be one quotation missing
        //but what if theres two quotations missing, making it even
        //Then thats actually a valid string!!!!!!! LOL
            std::cerr << "Unterminated String.";
            exit(EXIT_FAILURE);
        
        }

        if(peek(1).value() == '"'){
            //if next token is closing quote
            bufa.push_back(eat());
            TokenType.push_back({ .type = TokenType::STRING, .value=bufa, .theLine=line});
            eat(); //eat the right quotation
        }
        
    }

    void number(std::string & bufa, std::vector<Token>&TokenType){
        while(isdigit(peek().value())) bufa.push_back(eat());
        //breaks out of while if hit '.'
        if(peek().value() == '.' && isdigit(peek(1).value())){
            bufa.push_back(eat());

            while(isdigit(peek().value())) bufa.push_back(eat());
        }

        TokenType.push_back({ .type = TokenType::NUMBER, .value=bufa, .theLine=line});
    }



    void identifier(std::string & bufa, std::vector<Token>&TokenType){
        while(peek().has_value() && isalnum(peek().value())){ //will figure out the start of a statement later on
            bufa.push_back(eat()); //push onto bufafer and increment 
        }
        /*
        if(bufa == "leave"){
            TokenType.push_back({ .type = TokenType::LEAVE, .theLine=line});
        } else if(bufa == "let"){
            TokenType.push_back({ .type = TokenType::LET, .theLine=line});
        } else { //.value will hold the idnetifier symbol 
            TokenType.push_back({ .type = TokenType::IDENTIFIER, .value=bufa, .theLine=line });
        }
        */
        auto iter = HashMap.find(bufa);
        if (iter != HashMap.end()){
            TokenType.push_back({ .type = iter->second, .theLine=line});
        } else {
            TokenType.push_back({ .type = TokenType::IDENTIFIER, .value=bufa, .theLine=line });
        }
    }

    size_t idx = 0;
    int line = 1;
    

};