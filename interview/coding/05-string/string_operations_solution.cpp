/**
 * @file string_operations_solution.cpp
 * @brief 字符串操作问题 - 解答文件
 */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <climits>
#include <sstream>

/**
 * 题目1: 反转字符串
 *
 * 解法: 双指针
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
void reverseString(std::vector<char>& s) {
    int left = 0, right = s.size() - 1;
    while (left < right) {
        std::swap(s[left], s[right]);
        ++left;
        --right;
    }
}

/**
 * 题目2: 反转字符串中的单词
 *
 * 解法: 双指针从后向前扫描
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string reverseWords(std::string s) {
    std::string result;
    int n = s.size();
    int end = n - 1;

    while (end >= 0) {
        // 跳过尾部空格
        while (end >= 0 && s[end] == ' ') {
            --end;
        }
        if (end < 0) break;

        // 找单词开头
        int start = end;
        while (start >= 0 && s[start] != ' ') {
            --start;
        }

        // 添加单词
        if (!result.empty()) {
            result += ' ';
        }
        result += s.substr(start + 1, end - start);

        end = start;
    }

    return result;
}

/**
 * 原地反转解法（先整体反转，再逐个单词反转）
 */
std::string reverseWordsInPlace(std::string s) {
    // 去除多余空格
    int slow = 0;
    for (int fast = 0; fast < static_cast<int>(s.size()); ++fast) {
        if (s[fast] != ' ') {
            if (slow != 0) s[slow++] = ' ';
            while (fast < static_cast<int>(s.size()) && s[fast] != ' ') {
                s[slow++] = s[fast++];
            }
        }
    }
    s.resize(slow);

    // 整体反转
    std::reverse(s.begin(), s.end());

    // 逐个单词反转
    int start = 0;
    for (int i = 0; i <= static_cast<int>(s.size()); ++i) {
        if (i == static_cast<int>(s.size()) || s[i] == ' ') {
            std::reverse(s.begin() + start, s.begin() + i);
            start = i + 1;
        }
    }

    return s;
}

/**
 * 题目3: 反转字符串中的单词 III
 *
 * 解法: 原地反转每个单词
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
std::string reverseWordsIII(std::string s) {
    int start = 0;
    for (int i = 0; i <= static_cast<int>(s.size()); ++i) {
        if (i == static_cast<int>(s.size()) || s[i] == ' ') {
            std::reverse(s.begin() + start, s.begin() + i);
            start = i + 1;
        }
    }
    return s;
}

/**
 * 题目4: 字符串转换整数 (atoi)
 *
 * 解法: 模拟
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int myAtoi(std::string s) {
    int n = s.size();
    int i = 0;

    // 1. 跳过前导空格
    while (i < n && s[i] == ' ') {
        ++i;
    }

    if (i == n) return 0;

    // 2. 处理符号
    int sign = 1;
    if (s[i] == '-') {
        sign = -1;
        ++i;
    } else if (s[i] == '+') {
        ++i;
    }

    // 3. 转换数字
    long long result = 0;
    while (i < n && std::isdigit(s[i])) {
        result = result * 10 + (s[i] - '0');

        // 4. 处理溢出
        if (sign == 1 && result > INT_MAX) {
            return INT_MAX;
        }
        if (sign == -1 && -result < INT_MIN) {
            return INT_MIN;
        }

        ++i;
    }

    return static_cast<int>(sign * result);
}

/**
 * 题目5: 整数转罗马数字
 *
 * 解法: 贪心
 * 时间复杂度: O(1)
 * 空间复杂度: O(1)
 */
