#ifndef INCLUDE_LEXER_HPP
#define INCLUDE_LEXER_HPP
#include <queue>
#include <string>


enum TokenKind {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF
};

union Value {
    double d;
    float f;
    int64_t i;
    std::string str;
};

struct Token;
struct Token {
    TokenKind kind;
    std::string name;
    Value val;
    std::string str;
};

std::queue<Token*>* lexer(std::queue<std::string>*);


#endif
