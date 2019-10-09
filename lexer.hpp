#include <fstream>
#include <vector>
#include <string>

extern std::vector<std::string> *source;

void lexer();

class Token {
    std::string name;
};

extern std::vector<Token> *tokens;

