/**
 * @file main.cpp
 * @author YAN Lin (sheeplin@bupt.edu.cn)
 * @brief 程序入口
 * @date 2021-12-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <stdio.h>

#include "parser.h"
int main(int argc, char** argv) {
    if (argc == 3) {
        Parser parser(argv[1]);
        Lexer lexer(argv[2]);
        parser.analyse(lexer);
        return EXIT_SUCCESS;
    } else {
        puts("参数数量不足！");
        puts("需要两个文件作为参数，分别代表文法和句子。");
        return EXIT_FAILURE;
    }
}