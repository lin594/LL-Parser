#include "parser.h"
#include "lexer.h"
int main(){
    Lexer lexer("sentence.txt");
    Parser parser("grammar2.txt");
    return 0;
}