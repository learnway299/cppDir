/**
 * @file string_operations.cpp
 * @brief 字符串操作问题 - 题目文件
 *
 * 包含字符串的各种操作和变换问题。
 */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>

/**
 * 题目1: 反转字符串
 *
 * 编写一个函数，其作用是将输入的字符串反转过来。
 * 输入字符串以字符数组 s 的形式给出。
 *
 * 必须原地修改输入数组、使用 O(1) 的额外空间解决这一问题。
 *
 * 示例:
 *   输入: s = ['h','e','l','l','o']
 *   输出: ['o','l','l','e','h']
 *
 * @param s 字符数组
 */
void reverseString(std::vector<char>& s) {
    // TODO: 实现你的解法
}

/**
 * 题目2: 反转字符串中的单词
 *
 * 给你一个字符串 s，请你反转字符串中单词的顺序。
 * 单词是由非空格字符组成的字符串。s 中使用至少一个空格将字符串中的单词分隔开。
 * 返回单词顺序颠倒且单词之间用单个空格连接的结果字符串。
 *
 * 注意：s 可能在前面、后面或两个单词之间包含多余的空格。
 * 返回的结果字符串中，单词之间应当仅用单个空格分隔，且不包含任何额外的空格。
 *
 * 示例:
 *   输入: s = "the sky is blue"
 *   输出: "blue is sky the"
 *
 *   输入: s = "  hello world  "
 *   输出: "world hello"
 *
 *   输入: s = "a good   example"
 *   输出: "example good a"
 *
 * @param s 输入字符串
 * @return 反转后的字符串
 */
