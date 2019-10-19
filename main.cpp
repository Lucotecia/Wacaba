#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"
#include "main.hpp"

std::string get_file_extention(std::string full_name){
    int ext_idx = full_name.find_last_of(".");
    int ext_len = full_name.size() - ext_idx;
    if(ext_idx == -1){
        return std::string("");
    }
    return full_name.substr(ext_idx, ext_len);
}

int main(int argc, char *argv[]){
    std::ifstream source_file_stream;
    std::string buf;
    std::queue<std::string> *source = nullptr;
    std::queue<Token*> *tokens = nullptr;
    if(argc == 1){
        //display information about this language
        std::cout << "Wacaba" << std::endl;
        std::cout << VERSION << " " << RELEASE_DATE << std::endl;
        return 0;
    }else if(argc == 2){
        //just compile the src
        std::string source_file_name = std::string(argv[1]);
        std::string file_ext = get_file_extention(source_file_name);
        if(file_ext != SOURCE_FILE_EXTENTION){
            std::cerr << "invalid file format" << std::endl;
            exit(1);
        }
        source_file_stream.open(source_file_name, std::ios::in);
        if(!source_file_stream.is_open()){
            std::cerr << "failed to open the source file" << std::endl;
            exit(1);
        }
        source = new std::queue<std::string>();
        while(true){
            std::getline(source_file_stream, buf);//skip LF and waste EOF here
            if(source_file_stream.eof()) break;
            source->push(std::string(buf));
        }
        
        tokens = lexer(source);
        
        parser(tokens);
        
        generator(source_file_name);

        delete source;
        source = nullptr;
    }else{
        std::cerr << "too many options" << std::endl;
        exit(1);
    }
    return 0;
}
