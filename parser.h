#include <map>
#include <set>
#include <string>
#include <vector>
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
};