#include "parser.h"
#include <cstdio>
int main(){    
    Parser parser("grammar2.txt");
    Lexer lexer("sentence.txt");
    // Token token=lexer.getNextToken();
    // while(token.isValid()){
    //     // printf("%s %s\n",token.getKind().c_str(),token.getContent().c_str());
    //     token=lexer.getNextToken();
    // }
    parser.analyse(lexer);
    return 0;
}