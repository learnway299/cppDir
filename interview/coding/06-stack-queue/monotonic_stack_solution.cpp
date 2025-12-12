/**
 * @file monotonic_stack_solution.cpp
 * @brief 单调栈问题 - 解答文件
 *
 * 单调栈模板：
 * - 单调递增栈：栈底到栈顶递增，用于找下一个更大元素
 * - 单调递减栈：栈底到栈顶递减，用于找下一个更小元素
 *
 * 遍历方向选择：
 * - 找右边第一个更大/更小：从左向右遍历
 * - 找左边第一个更大/更小：从右向左遍历
 */

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <unordered_map>
#include <algorithm>

/**
 * 题目1: 下一个更大元素 I
 *
 * 解法: 单调递减栈 + 哈希表
 * 时间复杂度: O(m + n)
 * 空间复杂度: O(n)
 */
std::vector<int> nextGreaterElement(std::vector<int>& nums1, std::vector<int>& nums2) {
    std::unordered_map<int, int> nextGreater;
    std::stack<int> stk;

    // 对 nums2 构建下一个更大元素映射
    for (int num : nums2) {
        while (!stk.empty() && stk.top() < num) {
            nextGreater[stk.top()] = num;
            stk.pop();
        }
        stk.push(num);
    }

    // 查询 nums1
    std::vector<int> result;
    for (int num : nums1) {
        result.push_back(nextGreater.count(num) ? nextGreater[num] : -1);
    }

    return result;
}

/**
 * 题目2: 下一个更大元素 II（循环数组）
 *
 * 解法: 单调递减栈 + 遍历两遍
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<int> nextGreaterElements(std::vector<int>& nums) {
    int n = nums.size();
    std::vector<int> result(n, -1);
    std::stack<int> stk;  // 存储索引

    // 遍历两遍数组模拟循环
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

/**
 * 题目3: 每日温度
 *
 * 解法: 单调递减栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<int> dailyTemperatures(std::vector<int>& temperatures) {
    int n = temperatures.size();
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

/**
 * 题目4: 柱状图中最大的矩形
 *
 * 解法: 单调递增栈
 * 对于每个柱子，找左右第一个比它矮的柱子
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int largestRectangleArea(std::vector<int>& heights) {
    int n = heights.size();

    // 左右两边添加哨兵 0
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

/**
 * 不使用哨兵的版本
 */
int largestRectangleAreaNoSentinel(std::vector<int>& heights) {
    int n = heights.size();
    std::vector<int> left(n), right(n);
    std::stack<int> stk;

    // 找每个柱子左边第一个更小的
    for (int i = 0; i < n; ++i) {
        while (!stk.empty() && heights[stk.top()] >= heights[i]) {
            stk.pop();
        }
        left[i] = stk.empty() ? -1 : stk.top();
        stk.push(i);
    }

    while (!stk.empty()) stk.pop();

    // 找每个柱子右边第一个更小的
    for (int i = n - 1; i >= 0; --i) {
        while (!stk.empty() && heights[stk.top()] >= heights[i]) {
            stk.pop();
        }
        right[i] = stk.empty() ? n : stk.top();
        stk.push(i);
    }

    int maxArea = 0;
    for (int i = 0; i < n; ++i) {
        maxArea = std::max(maxArea, heights[i] * (right[i] - left[i] - 1));
    }

    return maxArea;
}

/**
 * 题目5: 最大矩形
 *
 * 解法: 将每一行作为柱状图底部，复用柱状图最大矩形
 * 时间复杂度: O(rows * cols)
 * 空间复杂度: O(cols)
 */
int maximalRectangle(std::vector<std::vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return 0;

    int rows = matrix.size();
    int cols = matrix[0].size();

    std::vector<int> heights(cols, 0);
    int maxArea = 0;

    for (int i = 0; i < rows; ++i) {
        // 更新高度
        for (int j = 0; j < cols; ++j) {
            heights[j] = matrix[i][j] == '1' ? heights[j] + 1 : 0;
        }

        // 计算当前行的最大矩形
        maxArea = std::max(maxArea, largestRectangleArea(heights));
    }

    return maxArea;
}

/**
 * 题目6: 接雨水
 *
 * 解法1: 单调递减栈（按行计算）
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int trap(std::vector<int>& height) {
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

/**
 * 解法2: 双指针
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int trapTwoPointers(std::vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int leftMax = 0, rightMax = 0;
    int water = 0;

    while (left < right) {
        if (height[left] < height[right]) {
            if (height[left] >= leftMax) {
                leftMax = height[left];
            } else {
                water += leftMax - height[left];
            }
            ++left;
        } else {
            if (height[right] >= rightMax) {
                rightMax = height[right];
            } else {
                water += rightMax - height[right];
            }
            --right;
        }
    }

    return water;
}

/**
 * 题目7: 去除重复字母
 *
 * 解法: 单调递增栈 + 贪心
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)，字符集固定
 */
std::string removeDuplicateLetters(std::string s) {
    std::vector<int> count(26, 0);
    std::vector<bool> inStack(26, false);

    for (char c : s) {
        ++count[c - 'a'];
    }

    std::string result;

    for (char c : s) {
        --count[c - 'a'];

        if (inStack[c - 'a']) continue;

        // 单调递增栈：如果栈顶字符比当前大，且后面还有，则出栈
        while (!result.empty() && result.back() > c && count[result.back() - 'a'] > 0) {
            inStack[result.back() - 'a'] = false;
            result.pop_back();
        }

        result.push_back(c);
        inStack[c - 'a'] = true;
    }

    return result;
}

/**
 * 题目8: 股票价格跨度
 *
 * 解法: 单调递减栈，存储 (价格, 跨度)
 */
