/**
 * @file monotonic_stack_solution.cpp
 * @brief 单调栈问题 - 解答文件
 */

#include "monotonic_stack.h"
#include <iostream>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <cassert>

namespace MonotonicStackProblems {

// 题目1: 下一个更大元素 I
std::vector<int> nextGreaterElementSolution(std::vector<int>& nums1, std::vector<int>& nums2) {
    std::unordered_map<int, int> nextGreater;
    std::stack<int> stk;

    for (int num : nums2) {
        while (!stk.empty() && stk.top() < num) {
            nextGreater[stk.top()] = num;
            stk.pop();
        }
        stk.push(num);
    }

    std::vector<int> result;
    for (int num : nums1) {
        result.push_back(nextGreater.count(num) ? nextGreater[num] : -1);
    }

    return result;
}

// 题目2: 下一个更大元素 II（循环数组）
std::vector<int> nextGreaterElementsSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    std::vector<int> result(n, -1);
    std::stack<int> stk;

    for (int i = 0; i < 2 * n; ++i) {
        int idx = i % n;
        while (!stk.empty() && nums[stk.top()] < nums[idx]) {
            result[stk.top()] = nums[idx];
            stk.pop();
        }
        if (i < n) {
            stk.push(idx);
        }
    }

    return result;
}

// 题目3: 每日温度
std::vector<int> dailyTemperaturesSolution(std::vector<int>& temperatures) {
    int n = static_cast<int>(temperatures.size());
    std::vector<int> answer(n, 0);
    std::stack<int> stk;

    for (int i = 0; i < n; ++i) {
        while (!stk.empty() && temperatures[i] > temperatures[stk.top()]) {
            int prevIdx = stk.top();
            stk.pop();
            answer[prevIdx] = i - prevIdx;
        }
        stk.push(i);
    }

    return answer;
}

// 题目4: 柱状图中最大的矩形
int largestRectangleAreaSolution(std::vector<int>& heights) {
    int n = static_cast<int>(heights.size());

    std::vector<int> h(n + 2, 0);
    for (int i = 0; i < n; ++i) {
        h[i + 1] = heights[i];
    }

    std::stack<int> stk;
    int maxArea = 0;

    for (int i = 0; i < n + 2; ++i) {
        while (!stk.empty() && h[stk.top()] > h[i]) {
            int height = h[stk.top()];
            stk.pop();
            int width = i - stk.top() - 1;
            maxArea = std::max(maxArea, height * width);
        }
        stk.push(i);
    }

    return maxArea;
}

// 题目5: 最大矩形
int maximalRectangleSolution(std::vector<std::vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return 0;

    int rows = static_cast<int>(matrix.size());
    int cols = static_cast<int>(matrix[0].size());

    std::vector<int> heights(cols, 0);
    int maxArea = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            heights[j] = matrix[i][j] == '1' ? heights[j] + 1 : 0;
        }
        maxArea = std::max(maxArea, largestRectangleAreaSolution(heights));
    }

    return maxArea;
}

// 题目6: 接雨水
int trapSolution(std::vector<int>& height) {
    std::stack<int> stk;
    int water = 0;

    for (int i = 0; i < static_cast<int>(height.size()); ++i) {
        while (!stk.empty() && height[i] > height[stk.top()]) {
            int bottom = stk.top();
            stk.pop();

            if (stk.empty()) break;

            int left = stk.top();
            int h = std::min(height[left], height[i]) - height[bottom];
            int w = i - left - 1;
            water += h * w;
        }
        stk.push(i);
    }

    return water;
}

// 题目7: 去除重复字母
std::string removeDuplicateLettersSolution(std::string s) {
    std::vector<int> count(26, 0);
    std::vector<bool> inStack(26, false);

    for (char c : s) {
        ++count[c - 'a'];
    }

    std::string result;

    for (char c : s) {
        --count[c - 'a'];

        if (inStack[c - 'a']) continue;

        while (!result.empty() && result.back() > c && count[result.back() - 'a'] > 0) {
            inStack[result.back() - 'a'] = false;
            result.pop_back();
        }

        result.push_back(c);
        inStack[c - 'a'] = true;
    }

    return result;
}

// 题目8: 股票价格跨度（解答版本）
class StockSpannerSolutionImpl {
private:
    std::stack<std::pair<int, int>> stk;

public:
    StockSpannerSolutionImpl() {}

