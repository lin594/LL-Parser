#include <map>
#include <set>
#include <string>
#include <vector>

#include "lexer.h"
#define EpsilonSymbol "`"  // "ε"
/**
 * @brief 语法分析器
 *
 */
class Parser {
   private:
    /**
     * @brief variable
     * 非终极符
     */
    std::set<std::string> V;
    /**
     * @brief terminal
     * 终极符
     */
    std::set<std::string> T;
    /**
     * @brief start
     * 开始符号
     */
    std::string S = "";
    /**
     * @brief production
     * 产生式
     */
    std::vector<std::vector<std::string> > P;
    /**
     * @brief catalog
     * 按产生式左侧的非终极符归类产生式（编号）
     */
    std::map<std::string, std::vector<int> > catalog;

    /**
     * @brief FIRST集合
     * FIRST(X)是X可以推导出的开头终结符号集合
     */
    std::map<std::string, std::set<std::string> > FIRST;
    /**
     * @brief FOLLOW集
     * FOLLOW(A)是该文法的所有句型中紧跟在A之后出现的终结符号或＄组成的集合，
     */
    std::map<std::string, std::set<std::string> > FOLLOW;

    /**
     * @brief 预测分析表
     * <非终结符，终结符> 对应的产生式列表
     */
    std::map<std::pair<std::string, std::string>, std::vector<int> > table;

   private:
    /**
     * @brief 初始化
     *
     */
    void init();
    /**
     * @brief 输入文法
     *
     * @param grammarFilename 文法的路径
     */
    void inputGrammar(std::string grammarFilename);
    /**
     * @brief 构建FIRST集合
     *
     */
    void constructFIRST();
    /**
     * @brief 构建FOLLOW集合
     *
     */
    void constructFOLLOW();
    /**
     * @brief 构造预测分析表
     *
     */

    void constructTable();

   public:
    /**
     * @brief 构造一个语法分析器
     *
     * @param grammarFilename 文法的路径
     */
    Parser(std::string grammarFilename);
    /**
     * @brief 打印非终极符
     *
     */
    void printV();
    /**
     * @brief 打印终极符
     *
     */
    void printT();
    /**
     * @brief 打印开始符号
     *
     */
    void printS();
    /**
     * @brief 打印所有产生式
     *
     */
    void printP();
    /**
     * @brief 按非终极符打印产生式
     *
     * @param v 非终极符（为空时表示全部）
     */
    void printCatalog(std::string v = "");
    /**
     * @brief 分析一个句子
     *
     * @param lexer 待分析的句子
     */
    void analyse(Lexer lexer);
};