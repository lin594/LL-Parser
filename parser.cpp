#include "parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

Parser::Parser(std::string grammarFilename) {
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
}
/**
 * @brief 打印一个集合到标准输出
 *
 * @param set 待打印的集合
 */
void printSet(const std::set<std::string>& set) {
    std::vector<std::string> output;
    for (auto s : set) output.push_back(s);
    if (!output.size()) return;
    std::cout << "{" << output[0];
    for (int i = 1; i < output.size(); i++) std::cout << "," << output[i];
    std::cout << "}";
}
/**
 * @brief 打印产生式
 *
 * @param production 产生式向量
 */
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