class StockSpanner {
private:
    std::stack<std::pair<int, int>> stk;  // (price, span)

public:
    StockSpanner() {}

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

/**
 * 题目9: 132 模式
 *
 * 解法: 单调递减栈，从右向左遍历
 * 维护 nums[k]（132 中的 2）的最大值
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool find132pattern(std::vector<int>& nums) {
    int n = nums.size();
    if (n < 3) return false;

    std::stack<int> stk;
    int third = INT_MIN;  // 132 中的 2（nums[k] 的候选最大值）

    for (int i = n - 1; i >= 0; --i) {
        if (nums[i] < third) {
            return true;  // 找到 nums[i] < third < stk.top()
        }

        while (!stk.empty() && nums[i] > stk.top()) {
            third = stk.top();  // 更新 third 为被弹出的最大值
            stk.pop();
        }

        stk.push(nums[i]);
    }

    return false;
}

/**
 * 题目10: 移掉 K 位数字
 *
 * 解法: 单调递增栈 + 贪心
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string removeKdigits(std::string num, int k) {
    std::string result;

    for (char c : num) {
        while (!result.empty() && k > 0 && result.back() > c) {
            result.pop_back();
            --k;
        }
        result.push_back(c);
    }

    // 如果还需要删除
    while (k > 0) {
        result.pop_back();
        --k;
    }

    // 去除前导零
    int start = 0;
    while (start < static_cast<int>(result.size()) && result[start] == '0') {
        ++start;
    }

    result = result.substr(start);

    return result.empty() ? "0" : result;
}


// ==================== 单调栈模板总结 ====================
/**
 * 模板：找下一个更大元素
 *
 * vector<int> nextGreater(vector<int>& nums) {
 *     int n = nums.size();
 *     vector<int> result(n, -1);
 *     stack<int> stk;  // 单调递减栈（栈底到栈顶递减）
 *
 *     for (int i = 0; i < n; ++i) {
 *         while (!stk.empty() && nums[stk.top()] < nums[i]) {
 *             result[stk.top()] = nums[i];  // 或 result[stk.top()] = i
 *             stk.pop();
 *         }
 *         stk.push(i);
 *     }
 *
 *     return result;
 * }
 *
 * 变体：
 * - 找下一个更小元素：把 < 改成 >
 * - 找上一个更大/更小元素：从右向左遍历
 * - 循环数组：遍历 2n 次，用 i % n 取索引
 */


// ==================== 测试代码 ====================
#include <cassert>

void testNextGreaterElement() {
    std::vector<int> nums1 = {4, 1, 2};
    std::vector<int> nums2 = {1, 3, 4, 2};
    std::vector<int> expected = {-1, 3, -1};
    assert(nextGreaterElement(nums1, nums2) == expected);

    std::cout << "nextGreaterElement: PASSED\n";
}

void testNextGreaterElements() {
    std::vector<int> nums = {1, 2, 1};
    std::vector<int> expected = {2, -1, 2};
    assert(nextGreaterElements(nums) == expected);

    std::cout << "nextGreaterElements: PASSED\n";
}

void testDailyTemperatures() {
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> expected = {1, 1, 4, 2, 1, 1, 0, 0};
    assert(dailyTemperatures(temps) == expected);

    std::cout << "dailyTemperatures: PASSED\n";
}

void testLargestRectangleArea() {
    std::vector<int> heights = {2, 1, 5, 6, 2, 3};
    assert(largestRectangleArea(heights) == 10);
    assert(largestRectangleAreaNoSentinel(heights) == 10);

    std::cout << "largestRectangleArea: PASSED\n";
}

void testMaximalRectangle() {
    std::vector<std::vector<char>> matrix = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    assert(maximalRectangle(matrix) == 6);

    std::cout << "maximalRectangle: PASSED\n";
}

void testTrap() {
    std::vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    assert(trap(height) == 6);
    assert(trapTwoPointers(height) == 6);

    std::cout << "trap: PASSED\n";
}

void testRemoveDuplicateLetters() {
    assert(removeDuplicateLetters("bcabc") == "abc");
    assert(removeDuplicateLetters("cbacdcbc") == "acdb");

    std::cout << "removeDuplicateLetters: PASSED\n";
}

void testStockSpanner() {
    StockSpanner stockSpanner;
    assert(stockSpanner.next(100) == 1);
    assert(stockSpanner.next(80) == 1);
    assert(stockSpanner.next(60) == 1);
    assert(stockSpanner.next(70) == 2);
    assert(stockSpanner.next(60) == 1);
    assert(stockSpanner.next(75) == 4);
    assert(stockSpanner.next(85) == 6);

    std::cout << "StockSpanner: PASSED\n";
}

void testFind132pattern() {
    std::vector<int> nums1 = {1, 2, 3, 4};
    assert(find132pattern(nums1) == false);

    std::vector<int> nums2 = {3, 1, 4, 2};
    assert(find132pattern(nums2) == true);

    std::cout << "find132pattern: PASSED\n";
}

void testRemoveKdigits() {
    assert(removeKdigits("1432219", 3) == "1219");
    assert(removeKdigits("10200", 1) == "200");
    assert(removeKdigits("10", 2) == "0");

    std::cout << "removeKdigits: PASSED\n";
}

int main() {
    std::cout << "=== Monotonic Stack Solution Tests ===\n";

    testNextGreaterElement();
    testNextGreaterElements();
    testDailyTemperatures();
    testLargestRectangleArea();
    testMaximalRectangle();
    testTrap();
    testRemoveDuplicateLetters();
    testStockSpanner();
    testFind132pattern();
    testRemoveKdigits();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