std::string reverseWords(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目3: 反转字符串中的单词 III
 *
 * 给定一个字符串 s，你需要反转字符串中每个单词的字符顺序，同时仍保留空格和单词的初始顺序。
 *
 * 示例:
 *   输入: s = "Let's take LeetCode contest"
 *   输出: "s'teL ekat edoCteeL tsetnoc"
 *
 *   输入: s = "God Ding"
 *   输出: "doG gniD"
 *
 * @param s 输入字符串
 * @return 反转后的字符串
 */
std::string reverseWordsIII(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目4: 字符串转换整数 (atoi)
 *
 * 请你来实现一个 myAtoi(string s) 函数，使其能将字符串转换成一个 32 位有符号整数。
 *
 * 函数 myAtoi(string s) 的算法如下：
 * 1. 读入字符串并丢弃无用的前导空格
 * 2. 检查下一个字符是否为正号或负号，读取该字符（如果有）
 * 3. 读入下一个字符，直到到达下一个非数字字符或输入结尾
 * 4. 将前面步骤读入的这些数字转换为整数
 * 5. 如果整数数超过 32 位有符号整数范围，需要截断
 *
 * 示例:
 *   输入: s = "42"
 *   输出: 42
 *
 *   输入: s = "   -42"
 *   输出: -42
 *
 *   输入: s = "4193 with words"
 *   输出: 4193
 *
 *   输入: s = "words and 987"
 *   输出: 0
 *
 * @param s 输入字符串
 * @return 转换后的整数
 */
int myAtoi(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 整数转罗马数字
 *
 * 罗马数字包含以下七种字符: I, V, X, L, C, D, M
 *   I = 1, V = 5, X = 10, L = 50, C = 100, D = 500, M = 1000
 *
 * 特殊规则：
 *   - I 可以放在 V (5) 和 X (10) 的左边，来表示 4 和 9
 *   - X 可以放在 L (50) 和 C (100) 的左边，来表示 40 和 90
 *   - C 可以放在 D (500) 和 M (1000) 的左边，来表示 400 和 900
 *
 * 示例:
 *   输入: num = 3
 *   输出: "III"
 *
 *   输入: num = 58
 *   输出: "LVIII"
 *
 *   输入: num = 1994
 *   输出: "MCMXCIV"
 *
 * @param num 整数 (1 <= num <= 3999)
 * @return 罗马数字字符串
 */
std::string intToRoman(int num) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目6: 罗马数字转整数
 *
 * 给定一个罗马数字，将其转换成整数。
 *
 * 示例:
 *   输入: s = "III"
 *   输出: 3
 *
 *   输入: s = "LVIII"
 *   输出: 58
 *
 *   输入: s = "MCMXCIV"
 *   输出: 1994
 *
 * @param s 罗马数字字符串
 * @return 对应的整数
 */
int romanToInt(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 字符串压缩
 *
 * 利用字符重复出现的次数，编写一种方法，实现基本的字符串压缩功能。
 * 比如，字符串 aabcccccaaa 会变为 a2b1c5a3。
 * 若压缩后的字符串没有变短，则返回原先的字符串。
 *
 * 示例:
 *   输入: "aabcccccaaa"
 *   输出: "a2b1c5a3"
 *
 *   输入: "abbccd"
 *   输出: "abbccd"
 *   解释: 压缩后为 "a1b2c2d1"，比原字符串长，返回原字符串
 *
 * @param s 输入字符串
 * @return 压缩后的字符串
 */
std::string compressString(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目8: 字符串相加
 *
 * 给定两个字符串形式的非负整数 num1 和 num2，计算它们的和并同样以字符串形式返回。
 * 不能使用任何內建的用于处理大整数的库。
 *
 * 示例:
 *   输入: num1 = "11", num2 = "123"
 *   输出: "134"
 *
 *   输入: num1 = "456", num2 = "77"
 *   输出: "533"
 *
 * @param num1 第一个数字字符串
 * @param num2 第二个数字字符串
 * @return 和的字符串表示
 */
std::string addStrings(std::string num1, std::string num2) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目9: 字符串相乘
 *
 * 给定两个以字符串形式表示的非负整数 num1 和 num2，返回它们的乘积。
 * 不能使用任何內建的大整数库。
 *
 * 示例:
 *   输入: num1 = "2", num2 = "3"
 *   输出: "6"
 *
 *   输入: num1 = "123", num2 = "456"
 *   输出: "56088"
 *
 * 思路: 模拟竖式乘法
 *
 * @param num1 第一个数字字符串
 * @param num2 第二个数字字符串
 * @return 乘积的字符串表示
 */
std::string multiply(std::string num1, std::string num2) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目10: 基本计算器 II
 *
 * 给你一个字符串表达式 s，请你实现一个基本计算器来计算并返回它的值。
 * 整数除法仅保留整数部分。
 *
 * s 由整数和算符 (+, -, *, /) 组成，中间由一些空格隔开。
 *
 * 示例:
 *   输入: s = "3+2*2"
 *   输出: 7
 *
 *   输入: s = " 3/2 "
 *   输出: 1
 *
 *   输入: s = " 3+5 / 2 "
 *   输出: 5
 *
 * 思路: 栈处理优先级
 *
 * @param s 表达式字符串
 * @return 计算结果
 */
int calculate(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目11: 比较版本号
 *
 * 给你两个版本号 version1 和 version2，请你比较它们。
 * 版本号由一个或多个修订号组成，各修订号由一个 '.' 连接。
 * 每个修订号由多位数字组成，可能包含前导零。
 *
 * 比较版本号时，请按从左到右的顺序依次比较它们的修订号。
 *
 * 返回规则如下：
 *   - 如果 version1 > version2 返回 1
 *   - 如果 version1 < version2 返回 -1
 *   - 否则返回 0
 *
 * 示例:
 *   输入: version1 = "1.01", version2 = "1.001"
 *   输出: 0
 *
 *   输入: version1 = "1.0", version2 = "1.0.0"
 *   输出: 0
 *
 *   输入: version1 = "0.1", version2 = "1.1"
 *   输出: -1
 *
 * @param version1 版本号1
 * @param version2 版本号2
 * @return 比较结果
 */
int compareVersion(std::string version1, std::string version2) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目12: 有效的括号
 *
 * 给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串 s，判断字符串是否有效。
 *
 * 有效字符串需满足：
 *   1. 左括号必须用相同类型的右括号闭合
 *   2. 左括号必须以正确的顺序闭合
 *
 * 示例:
 *   输入: s = "()"
 *   输出: true
 *
 *   输入: s = "()[]{}"
 *   输出: true
 *
 *   输入: s = "(]"
 *   输出: false
 *
 * @param s 括号字符串
 * @return 是否有效
 */
bool isValid(std::string s) {
    // TODO: 实现你的解法
    return false;
}


// ==================== 测试代码 ====================
#include <cassert>

void testReverseString() {
    std::vector<char> s = {'h', 'e', 'l', 'l', 'o'};
    reverseString(s);
    std::vector<char> expected = {'o', 'l', 'l', 'e', 'h'};
    assert(s == expected);

    std::cout << "reverseString: PASSED\n";
}

void testReverseWords() {
    assert(reverseWords("the sky is blue") == "blue is sky the");
    assert(reverseWords("  hello world  ") == "world hello");
    assert(reverseWords("a good   example") == "example good a");

    std::cout << "reverseWords: PASSED\n";
}

void testReverseWordsIII() {
    assert(reverseWordsIII("Let's take LeetCode contest") == "s'teL ekat edoCteeL tsetnoc");
    assert(reverseWordsIII("God Ding") == "doG gniD");

    std::cout << "reverseWordsIII: PASSED\n";
}

void testMyAtoi() {
    assert(myAtoi("42") == 42);
    assert(myAtoi("   -42") == -42);
    assert(myAtoi("4193 with words") == 4193);
    assert(myAtoi("words and 987") == 0);

    std::cout << "myAtoi: PASSED\n";
}

void testIntToRoman() {
    assert(intToRoman(3) == "III");
    assert(intToRoman(58) == "LVIII");
    assert(intToRoman(1994) == "MCMXCIV");

    std::cout << "intToRoman: PASSED\n";
}

void testRomanToInt() {
    assert(romanToInt("III") == 3);
    assert(romanToInt("LVIII") == 58);
    assert(romanToInt("MCMXCIV") == 1994);

    std::cout << "romanToInt: PASSED\n";
}

void testCompressString() {
    assert(compressString("aabcccccaaa") == "a2b1c5a3");
    assert(compressString("abbccd") == "abbccd");

    std::cout << "compressString: PASSED\n";
}

void testAddStrings() {
    assert(addStrings("11", "123") == "134");
    assert(addStrings("456", "77") == "533");
    assert(addStrings("0", "0") == "0");

    std::cout << "addStrings: PASSED\n";
}

void testMultiply() {
    assert(multiply("2", "3") == "6");
    assert(multiply("123", "456") == "56088");

    std::cout << "multiply: PASSED\n";
}

void testCalculate() {
    assert(calculate("3+2*2") == 7);
    assert(calculate(" 3/2 ") == 1);
    assert(calculate(" 3+5 / 2 ") == 5);

    std::cout << "calculate: PASSED\n";
}

void testCompareVersion() {
    assert(compareVersion("1.01", "1.001") == 0);
    assert(compareVersion("1.0", "1.0.0") == 0);
    assert(compareVersion("0.1", "1.1") == -1);

    std::cout << "compareVersion: PASSED\n";
}

void testIsValid() {
    assert(isValid("()") == true);
    assert(isValid("()[]{}") == true);
    assert(isValid("(]") == false);

    std::cout << "isValid: PASSED\n";
}

int main() {
    std::cout << "=== String Operations Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testReverseString();
    // testReverseWords();
    // testReverseWordsIII();
    // testMyAtoi();
    // testIntToRoman();
    // testRomanToInt();
    // testCompressString();
    // testAddStrings();
    // testMultiply();
    // testCalculate();
    // testCompareVersion();
    // testIsValid();

    std::cout << "All tests passed!\n";
    return 0;
}