    int next(int price) {
        int span = 1;

        while (!stk.empty() && stk.top().first <= price) {
            span += stk.top().second;
            stk.pop();
        }

        stk.push({price, span});
        return span;
    }
};

StockSpannerSolution::StockSpannerSolution() {}
int StockSpannerSolution::next(int price) { return 0; }

// 题目9: 132 模式
bool find132patternSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    if (n < 3) return false;

    std::stack<int> stk;
    int third = INT_MIN;

    for (int i = n - 1; i >= 0; --i) {
        if (nums[i] < third) {
            return true;
        }

        while (!stk.empty() && nums[i] > stk.top()) {
            third = stk.top();
            stk.pop();
        }

        stk.push(nums[i]);
    }

    return false;
}

// 题目10: 移掉 K 位数字
std::string removeKdigitsSolution(std::string num, int k) {
    std::string result;

    for (char c : num) {
        while (!result.empty() && k > 0 && result.back() > c) {
            result.pop_back();
            --k;
        }
        result.push_back(c);
    }

    while (k > 0) {
        result.pop_back();
        --k;
    }

    int start = 0;
    while (start < static_cast<int>(result.size()) && result[start] == '0') {
        ++start;
    }

    result = result.substr(start);

    return result.empty() ? "0" : result;
}

// ==================== 测试函数 ====================

void testMonotonicStackSolution() {
    std::cout << "=== Monotonic Stack Tests (Solution) ===\n";

    // nextGreaterElement
    std::vector<int> nums1 = {4, 1, 2};
    std::vector<int> nums2 = {1, 3, 4, 2};
    std::vector<int> expected1 = {-1, 3, -1};
    assert(nextGreaterElementSolution(nums1, nums2) == expected1);
    std::cout << "  nextGreaterElement: PASSED\n";

    // nextGreaterElements
    std::vector<int> nums = {1, 2, 1};
    std::vector<int> expected2 = {2, -1, 2};
    assert(nextGreaterElementsSolution(nums) == expected2);
    std::cout << "  nextGreaterElements: PASSED\n";

    // dailyTemperatures
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> expected3 = {1, 1, 4, 2, 1, 1, 0, 0};
    assert(dailyTemperaturesSolution(temps) == expected3);
    std::cout << "  dailyTemperatures: PASSED\n";

    // largestRectangleArea
    std::vector<int> heights = {2, 1, 5, 6, 2, 3};
    assert(largestRectangleAreaSolution(heights) == 10);
    std::cout << "  largestRectangleArea: PASSED\n";

    // maximalRectangle
    std::vector<std::vector<char>> matrix = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    assert(maximalRectangleSolution(matrix) == 6);
    std::cout << "  maximalRectangle: PASSED\n";

    // trap
    std::vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    assert(trapSolution(height) == 6);
    std::cout << "  trap: PASSED\n";

    // removeDuplicateLetters
    assert(removeDuplicateLettersSolution("bcabc") == "abc");
    assert(removeDuplicateLettersSolution("cbacdcbc") == "acdb");
    std::cout << "  removeDuplicateLetters: PASSED\n";

    // StockSpanner
    StockSpannerSolutionImpl stockSpanner;
    assert(stockSpanner.next(100) == 1);
    assert(stockSpanner.next(80) == 1);
    assert(stockSpanner.next(60) == 1);
    assert(stockSpanner.next(70) == 2);
    assert(stockSpanner.next(60) == 1);
    assert(stockSpanner.next(75) == 4);
    assert(stockSpanner.next(85) == 6);
    std::cout << "  StockSpanner: PASSED\n";

    // find132pattern
    std::vector<int> pattern1 = {1, 2, 3, 4};
    assert(find132patternSolution(pattern1) == false);
    std::vector<int> pattern2 = {3, 1, 4, 2};
    assert(find132patternSolution(pattern2) == true);
    std::cout << "  find132pattern: PASSED\n";

    // removeKdigits
    assert(removeKdigitsSolution("1432219", 3) == "1219");
    assert(removeKdigitsSolution("10200", 1) == "200");
    assert(removeKdigitsSolution("10", 2) == "0");
    std::cout << "  removeKdigits: PASSED\n";

    std::cout << "All Monotonic Stack tests passed!\n";
}

} // namespace MonotonicStackProblems
