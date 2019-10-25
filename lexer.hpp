#ifndef INCLUDE_LEXER_HPP
#define INCLUDE_LEXER_HPP
#include <queue>
#include <string>
#include <cstdint>

enum TokenKind {
    TK_RESERVED,
    TK_IDENT,
    TK_INTEGER,
    TK_FLOAT,
    TK_CHARACTER,
    TK_STRING,
    TK_TYPE
};

union Value {
    double f;
    std::uint64_t i;
};


class Token {
public:
    TokenKind kind;
    std::string str;
    Value val;
};

std::queue<Token*>* lexer(std::queue<std::string>*);


#endif
