/**
 * @file lexer.cpp
 * @author YAN Lin (sheeplin@bupt.edu.cn)
 * @brief
 * @date 2021-12-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "lexer.h"

#define debug 1
#ifdef debug
#include <stdio.h>
#endif
char Lexer::getNextChar() {
    static bool changeLine = false;
    char c = fgetc(input);
    if (changeLine) {
        changeLine = false;
        line++;
        col = 0;
    }
    col++;
    if (c == '\n') changeLine = true;
    return c;
}

/**
 * @brief 判定是否为一个数字字符
 *
 * @param c 需要判定的字符
 * @return true 字符'0'到'9'
 * @return false 其他字符
 */
bool isDigit(char c) { return c >= '0' && c <= '9'; }

/**
 * @brief 判定是否为一个变量可能用到的字符
 *
 * @param c 需要判定的字符
 * @return true 下划线、小写字母、大写字母、数字字符
 * @return false 其他字符
 */
bool isVar(char c) {
    return c == '_'                 // 下划线
           || c >= 'a' && c <= 'z'  // 小写字母
           || c >= 'A' && c <= 'Z'  // 大写字母
           || c >= '0' && c <= '9'  // 数字字符
        ;
}
static char c;
Lexer::Lexer(const char* filename) {
    input = fopen(filename, "r");
    c = getNextChar();
}
Lexer::Lexer() {
    input = stdin;
    c = getNextChar();
}

Token Lexer::getNextToken() {
    if (c == EOF) {
        fclose(input);
        return Token();
    }
    // 如果遇到数字字符 进入数字常量的判定
    else if (isDigit(c)) {
        std::string temp = "";
        for (; isDigit(c); c = getNextChar()) temp += c;
#ifdef debug
        printf("(%3d,%3d): num %s\n", line, col, temp.c_str());
#endif
        return Token{"num", temp};
    }
    // 如果遇到下划线、英文字母 进入变量或保留字的判定
    else if (isVar(c)) {
        std::string temp = "";
        for (; isVar(c); c = getNextChar()) temp += c;

#ifdef debug
        printf("(%3d,%3d): id %s\n", line, col, temp.c_str());
#endif
        return Token{"id", temp};
    }
    // 如果遇到空白字符 重新读一个重新来过
    else if (c == ' ' || c == '\n' || c == '\t') {
        c = getNextChar();
        return getNextToken();
    }
    // 如果遇到符号字符 进入各类符号的判定
    else {
#ifdef debug
        printf("(%3d,%3d): %c\n", line, col, c);
#endif
        std::string temp(1, c);
        c = getNextChar();
        return Token(temp);
    }
}
