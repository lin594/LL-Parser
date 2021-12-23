#include "parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

/**
 * @brief 打印一个集合到标准输出
 *
 * @param set 待打印的集合
 */
void printSet(const std::set<std::string>& set);

/**
 * @brief 打印产生式
 *
 * @param production 产生式向量
 */
void printProduction(const std::vector<std::string>& production);

Parser::Parser(std::string grammarFilename) {
    init();
    inputGrammar(grammarFilename);
    // 输出若干信息
    printV();
    printT();
    printS();
    printP();
    printCatalog();
    findFIRST();
    findFOLLOW();
}

void Parser::init() {
    V.clear();
    T.clear();
    S = "";
    P.clear();
    catalog.clear();
    FIRST.clear();
    FOLLOW.clear();
}

void Parser::inputGrammar(std::string grammarFilename) {
    std::fstream input(grammarFilename);  // 输入文件流
    std::string productions;              // 一行中的多个产生式
    std::stringstream ss;                 // 通过stringstream方便处理
    std::string v;           // 行首（产生式左侧）非终极符
    std::string p;           // 产生式（右侧）
    std::string token = "";  // 识别标号
    int index = 0;           // 产生式的下标
    while (std::getline(input, productions)) {
        productions += " | ";
        // 通过stringstream方便处理
        ss.clear();
        ss << productions;
        // 每一行第一个元素都是一个非终极符
        ss >> v;
        V.insert(v);
        // 默认第一个非终极符是文法的开始符号
        if (!S.length()) S = v;
        // 读入无用的 ->
        ss >> p;
        // 开始处理每一个产生式p
        while (ss >> p) {
            if (p == "|") {
                // 如果只有一个非大写字母，则为终极符
                if (token.length() == 1 &&
                    !(token[0] >= 'A' && token[0] <= 'Z'))
                    T.insert(token);
                // 其他情况均为终极符
                else
                    V.insert(token);
                P[index].push_back(token);
                token = "";
                index++;
                continue;
            }
            if (P.size() == index) {
                catalog[v].push_back(index);
                P.push_back(std::vector<std::string>{v});
            }
            // 对于产生式里的每一个字符c
            for (char c : p) {
                // 第一个字符必然是token的开始
                if (!token.length()) {
                    token += c;
                    continue;
                }
                if (token[0] >= 'A' && token[0] <= 'Z') {
                    // 可能是一个非终极符
                    // 大写字母只能续上'
                    if (c == '\'')
                        token += c;
                    else {
                        // 识别出这个非终极符
                        V.insert(token);
                        P[index].push_back(token);
                        // 继续向下识别
                        token = c;
                    }
                } else if (token[0] >= 'a' && token[0] <= 'z') {
                    // 小写字母可以继续续上小写字母
                    if (c >= 'a' && c <= 'z')
                        token += c;
                    else {
                        if (token.length() == 1) {
                            // 如果只有一个小写字母，则为终极符
                            T.insert(token);
                            P[index].push_back(token);
                            token = c;
                        } else {
                            // 如果有多个小写字母，则为非终极符
                            V.insert(token);
                            P[index].push_back(token);
                            token = c;
                        }
                    }
                } else {
                    // 其他字符都是终极符
                    T.insert(token);
                    P[index].push_back(token);
                    token = c;
                }
            }
        }
    }
    // 特殊判定
    if (V.count("id")) {
        V.erase("id");
        T.insert("id");
    }
    if (V.count("num")) {
        V.erase("num");
        T.insert("num");
    }
}

void printSet(const std::set<std::string>& set) {
    std::vector<std::string> output;
    for (auto s : set) output.push_back(s);
    if (!output.size()) return;
    std::cout << "{" << output[0];
    for (int i = 1; i < output.size(); i++) std::cout << "," << output[i];
    std::cout << "}";
}
void printProduction(const std::vector<std::string>& production) {
    std::cout << production[0] << " -> ";
    for (int i = 1; i < production.size(); i++)
        std::cout << production[i] << " ";
}

