#include "parser.h"
int main() {
    Parser parser("grammar3.txt");
    Lexer lexer("sentence3.txt");
    // Token token=lexer.getNextToken();
    // while(token.isValid()){
    //     // printf("%s
    //     %s\n",token.getKind().c_str(),token.getContent().c_str());
    //     token=lexer.getNextToken();
    // }
    parser.analyse(lexer);
    return 0;
}