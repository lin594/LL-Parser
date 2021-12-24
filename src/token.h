/**
 * @file token.h
 * @author YAN Lin (sheeplin@bupt.edu.cn)
 * @brief 符号Token头文件
 * @date 2021-12-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <string>
#define EndSymbol "$"
/**
 * @brief 识别出的符号
 *
 */
class Token {
   private:
    /**
     * @brief 种别
     *
     */
    std::string kind;
    /**
     * @brief 内容
     * 如果不是id或者num，则内容与种别相同
     */
    std::string content;

   public:
    /**
     * @brief 无参构造函数（产生终止Token）
     *
     */
    Token() : kind{EndSymbol}, content{EndSymbol} {};
    /**
     * @brief 构造函数（普通Token）
     *
     * @param kind 种别
     */
    Token(std::string kind) : kind{kind}, content{kind} {};
    /**
     * @brief 构造函数（变量）
     *
     * @param kind 种别
     * @param content 内容
     */
    Token(std::string kind, std::string content)
        : kind{kind}, content{content} {};
    /**
     * @brief 获取种别
     *
     * @return std::string 种别
     */
    std::string getKind() { return kind; }
    /**
     * @brief 获取内容
     *
     * @return std::string 内容
     */
    std::string getContent() { return content; }
    /**
     * @brief 是否有效（非终止Token）
     *
     * @return true 普通Token
     * @return false 终止Token
     */
    bool isValid() { return !(kind == EndSymbol && content == EndSymbol); }
};