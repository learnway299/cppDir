/**
 * @file generate_parentheses_solution.cpp
 * @brief 生成括号 - 解答
 */
#include <vector>
#include <string>
#include <iostream>
#include <stack>
using namespace std;

class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> result;
        string path;
        backtrack(result, path, 0, 0, n);
        return result;
    }

private:
    void backtrack(vector<string>& result, string& path, int open, int close, int n) {
        if (path.size() == 2 * n) {
            result.push_back(path);
            return;
        }
        if (open < n) {
            path.push_back('(');
            backtrack(result, path, open + 1, close, n);
            path.pop_back();
        }
        if (close < open) {
            path.push_back(')');
            backtrack(result, path, open, close + 1, n);
            path.pop_back();
        }
    }
};

// 判断括号是否有效
bool isValid(string s) {
    stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else {
            if (st.empty()) return false;
            char top = st.top();
            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) return false;
            st.pop();
        }
    }
    return st.empty();
}

int main() {
    cout << "Generate Parentheses (n=3):\n";
    auto result = Solution().generateParenthesis(3);
    for (auto& s : result) cout << s << "\n";

    cout << "\nValidate:\n";
    cout << "()(): " << isValid("()()") << "\n";
    cout << "(()): " << isValid("(())") << "\n";
    cout << "(]: " << isValid("(]") << "\n";

    return 0;
}
