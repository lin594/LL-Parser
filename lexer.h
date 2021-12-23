#include <vector>

#include "token.h"
class Lexer {
   private:
    /**
     * @brief 输入文件指针
     *
     */
    FILE* input = nullptr;
    /**
     * @brief 当前行号
     *
     */
    int line = 1;
    /**
     * @brief 当前列号
     *
     */
    int col = 0;
    /**
     * @brief 当前Lexer已经被读取到
     *
     */
    int index = 0;
    /**
     * @brief 从输入文件中获取下一个字符
     * 同时更新line和col信息
     * @return char 下一个读入字符
     */
    char getNextChar();

   public:
    /**
     * @brief Lexer的构造函数
     * 从指定文件获取数据
     * @param filename 输入文件地址
     */
    Lexer(const char* filename);
    /**
     * @brief Lexer的构造函数
     * 从标准输入获取数据
     */
    Lexer();
    /**
     * @brief 获取下一个Token
     *
     * @return Token 获取到的Token
     */
    Token getNextToken();
};