#ifndef INCLUDE_PARSER_HPP
#define INCLUDE_PARSER_HPP
struct Node {
    Node *parent, *left, *right;
};

void parser(std::queue<Token*>*);
#endif
