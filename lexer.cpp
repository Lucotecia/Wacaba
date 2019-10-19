#include "lexer.hpp"
#include <iostream>

std::queue<Token*>* lexer(std::queue<std::string> *source){
    std::queue<Token*> *ret = new std::queue<Token*>();
    while(!source->empty()){
        std::cout << source->front() << std::endl;
        source->pop();
    }
    return ret;
}
