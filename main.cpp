#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"
#include "main.hpp"
std::vector<std::string> *source;
int main(int argc, char *argv[]){
    std::ifstream source_file;
    std::string buf;
    source = new std::vector<std::string>();
    if(argc == 1){
        //display information about this language
        std::cout << "Wacaba" << std::endl;
        std::cout << VERSION << " " << RELEASE_DATE << std::endl;
    }else if(argc == 2){
        //just compile the src
        source_file.open(argv[1], std::ios::in);
        if(!source_file.is_open()){
            std::cerr << "failed to open the source file" << std::endl;
            exit(1);
        }
        while(true){
            std::getline(source_file, buf);//skip LF and waste EOF here
            if(source_file.eof()) break;
            source->push_back(std::string(buf));
        }
        lexer();
        parser();
        generator(std::string(argv[1]));
    }
    return 0;
}