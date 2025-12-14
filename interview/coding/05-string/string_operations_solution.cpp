/**
 * @file string_operations_solution.cpp
 * @brief 字符串操作问题 - 参考答案
 */

#include "string_operations.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <stack>
#include <unordered_map>
#include <climits>

namespace StringOperations {

// ==================== 参考答案实现 ====================

// 题目1: 反转字符串
void reverseStringSolution(std::vector<char>& s) {
    int left = 0, right = static_cast<int>(s.size()) - 1;
    while (left < right) {
        std::swap(s[left], s[right]);
        ++left;
        --right;
    }
}

// 题目2: 反转字符串中的单词
std::string reverseWordsSolution(const std::string& s) {
    std::string result;
    int n = static_cast<int>(s.size());
    int end = n - 1;

    while (end >= 0) {
        while (end >= 0 && s[end] == ' ') {
            --end;
        }
        if (end < 0) break;

        int start = end;
        while (start >= 0 && s[start] != ' ') {
            --start;
        }

        if (!result.empty()) {
            result += ' ';
        }
        result += s.substr(start + 1, end - start);

        end = start;
    }

    return result;
}

// 题目3: 反转字符串中的单词 III
std::string reverseWordsIIISolution(std::string s) {
    int start = 0;
    for (int i = 0; i <= static_cast<int>(s.size()); ++i) {
        if (i == static_cast<int>(s.size()) || s[i] == ' ') {
            std::reverse(s.begin() + start, s.begin() + i);
            start = i + 1;
        }
    }
    return s;
}

// 题目4: 字符串转换整数 (atoi)
int myAtoiSolution(const std::string& s) {
    int n = static_cast<int>(s.size());
    int i = 0;

    while (i < n && s[i] == ' ') {
        ++i;
    }

    if (i == n) return 0;

    int sign = 1;
    if (s[i] == '-') {
        sign = -1;
        ++i;
    } else if (s[i] == '+') {
        ++i;
    }

    long long result = 0;
    while (i < n && std::isdigit(s[i])) {
        result = result * 10 + (s[i] - '0');

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

// 题目5: 整数转罗马数字
std::string intToRomanSolution(int num) {
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

// 题目6: 罗马数字转整数
int romanToIntSolution(const std::string& s) {
    std::unordered_map<char, int> values = {
        {'I', 1}, {'V', 5}, {'X', 10}, {'L', 50},
        {'C', 100}, {'D', 500}, {'M', 1000}
    };

    int result = 0;
    int prev = 0;

    for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i) {
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

// 题目7: 字符串压缩
std::string compressStringSolution(const std::string& s) {
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

// 题目8: 字符串相加
std::string addStringsSolution(const std::string& num1, const std::string& num2) {
    std::string result;
    int i = static_cast<int>(num1.size()) - 1;
    int j = static_cast<int>(num2.size()) - 1;
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

// 题目9: 字符串相乘
std::string multiplySolution(const std::string& num1, const std::string& num2) {
    if (num1 == "0" || num2 == "0") return "0";

    int m = static_cast<int>(num1.size()), n = static_cast<int>(num2.size());
    std::vector<int> result(m + n, 0);

    for (int i = m - 1; i >= 0; --i) {
        for (int j = n - 1; j >= 0; --j) {
            int mul = (num1[i] - '0') * (num2[j] - '0');
            int p1 = i + j, p2 = i + j + 1;

            int sum = mul + result[p2];
            result[p2] = sum % 10;
            result[p1] += sum / 10;
        }
    }

    std::string str;
    for (int digit : result) {
        if (!(str.empty() && digit == 0)) {
            str += ('0' + digit);
        }
    }

    return str.empty() ? "0" : str;
}

// 题目10: 基本计算器 II
int calculateSolution(const std::string& s) {
    std::stack<int> nums;
    int num = 0;
    char op = '+';

    for (int i = 0; i <= static_cast<int>(s.size()); ++i) {
        char c = (i < static_cast<int>(s.size())) ? s[i] : '+';

        if (std::isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c != ' ') {
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

    int result = 0;
    while (!nums.empty()) {
        result += nums.top();
        nums.pop();
    }

    return result;
}

// 题目11: 比较版本号
int compareVersionSolution(const std::string& version1, const std::string& version2) {
    int i = 0, j = 0;
    int m = static_cast<int>(version1.size()), n = static_cast<int>(version2.size());

    while (i < m || j < n) {
        int num1 = 0, num2 = 0;

        while (i < m && version1[i] != '.') {
            num1 = num1 * 10 + (version1[i] - '0');
            ++i;
        }
        ++i;

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

// 题目12: 有效的括号
bool isValidSolution(const std::string& s) {
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

// ==================== 测试函数 ====================

void testStringOperationsSolution() {
    std::cout << "=== String Operations Tests (Solution) ===\n";

    // reverseString
    std::vector<char> s = {'h', 'e', 'l', 'l', 'o'};
    reverseStringSolution(s);
    std::vector<char> expected = {'o', 'l', 'l', 'e', 'h'};
    assert(s == expected);
    std::cout << "  reverseStringSolution: PASSED\n";

    // reverseWords
    assert(reverseWordsSolution("the sky is blue") == "blue is sky the");
    assert(reverseWordsSolution("  hello world  ") == "world hello");
    assert(reverseWordsSolution("a good   example") == "example good a");
    std::cout << "  reverseWordsSolution: PASSED\n";

    // reverseWordsIII
    assert(reverseWordsIIISolution("Let's take LeetCode contest") == "s'teL ekat edoCteeL tsetnoc");
    assert(reverseWordsIIISolution("God Ding") == "doG gniD");
    std::cout << "  reverseWordsIIISolution: PASSED\n";

    // myAtoi
    assert(myAtoiSolution("42") == 42);
    assert(myAtoiSolution("   -42") == -42);
    assert(myAtoiSolution("4193 with words") == 4193);
    assert(myAtoiSolution("words and 987") == 0);
    std::cout << "  myAtoiSolution: PASSED\n";

    // intToRoman
    assert(intToRomanSolution(3) == "III");
    assert(intToRomanSolution(58) == "LVIII");
    assert(intToRomanSolution(1994) == "MCMXCIV");
    std::cout << "  intToRomanSolution: PASSED\n";

    // romanToInt
    assert(romanToIntSolution("III") == 3);
    assert(romanToIntSolution("LVIII") == 58);
    assert(romanToIntSolution("MCMXCIV") == 1994);
    std::cout << "  romanToIntSolution: PASSED\n";

    // compressString
    assert(compressStringSolution("aabcccccaaa") == "a2b1c5a3");
    assert(compressStringSolution("abbccd") == "abbccd");
    std::cout << "  compressStringSolution: PASSED\n";

    // addStrings
    assert(addStringsSolution("11", "123") == "134");
    assert(addStringsSolution("456", "77") == "533");
    assert(addStringsSolution("0", "0") == "0");
    std::cout << "  addStringsSolution: PASSED\n";

    // multiply
    assert(multiplySolution("2", "3") == "6");
    assert(multiplySolution("123", "456") == "56088");
    assert(multiplySolution("0", "456") == "0");
    std::cout << "  multiplySolution: PASSED\n";

    // calculate
    assert(calculateSolution("3+2*2") == 7);
    assert(calculateSolution(" 3/2 ") == 1);
    assert(calculateSolution(" 3+5 / 2 ") == 5);
    std::cout << "  calculateSolution: PASSED\n";

    // compareVersion
    assert(compareVersionSolution("1.01", "1.001") == 0);
    assert(compareVersionSolution("1.0", "1.0.0") == 0);
    assert(compareVersionSolution("0.1", "1.1") == -1);
    std::cout << "  compareVersionSolution: PASSED\n";

    // isValid
    assert(isValidSolution("()") == true);
    assert(isValidSolution("()[]{}") == true);
    assert(isValidSolution("(]") == false);
    assert(isValidSolution("{[]}") == true);
    std::cout << "  isValidSolution: PASSED\n";
}

} // namespace StringOperations
