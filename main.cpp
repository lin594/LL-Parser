/**
 * @file main.cpp
 * @author YAN Lin (sheeplin@bupt.edu.cn)
 * @brief
 * @date 2021-12-24
 *
 * @copyright Copyright (c) 2021
 *
 */
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