void Parser::printV() {
    std::cout << "非终极符符号V\t";
    printSet(V);
    std::cout << std::endl;
}
void Parser::printT() {
    std::cout << "终极符符号T\t";
    printSet(T);
    std::cout << std::endl;
}
void Parser::printS() { std::cout << "开始符号S\t" << S << std::endl; }
void Parser::printP() {
    std::cout << "产生式集合P:\n";
    for (const auto& production : P) {
        printProduction(production);
        std::cout << std::endl;
    }
}

void Parser::printCatalog(std::string v) {
    if (v.length() == 0) {
        for (auto v : V) printCatalog(v);
        return;
    }
    std::cout << "非终极符" << v << "的产生式：\n";
    for (auto index : catalog[v]) {
        std::cout << index << "\t";
        printProduction(P[index]);
        std::cout << std::endl;
    }
    if (catalog[v].size() == 0) std::cout << "（没有相关产生式）" << std::endl;
}

bool containsEpsilon(const std::set<std::string>& s) {
    return s.count(epsilon);
}
std::set<std::string> excepctEpsilon(std::set<std::string> s) {
    s.erase(epsilon);
    return s;
}
void Parser::findFIRST() {
    // 终极符t的FIRST就是{t}
    for (auto t : T) {
        FIRST[t].insert(t);
    }
    bool hasChange;
    do {
        hasChange = false;
        // 对于非终极符v来说
        for (auto v : V)
            // 枚举每个产生式
            for (auto id : catalog[v]) {
                int before = FIRST[v].size();
                //  当前产生式production
                const std::vector<std::string>& production{P[id]};
                // 产生式右侧第一个符号为firWst
                const std::string& first{production[1]};
                // 产生式右侧第一个符号是终极符
                if (T.count(first)) FIRST[v].insert(first);
                // 产生式右侧第一个符号是epsilon
                else if (first == epsilon)
                    FIRST[v].insert(epsilon);
                // 产生式右侧第一个符号是非终极符
                else {
                    auto&& set = excepctEpsilon(FIRST[first]);
                    FIRST[v].insert(set.begin(), set.end());
                    int i = 1;
                    for (; i < production.size() && V.count(production[i]) &&
                           containsEpsilon(FIRST[production[i]]);
                         i++) {
                        auto&& set = excepctEpsilon(FIRST[production[i]]);
                        FIRST[v].insert(set.begin(), set.end());
                    }
                    if (i == production.size()) FIRST[v].insert(epsilon);
                }
                hasChange |= FIRST[v].size() != before;
            }
    } while (hasChange);
    for (auto v : V) {
        std::cout << "FIRST[" << v << "]: ";
        printSet(FIRST[v]);
        std::cout << std::endl;
    }
}
void Parser::findFOLLOW() {
    // 规则1：开始符号的FOLLOW集里至少有$
    FOLLOW[S].insert("$");
    int before = -1, after = 0;
    do {
        before = after;
        for (auto production : P) {
            std::string v = production[0];
            // 规则2：产生式里非终极符的FOLLOW要包括下一个字段的非空FIRST集
            for (int i = 1; i < production.size() - 1; i++)
                if (V.count(production[i])) {
                    auto&& set = excepctEpsilon(FIRST[production[i + 1]]);
                    FOLLOW[production[i]].insert(set.begin(), set.end());
                }
            // 规则3：非终极符B后面可能为空，那就要加入FOLLOW(A)
            // 为了实现方便应该倒着写
            for (int i = production.size() - 1; i >= 1; i--) {
                if (!V.count(production[i])) break;
                FOLLOW[production[i]].insert(FOLLOW[v].begin(),
                                             FOLLOW[v].end());
                if (!FIRST[production[i]].count(epsilon)) break;
            }
        }
        after = 0;
        for (auto v : V) after += v.size();
    } while (before != after);
    for (auto v : V) {
        std::cout << "FOLLOW[" << v << "]: ";
        printSet(FOLLOW[v]);
        std::cout << std::endl;
    }
}