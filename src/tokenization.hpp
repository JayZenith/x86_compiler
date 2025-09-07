#pragma once
#include <string>
#include <vector>
#include <optional>

enum class TokenType { IntLit, Ident, Plus, Minus, Star, Slash, Eq, Semi, Let, Exit, Eof };

struct Token {
    TokenType type;
    std::string value;
};

class Tokenizer {
public:
    explicit Tokenizer(const std::string& input) : m_input(input) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        size_t i = 0;
        while (i < m_input.size()) {
            char c = m_input[i];
            if (isspace(c)) { i++; continue; }

            if (isdigit(c)) {
                size_t start = i;
                while (i < m_input.size() && isdigit(m_input[i])) i++;
                tokens.push_back({TokenType::IntLit, m_input.substr(start, i - start)});
                continue;
            }

            if (isalpha(c)) {
                size_t start = i;
                while (i < m_input.size() && isalnum(m_input[i])) i++;
                std::string word = m_input.substr(start, i - start);
                if (word == "let") tokens.push_back({TokenType::Let, word});
                else if (word == "exit") tokens.push_back({TokenType::Exit, word});
                else tokens.push_back({TokenType::Ident, word});
                continue;
            }

            switch (c) {
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


// #pragma once

// #include <string>
// #include <vector>

// enum class TokenType {
//     exit,
//     int_lit,
//     semi,
//     open_paren,
//     close_paren,
//     ident,
//     let,
//     eq,
//     plus,
//     star,
//     minus,
//     fslash,
//     open_curly,
//     close_curly,
//     if_,
//     elif,
//     else_,
//     none,
// };

// // human-readable debug output 
// inline std::string to_string(const TokenType type)
// {
//     switch (type) {
//     case TokenType::exit:
//         return "`exit`";
//     case TokenType::int_lit:
//         return "int literal";
//     case TokenType::semi:
//         return "`;`";
//     case TokenType::open_paren:
//         return "`(`";
//     case TokenType::close_paren:
//         return "`)`";
//     case TokenType::ident:
//         return "identifier";
//     case TokenType::let:
//         return "`let`";
//     case TokenType::eq:
//         return "`=`";
//     case TokenType::plus:
//         return "`+`";
//     case TokenType::star:
//         return "`*`";
//     case TokenType::minus:
//         return "`-`";
//     case TokenType::fslash:
//         return "`/`";
//     case TokenType::open_curly:
//         return "`{`";
//     case TokenType::close_curly:
//         return "`}`";
//     case TokenType::if_:
//         return "`if`";
//     case TokenType::elif:
//         return "`elif`";
//     case TokenType::else_:
//         return "`else`";
//     }
//     assert(false);
// }

// // operator precedence for parsing expressions 
// inline std::optional<int> bin_prec(const TokenType type)
// {
//     switch (type) {
//     case TokenType::minus:
//     case TokenType::plus:
//         return 0;
//     case TokenType::fslash:
//     case TokenType::star:
//         return 1;
//     default:
//         return {};
//     }
// }

// struct Token {
//     TokenType type;
//     int line;
//     std::optional<std::string> value {};
// };

// class Tokenizer {
// public:
//     explicit Tokenizer(std::string src)
//         : m_src(std::move(src))
//     {
//     }

//     std::vector<Token> tokenize()
//     {
//         std::vector<Token> tokens;
//         std::string buf;
//         int line_count = 1;
//         while (peek().has_value()) {
//             if (std::isalpha(peek().value())) {
//                 buf.push_back(consume());
//                 while (peek().has_value() && std::isalnum(peek().value())) {
//                     buf.push_back(consume());
//                 }
//                 if (buf == "exit") {
//                     tokens.push_back({ TokenType::exit, line_count });
//                     buf.clear();
//                 }
//                 else if (buf == "let") {
//                     tokens.push_back({ TokenType::let, line_count });
//                     buf.clear();
//                 }
//                 else if (buf == "if") {
//                     tokens.push_back({ TokenType::if_, line_count });
//                     buf.clear();
//                 }
//                 else if (buf == "elif") {
//                     tokens.push_back({ TokenType::elif, line_count });
//                     buf.clear();
//                 }
//                 else if (buf == "else") {
//                     tokens.push_back({ TokenType::else_, line_count });
//                     buf.clear();
//                 }
//                 else {
//                     tokens.push_back({ TokenType::ident, line_count, buf });
//                     buf.clear();
//                 }
//             }
//             else if (std::isdigit(peek().value())) {
//                 buf.push_back(consume());
//                 while (peek().has_value() && std::isdigit(peek().value())) {
//                     buf.push_back(consume());
//                 }
//                 tokens.push_back({ TokenType::int_lit, line_count, buf });
//                 buf.clear();
//             }
//             else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
//                 consume();
//                 consume();
//                 while (peek().has_value() && peek().value() != '\n') {
//                     consume();
//                 }
//             }
//             else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
//                 consume();
//                 consume();
//                 while (peek().has_value()) {
//                     if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/') {
//                         break;
//                     }
//                     consume();
//                 }
//                 if (peek().has_value()) {
//                     consume();
//                 }
//                 if (peek().has_value()) {
//                     consume();
//                 }
//             }
//             else if (peek().value() == '(') {
//                 consume();
//                 tokens.push_back({ TokenType::open_paren, line_count });
//             }
//             else if (peek().value() == ')') {
//                 consume();
//                 tokens.push_back({ TokenType::close_paren, line_count });
//             }
//             else if (peek().value() == ';') {
//                 consume();
//                 tokens.push_back({ TokenType::semi, line_count });
//             }
//             else if (peek().value() == '=') {
//                 consume();
//                 tokens.push_back({ TokenType::eq, line_count });
//             }
//             else if (peek().value() == '+') {
//                 consume();
//                 tokens.push_back({ TokenType::plus, line_count });
//             }
//             else if (peek().value() == '*') {
//                 consume();
//                 tokens.push_back({ TokenType::star, line_count });
//             }
//             else if (peek().value() == '-') {
//                 consume();
//                 tokens.push_back({ TokenType::minus, line_count });
//             }
//             else if (peek().value() == '/') {
//                 consume();
//                 tokens.push_back({ TokenType::fslash, line_count });
//             }
//             else if (peek().value() == '{') {
//                 consume();
//                 tokens.push_back({ TokenType::open_curly, line_count });
//             }
//             else if (peek().value() == '}') {
//                 consume();
//                 tokens.push_back({ TokenType::close_curly, line_count });
//             }
//             else if (peek().value() == '\n') {
//                 consume();
//                 line_count++;
//             }
//             else if (std::isspace(peek().value())) {
//                 consume();
//             }
//             else {
//                 std::cerr << "Invalid token" << std::endl;
//                 exit(EXIT_FAILURE);
//             }
//         }
//         m_index = 0;
//         return tokens;
//     }

// private:
//     //if caller dosent use return value, issue warning 
//     [[nodiscard]] std::optional<char> peek(const size_t offset = 0) const
//     {
//         if (m_index + offset >= m_src.length()) {
//             return {};
//         }
//         return m_src.at(m_index + offset);
//     }

//     char consume()
//     {
//         return m_src.at(m_index++);
//     }

//     const std::string m_src;
//     size_t m_index = 0;
// };
