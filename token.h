/**
 * @file token.h
 * @author YAN Lin (sheeplin@bupt.edu.cn)
 * @brief
 * @date 2021-12-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <string>
#define EndSymbol "$"
class Token {
   private:
    std::string kind;
    std::string content;

   public:
    Token() : kind{EndSymbol}, content{EndSymbol} {};
    Token(std::string kind) : kind{kind}, content{kind} {};
    Token(std::string kind, std::string content)
        : kind{kind}, content{content} {};
    std::string getKind() { return kind; }
    std::string getContent() { return content; }
    bool isValid() { return !(kind == EndSymbol && content == EndSymbol); }
};