#include "lexer.hpp"
#include <cctype>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <iostream>
#include <cstdlib>

bool is_alnum(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')|| (c == '_');
}

Token *new_token(TokenKind kind, char *str, int len){
    Token *ret = new Token;
    ret->kind = kind;
    ret->str = std::string(str, len);
    return ret;
}

bool isescseq(char *c){//複数文字のエスケープシーケンスにも後で対応
    return *c=='\''||*c=='\"'||*c=='\\'||*c=='0'||*c=='a'||*c=='b'||*c=='f'||*c=='n'||*c=='r'||*c=='t'||*c=='v';
}

std::queue<Token*>* lexer(std::queue<std::string> *source){
    std::queue<Token*> *ret = new std::queue<Token*>();
    char *line, *p, *q, *end;
    size_t rest, length;
    bool dotflag;
    double floattmp;
    long long integertmp;
    int n, i;
    while(!source->empty()){
        rest = source->front().size();
        line = new char[rest+1];
        p = line;
        std::char_traits<char>::copy(line, source->front().c_str(), rest + 1);
        source->pop();
        while(*p){
            //skip spaces
            if(isspace(*p)){
                p++;
                rest--;
                continue;
            }
            
            //文字列リテラル
            if(*p=='\"'){
                p++;//skip double quotation
                q = p;
                while(true){
                    if(*p==0){
                        std::cerr << "input ended in a string literal" << std::endl;
                        std::exit(1);
                    }
                    if(*p=='\\'){
                        p++;
                        if(!isescseq(p)){
                            std::cerr << "invalid escape sequence" << std::endl;
                            std::exit(1);
                        }
                        p++;
                        continue;
                    }
                    if(*p=='\"'){
                        p++;
                        break;
                    }
                    p++;
                }
                length = p - q - 1;
                ret->push(new_token(TK_STRING, q, length));
                rest -= (length+2);
                continue;
            }

            //文字リテラル(utf-8)
            if(*p=='\''){
                p++;//skip double quotation
                q = p;
                if(*p=='\\'){
                    p++;//バックスラッシュをスキップ
                    if(!isescseq(p)){
                        std::cerr << "an invalid escape sequence" << std::endl;//バックスラッシュの後の文字を解釈できない
                        std::exit(1);        
                    }
                    p++;//エスケープシーケンス(1文字)をスキップ
                    if(*p!='\''){
                        std::cerr << "an invalid character literal" << std::endl;//終端クォートが見つからない
                        std::exit(1);
                    }
                }else{
                    if((*p & 0x80)==0){//バックスラッシュ以外のascii(1バイト文字)
                        n=0;//残り0バイト
                    }else if((*p & 0xe2)==0xc2){//2バイト文字
                        n=1;//残り1バイト
                    }else if((*p & 0xf0)==0xe0){//3バイト文字
                        n=2;//残り2バイト
                    }else if((*p & 0xf8)==0xf0){//4バイト文字
                        n=3;//残り3バイト
                    }else{
                        std::cerr << "an invalid character literal" << std::endl;//UTF-8の1バイト目として不適なビット列
                        std::exit(1);
                    }
                    p++;//1バイト目をスキップ
                    for(i=0;i<n;i++){
                        if((*p & 0xc0)!=0x80){//複数バイト文字の2バイト目以降
                            std::cerr << "an invalid character literal" << std::endl;//UTF-8の2バイト目以降として不適なビット列
                        }
                        p++;
                    }
                    if(*p!='\''){
                        std::cerr << "an invalid character literal" << std::endl;//終端クォートが見つからない
                        std::exit(1);
                    }
                    p++;
                }
                length = n+1;
                ret->push(new_token(TK_CHARACTER, q, length));
                rest -= (length+2);
                continue;
            }
            
            //10進整数リテラル・浮動小数点数リテラル
            if(std::isdigit(*p)){
                dotflag = false;
                q = p;
                p++;
                while(*p){
                    if(*p=='.'){
                        if(std::isdigit(*(p+1))){
                            if(dotflag){
                                std::cerr << "multiple dots in a numerical literal" << std::endl;
                                std::exit(1);
                            }else{
                                dotflag = true;
                                p++;
                            }
                        }else{
                            p--;
                            break;
                        }
                    }else if(std::isdigit(*p)){
                        p++;
                    }else{
                        break;
                    }
                }
                length = p - q;
                errno = 0;
                if(dotflag){
                    floattmp = std::strtod(q, &end);
                    if(errno == ERANGE || floattmp == HUGE_VAL){
                        std::cerr << "a numerical literal is out of the range" << std::endl;
                        std::exit(1);
                    }
                    ret->push(new_token(TK_FLOAT, q, length));
                    ret->back()->val.f = floattmp;
                }else{
                    integertmp = std::strtoll(q, &end, 10);
                    if(errno == ERANGE){
                        std::cerr << "a numerical literal is out of the range" << std::endl;
                        std::exit(1);
                    }
                    ret->push(new_token(TK_INTEGER, q, length));
                    ret->back()->val.i = integertmp;
                }
                rest -= length;
                continue;
            }
            
            //8文字の予約語
            if(rest>=8 && !is_alnum(p[8]) &&
                !strncmp(p, "continue", 6)){
                ret->push(new_token(TK_RESERVED, p, 8));
                p += 8;
                rest -= 8;
                continue;
            }
            
            //7文字の予約語
            if(rest>=7 && !is_alnum(p[7]) && !strncmp(p, "default", 7)){
                ret->push(new_token(TK_RESERVED, p, 7));
                p += 7;
                rest -= 7;
                continue;
            }
            
            //6文字の予約語
            if(rest>=6 && !is_alnum(p[6]) &&
                (!strncmp(p, "return", 6) ||
                !strncmp(p, "string", 6) ||
                !strncmp(p, "import", 6) ||
                !strncmp(p, "switch", 6))){
                ret->push(new_token(TK_RESERVED, p, 6));
                p += 6;
                rest -= 6;
                continue;
            }
            
            //5文字の予約語
            if(rest>=5 && !is_alnum(p[5]) &&
                (!strncmp(p, "while", 5) ||
                !strncmp(p, "float", 5) ||
                !strncmp(p, "class", 5) ||
                !strncmp(p, "alias", 5) ||
                !strncmp(p, "break", 5))){
                ret->push(new_token(TK_RESERVED, p, 5));
                p += 5;
                rest -= 5;
                continue;
            }
                                    
            //4文字の予約語
            if(rest>=4 && !is_alnum(p[4]) &&
                (!strncmp(p, "else", 4) ||
                !strncmp(p, "exit", 4) ||
                !strncmp(p, "char", 4) ||
                !strncmp(p, "bool", 4) ||
                !strncmp(p, "enum", 4) ||
                !strncmp(p, "case", 4) ||
                !strncmp(p, "main", 4))){
                ret->push(new_token(TK_RESERVED, p, 4));
                p += 4;
                rest -= 4;
                continue;
            }
            
            //3文字の予約語
            if(rest>=3 && (!is_alnum(p[3]) &&
                (!strncmp(p, "i16", 3) ||
                !strncmp(p, "i32", 3) ||
                !strncmp(p, "i64", 3) ||
                !strncmp(p, "u16", 3) ||
                !strncmp(p, "u32", 3) ||
                !strncmp(p, "u64", 3) ||
                !strncmp(p, "f32", 3) ||
                !strncmp(p, "f64", 3) ||
                !strncmp(p, "int", 3)) ||
                !strncmp(p, "===", 3) ||
                !strncmp(p, "new", 3))){
                ret->push(new_token(TK_RESERVED, p, 3));
                p += 3;
                rest -= 3;
                continue;
            }
            
            //2文字の予約語
            if(rest>=2 && (!is_alnum(p[2]) &&
                (!strncmp(p, "i8", 2) ||
                !strncmp(p, "u8", 2) ||
                !strncmp(p, "if", 2))) ||
                (!strncmp(p, "++", 2) ||
                !strncmp(p, "--", 2) ||
                !strncmp(p, "+=", 2) ||
                !strncmp(p, "-=", 2) ||
                !strncmp(p, "==", 2) ||
                !strncmp(p, "::", 2) ||
                !strncmp(p, ">=", 2) ||
                !strncmp(p, "<=", 2) ||
                !strncmp(p, "!=", 2) ||
                !strncmp(p, "->", 2))){
                ret->push(new_token(TK_RESERVED, p, 2));
                p += 2;
                rest -= 2;
                continue;
            }
            //1文字の予約語
            if(*p=='+' || *p=='-' || *p=='*' || *p=='/' || 
                *p=='%' || *p=='&' || *p=='|' || *p=='=' || 
                *p=='!' || *p=='~' || *p=='^' || *p=='<' ||
                *p=='>' || *p=='[' || *p==']' || *p=='{' ||
                *p=='}' || *p=='(' || *p==')' || *p=='.' ||
                *p==':' || *p==';' || *p=='#' || *p=='?' ||
                *p=='$' || *p==','){
                ret->push(new_token(TK_RESERVED, p, 1));
                p++;
                rest--;
                continue;
            }
            //indentifier
            if(is_alnum(*p)){
                length = 1;
                while(*(p+length) && is_alnum(*(p+length))){
                    length++;
                }
                ret->push(new_token(TK_IDENT, p, length));
                p += length;
                rest -= length;
                continue;
            }

            std::cerr << "failed to tokenize" << std::endl;
            std::exit(1);
        }
        delete[] line;
    }
    return ret;
}
