#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>

void parser(std::queue<Token*> *tokens){
    Token *cur;
    std::string tmp;
    while(!tokens->empty()){
        cur = tokens->front();
        std::cout << cur->str << std::endl;
        switch(cur->kind){
        case 0:
            tmp = "reserved";
            break;
        case 1:
            tmp = "identifier";
            break;
        case 2:
            tmp = "integer:";
            break;
        case 3:
            tmp = "float:";
            break;
        case 4:
            tmp = "character";
            break;
        case 5:
            tmp = "string";
            break;
        }
        std::cout << tmp;
        if(cur->kind==TK_INTEGER){
            std::cout << cur->val.i << std::endl;
        }else if(cur->kind==TK_FLOAT){
            std::cout << cur->val.f << std::endl;
        }else{
            std::cout << std::endl;
        }
        std::cout << "===============================" << std::endl;
        tokens->pop();
    }
    return;
}
