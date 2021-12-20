#include "parser.h"
int main(){
    Parser parser("grammar.txt");
    parser.printV();
    parser.printT();
    parser.printS();
    parser.printP();
    parser.printCatalog();
    return 0;
}