std::string intToRoman(int num) {
    std::vector<std::pair<int, std::string>> mapping = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
        {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"},
        {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"}, {1, "I"}
    };

    std::string result;
    for (const auto& [value, symbol] : mapping) {
        while (num >= value) {
            result += symbol;
            num -= value;
        }
    }

    return result;
}

/**
 * 题目6: 罗马数字转整数
 *
 * 解法: 从右向左扫描
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int romanToInt(std::string s) {
    std::unordered_map<char, int> values = {
        {'I', 1}, {'V', 5}, {'X', 10}, {'L', 50},
        {'C', 100}, {'D', 500}, {'M', 1000}
    };

    int result = 0;
    int prev = 0;

    for (int i = s.size() - 1; i >= 0; --i) {
        int curr = values[s[i]];
        if (curr < prev) {
            result -= curr;
        } else {
            result += curr;
        }
        prev = curr;
    }

    return result;
}

/**
 * 题目7: 字符串压缩
 *
 * 解法: 双指针统计
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string compressString(std::string s) {
    if (s.empty()) return s;

    std::string compressed;
    int count = 1;

    for (int i = 1; i <= static_cast<int>(s.size()); ++i) {
        if (i < static_cast<int>(s.size()) && s[i] == s[i - 1]) {
            ++count;
        } else {
            compressed += s[i - 1];
            compressed += std::to_string(count);
            count = 1;
        }
    }

    return compressed.size() < s.size() ? compressed : s;
}

/**
 * 题目8: 字符串相加
 *
 * 解法: 模拟加法
 * 时间复杂度: O(max(m, n))
 * 空间复杂度: O(max(m, n))
 */
std::string addStrings(std::string num1, std::string num2) {
    std::string result;
    int i = num1.size() - 1;
    int j = num2.size() - 1;
    int carry = 0;

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) {
            sum += num1[i] - '0';
            --i;
        }
        if (j >= 0) {
            sum += num2[j] - '0';
            --j;
        }

        result += ('0' + sum % 10);
        carry = sum / 10;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

/**
 * 题目9: 字符串相乘
 *
 * 解法: 模拟竖式乘法
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m + n)
 */
std::string multiply(std::string num1, std::string num2) {
    if (num1 == "0" || num2 == "0") return "0";

    int m = num1.size(), n = num2.size();
    std::vector<int> result(m + n, 0);

    // 从低位到高位相乘
    for (int i = m - 1; i >= 0; --i) {
        for (int j = n - 1; j >= 0; --j) {
            int mul = (num1[i] - '0') * (num2[j] - '0');
            int p1 = i + j, p2 = i + j + 1;

            int sum = mul + result[p2];
            result[p2] = sum % 10;
            result[p1] += sum / 10;
        }
    }

    // 转换为字符串，跳过前导零
    std::string str;
    for (int digit : result) {
        if (!(str.empty() && digit == 0)) {
            str += ('0' + digit);
        }
    }

    return str.empty() ? "0" : str;
}

/**
 * 题目10: 基本计算器 II
 *
 * 解法: 栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int calculate(std::string s) {
    std::stack<int> nums;
    int num = 0;
    char op = '+';

    for (int i = 0; i <= static_cast<int>(s.size()); ++i) {
        char c = (i < static_cast<int>(s.size())) ? s[i] : '+';

        if (std::isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c != ' ') {
            // 根据前一个运算符处理当前数字
            if (op == '+') {
                nums.push(num);
            } else if (op == '-') {
                nums.push(-num);
            } else if (op == '*') {
                int top = nums.top();
                nums.pop();
                nums.push(top * num);
            } else if (op == '/') {
                int top = nums.top();
                nums.pop();
                nums.push(top / num);
            }

            op = c;
            num = 0;
        }
    }

    // 求和
    int result = 0;
    while (!nums.empty()) {
        result += nums.top();
        nums.pop();
    }

    return result;
}

/**
 * 不用栈的解法
 * 空间复杂度: O(1)
 */
int calculateOptimized(std::string s) {
    int result = 0;
    int lastNum = 0;
    int num = 0;
    char op = '+';

    for (int i = 0; i <= static_cast<int>(s.size()); ++i) {
        char c = (i < static_cast<int>(s.size())) ? s[i] : '+';

        if (std::isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c != ' ') {
            if (op == '+') {
                result += lastNum;
                lastNum = num;
            } else if (op == '-') {
                result += lastNum;
                lastNum = -num;
            } else if (op == '*') {
                lastNum *= num;
            } else if (op == '/') {
                lastNum /= num;
            }

            op = c;
            num = 0;
        }
    }

    return result + lastNum;
}

/**
 * 题目11: 比较版本号
 *
 * 解法: 双指针
 * 时间复杂度: O(max(m, n))
 * 空间复杂度: O(1)
 */
int compareVersion(std::string version1, std::string version2) {
    int i = 0, j = 0;
    int m = version1.size(), n = version2.size();

    while (i < m || j < n) {
        int num1 = 0, num2 = 0;

        // 解析 version1 的当前修订号
        while (i < m && version1[i] != '.') {
            num1 = num1 * 10 + (version1[i] - '0');
            ++i;
        }
        ++i;  // 跳过 '.'

        // 解析 version2 的当前修订号
        while (j < n && version2[j] != '.') {
            num2 = num2 * 10 + (version2[j] - '0');
            ++j;
        }
        ++j;

        if (num1 > num2) return 1;
        if (num1 < num2) return -1;
    }

    return 0;
}

/**
 * 题目12: 有效的括号
 *
 * 解法: 栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool isValid(std::string s) {
    std::stack<char> stk;
    std::unordered_map<char, char> pairs = {
        {')', '('}, {']', '['}, {'}', '{'}
    };

    for (char c : s) {
        if (pairs.count(c)) {
            if (stk.empty() || stk.top() != pairs[c]) {
                return false;
            }
            stk.pop();
        } else {
            stk.push(c);
        }
    }

    return stk.empty();
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

    assert(reverseWordsInPlace("the sky is blue") == "blue is sky the");

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
    assert(myAtoi("-91283472332") == INT_MIN);

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
    assert(multiply("0", "456") == "0");

    std::cout << "multiply: PASSED\n";
}

void testCalculate() {
    assert(calculate("3+2*2") == 7);
    assert(calculate(" 3/2 ") == 1);
    assert(calculate(" 3+5 / 2 ") == 5);

    assert(calculateOptimized("3+2*2") == 7);

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
    assert(isValid("([)]") == false);
    assert(isValid("{[]}") == true);

    std::cout << "isValid: PASSED\n";
}

int main() {
    std::cout << "=== String Operations Solution Tests ===\n";

    testReverseString();
    testReverseWords();
    testReverseWordsIII();
    testMyAtoi();
    testIntToRoman();
    testRomanToInt();
    testCompressString();
    testAddStrings();
    testMultiply();
    testCalculate();
    testCompareVersion();
    testIsValid();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
