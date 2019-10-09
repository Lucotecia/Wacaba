#include "lexer.hpp"
#include <iostream>

void lexer(){
    std::cout << source->back();
    source->pop_back();
    std::cout << source->back();
    source->pop_back();
    std::cout << source->back() << std::endl;
    source->pop_back();
    return;
}
