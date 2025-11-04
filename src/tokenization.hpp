#pragma once
#include <string>
#include <vector>
#include <optional>

enum class TokenType { IntLit, Ident, Plus, Minus, Star, Slash, Eq, Semi, Let, Exit, Eof };
struct Token { TokenType type; std::string value; };

class Tokenizer {
public: 
    //explicit prevents implicit conversions when calling constructor
    explicit Tokenizer(const std::string& input) : m_input(input) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        size_t i = 0;
        while (i < m_input.size()) {
            char c = m_input[i];
            if (isspace(c)) { i++; continue; }

            if (isdigit(c)) {  //Capture digit, not alphanumericals
                size_t start = i;
                while (i < m_input.size() && isdigit(m_input[i])) 
                    i++;
                //struct Token { TokenType type; std::string value; };
                tokens.push_back({TokenType::IntLit, m_input.substr(start, i - start)});
                continue;
            }

            if (isalpha(c)) { //EIther "let" "exit" or identifier 
                size_t start = i;
                while (i < m_input.size() && isalnum(m_input[i]))
                    i++;
                std::string word = m_input.substr(start, i - start);
                if (word == "let") tokens.push_back({TokenType::Let, word});
                else if (word == "exit") tokens.push_back({TokenType::Exit, word});
                else tokens.push_back({TokenType::Ident, word});
                continue;
            }

            switch (c) { //Not covered by functions above
                case '+': tokens.push_back({TokenType::Plus, "+"}); break;
                case '-': tokens.push_back({TokenType::Minus, "-"}); break;
                case '*': tokens.push_back({TokenType::Star, "*"}); break;
                case '/': tokens.push_back({TokenType::Slash, "/"}); break;
                case '=': tokens.push_back({TokenType::Eq, "="}); break;
                case ';': tokens.push_back({TokenType::Semi, ";"}); break;
                default: break;
            }
            i++;
        }
        tokens.push_back({TokenType::Eof, ""});
        return tokens;
    }

private:
    std::string m_input;
};

