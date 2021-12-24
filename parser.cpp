#include "parser.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
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
    removeLeftRecursion();
    leftFactor();
    T.insert(EndSymbol);
    T.erase(EpsilonSymbol);
    // 输出若干信息
    printV();
    printT();
    printS();
    printP();
    printCatalog();
    constructFIRST();
    constructFOLLOW();
    constructTable();
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
void Parser::removeLeftRecursion() {
    std::set<std::string> VV = std::set<std::string>{V};
    for (auto v : VV) {
        std::vector<int> pIdRec;  // 左递归的产生式
        std::vector<int> pIdNonRec;  // 没有左递归的产生式(不含空产生式)
        bool hasEpsilon = 0;
        for (auto id : catalog[v])
            if (P[id][1] == v)
                pIdRec.push_back(id);
            else if (P[id][1] == EpsilonSymbol)
                hasEpsilon = id;
            else
                pIdNonRec.push_back(id);
        // 没有左递归就忽略
        if (pIdRec.size() == 0) continue;
        // 构造新的非终结符
        std::string vv = v + "'";
        while (V.count(vv)) vv += "'";
        V.insert(vv);
        // 重新开始构建产生式
        catalog[v].clear();
        // 空产生式
        if (hasEpsilon) catalog[v].push_back(hasEpsilon);
        // beta
        for (auto id : pIdNonRec) {
            catalog[v].push_back(id);
            P[id].push_back(vv);
        }
        // alpha
        for (auto id : pIdRec) {
            catalog[vv].push_back(id);
            P[id].erase(P[id].begin() + 1);
            P[id].push_back(vv);
            P[id][0] = vv;
        }
        catalog[vv].push_back(P.size());
        P.push_back({vv, EpsilonSymbol});
        std::cout << "\033[33m消除左递归：\t\033[0m" << v << std::endl;
    }
}
void Parser::leftFactor() {
    auto VV = std::set<std::string>{V};
    for (auto& v : VV) {
        std::map<std::string, std::vector<int> > factors;
        for (int id : catalog[v]) {
            auto& production = P[id];
            factors[production[1]].push_back(id);
        }
        catalog[v].clear();
        for (auto& i : factors) {
            // 不需要提取公因式
            if (i.second.size() < 2) {
                catalog[v].insert(catalog[v].end(), i.second.begin(),
                                  i.second.end());
                continue;
            }
            // 造出一个新的非终结符
            std::string m = v;
            while (V.count(m)) m += "'";
            V.insert(m);
            std::cout << "\033[33m提取" << v << "关于" << i.first
                      << "的左公因式："
                      << "\t\033[0m" << v << std::endl;
            for (int id : i.second) {
                auto& production = P[id];
                production[0] = m;
                production.erase(production.begin() + 1);
                if (production.size() == 1) production.push_back(EpsilonSymbol);
            }
            catalog[m].insert(catalog[m].end(), i.second.begin(),
                              i.second.end());
            catalog[v].push_back(P.size());
            P.push_back({v, i.first, m});
        }
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
std::string ProductionToString(const std::vector<std::string>& production) {
    std::string s = production[0] + "->";
    for (int i = 1; i < production.size(); i++) s += production[i];
    return s;
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
    return s.count(EpsilonSymbol);
}

std::set<std::string> excepctEpsilon(std::set<std::string> s) {
    s.erase(EpsilonSymbol);
    return s;
}

void Parser::constructFIRST() {
    // epsilon
    FIRST[EpsilonSymbol].insert(EpsilonSymbol);
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
                // 规则1：产生式右侧第一个符号是终极符
                if (T.count(first)) FIRST[v].insert(first);
                // 规则2：产生式右侧第一个符号是epsilon
                else if (first == EpsilonSymbol)
                    FIRST[v].insert(EpsilonSymbol);
                // 规则3：产生式右侧第一个符号是非终极符
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
                    if (i == production.size()) FIRST[v].insert(EpsilonSymbol);
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
void Parser::constructFOLLOW() {
    // 规则1：开始符号的FOLLOW集里至少有$
    FOLLOW[S].insert(EndSymbol);
    int before = -1, after = 0;
    do {
        before = after;
        for (auto& production : P) {
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
                if (!FIRST[production[i]].count(EpsilonSymbol)) break;
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
void Parser::constructTable() {
    table.clear();
    for (int i = 0; i < P.size(); i++) {
        std::string A = P[i][0];
        for (auto& a : FIRST[P[i][1]])
            if (a != EpsilonSymbol) table[{A, a}].push_back(i);
        if (FIRST[P[i][1]].count(EpsilonSymbol))
            for (auto& b : FOLLOW[A]) table[{A, b}].push_back(i);
    }
    bool isLL = true;
    for (auto& i : table)
        if (i.second.size() != 1) {
            isLL = false;
            break;
        }
    if (isLL == false) {
        std::cout << "这不是一个LL(1)文法" << std::endl;
        return;
    }
    std::map<std::string, int> colLength;
    for (auto& t : T) colLength[t] = 5;  //最小宽度
    for (auto& i : table)
        colLength[i.first.second] =
            std::max(colLength[i.first.second],
                     (int)ProductionToString(P[i.second[0]]).length());
    bool hasEpsilon = T.erase(EpsilonSymbol);
    std::cout.setf(std::ios::left);
    std::cout << std::setw(5) << " ";
    for (auto& t : T) std::cout << std::setw(colLength[t] + 2) << t;
    std::cout << std::endl;
    for (auto& v : V) {
        std::cout << std::setw(5) << v;
        for (auto& t : T)
            std::cout << std::setw(colLength[t] + 2)
                      << (table[{v, t}].size()
                              ? ProductionToString(P[table[{v, t}][0]])
                              : "");
        std::cout << std::endl;
    }
    if (hasEpsilon) T.insert(EpsilonSymbol);
}

std::string VectorToString(const std::vector<std::string>& v) {
    std::string s = "";
    for (auto& i : v) s += i;
    return s;
}

const std::string ErrorMessage = "\033[31mERROR!\033[0m";
void Parser::analyse(Lexer lexer) {
    std::vector<std::string> stack{EndSymbol, S};
    std::vector<std::string> sentence{};
    bool hasError = false;
    std::string X;
    Token a = lexer.getNextToken();

    std::vector<std::string> outStack{"stack", VectorToString(stack)};
    std::vector<std::string> outInput{"input", a.getContent()};
    std::vector<std::string> outOutput{"output", ""};
    std::vector<std::string> outSentence{"sentence", VectorToString(sentence)};

    do {
        X = stack.back();
        // X是终结符或结尾
        if (T.count(X)) {
            if (X == a.getKind()) {
                if (X == EndSymbol) break;
                stack.pop_back();
                outOutput.push_back("match " + a.getKind());
                a = lexer.getNextToken();
                sentence.push_back(X);
            } else {
                // error
                outOutput.push_back(ErrorMessage);
                if (a.isValid() == false) break;
                hasError = true;
            }

        } else if (table[{X, a.getKind()}].size()) {
            stack.pop_back();
            auto& production = P[table[{X, a.getKind()}][0]];
            if (production[1] != EpsilonSymbol) {
                for (int i = production.size() - 1; i >= 1; i--)
                    stack.push_back(production[i]);
            }
            outOutput.push_back(ProductionToString(production));
        } else {
            // error
            outOutput.push_back(ErrorMessage);
            if (a.isValid() == false) break;
            stack.pop_back();
            hasError = true;
        }
        outStack.push_back(VectorToString(stack));
        outInput.push_back(a.getContent());
        outSentence.push_back(VectorToString(sentence));
    } while (X != EndSymbol);

    if (hasError == false && a.isValid() == false)
        std::cout << "分析成功！" << std::endl;
    else
        std::cout << "分析失败！" << std::endl;

    int colLength[4]{0};
    for (auto& s : outStack)
        colLength[0] = std::max(colLength[0], (int)s.length());
    for (auto& s : outInput)
        colLength[1] = std::max(colLength[1], (int)s.length());
    for (auto& s : outOutput)
        colLength[2] = std::max(colLength[2], (int)s.length());
    for (auto& s : outSentence)
        colLength[3] = std::max(colLength[3], (int)s.length());
    for (int i = 0; i < 4; i++) colLength[i] += 3;
    for (int i = 0; i < outStack.size(); i++)
        std::cout << std::setw(colLength[0]) << outStack[i]
                  << std::setw(colLength[1]) << outInput[i]
                  << std::setw(colLength[2]) << outOutput[i]
                  << std::setw(colLength[3]) << outSentence[i] << std::endl;
}
