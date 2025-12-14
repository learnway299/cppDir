/**
 * @file trapping_rain_solution.cpp
 * @brief 接雨水 - 参考答案
 */

#include "trapping_rain.h"
#include <algorithm>
#include <stack>
#include <iostream>
#include <cassert>

namespace TrappingRainImpl {

namespace Solution {

// ==================== 方法1: 双指针法 ====================
int trap(std::vector<int>& height) {
    if (height.empty()) return 0;

    int left = 0, right = static_cast<int>(height.size()) - 1;
    int leftMax = 0, rightMax = 0;
    int water = 0;

    while (left < right) {
        if (height[left] < height[right]) {
            // 处理左边
            if (height[left] >= leftMax) {
                leftMax = height[left];
            } else {
                water += leftMax - height[left];
            }
            ++left;
        } else {
            // 处理右边
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

// ==================== 方法2: 单调栈法 ====================
int trapStack(std::vector<int>& height) {
    std::stack<int> st;
    int water = 0;

    for (size_t i = 0; i < height.size(); ++i) {
        while (!st.empty() && height[i] > height[st.top()]) {
            int top = st.top();
            st.pop();
            if (st.empty()) break;

            int width = static_cast<int>(i) - st.top() - 1;
            int h = std::min(height[i], height[st.top()]) - height[top];
            water += width * h;
        }
        st.push(static_cast<int>(i));
    }
    return water;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Trapping Rain Tests ===" << std::endl;

    // 双指针法
    {
        std::vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
        assert(Solution::trap(height) == 6);
    }
    std::cout << "  Trap (two pointers): PASSED" << std::endl;

    // 单调栈法
    {
        std::vector<int> height = {4, 2, 0, 3, 2, 5};
        assert(Solution::trapStack(height) == 9);
    }
    std::cout << "  Trap (monotonic stack): PASSED" << std::endl;

    // 验证两种方法结果一致
    {
        std::vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
        assert(Solution::trap(height) == Solution::trapStack(height));
    }
    std::cout << "  Both methods consistent: PASSED" << std::endl;

    {
        std::vector<int> height = {4, 2, 3};
        assert(Solution::trap(height) == 1);
    }
    std::cout << "  Trap (simple case): PASSED" << std::endl;
}

} // namespace TrappingRainImpl

/**
 * 关键要点：
 *
 * 1. 双指针法（推荐）：
 *    - leftMax, rightMax 记录左右最大高度
 *    - 移动较小高度的指针
 *    - 当前位置能接的水 = max - height[i]
 *    - 核心：只需要知道某一侧的最大值即可
 *
 * 2. 为什么双指针法有效：
 *    - 假设 height[left] < height[right]
 *    - 那么 left 位置能接的水只取决于 leftMax
 *    - 因为 rightMax 至少为 height[right]，必然 >= leftMax
 *    - 所以 water[left] = leftMax - height[left]
 *
 * 3. 单调栈法：
 *    - 维护递减栈（存储索引）
 *    - 遇到更高的柱子时，计算凹槽
 *    - 按层计算（横向），而非按列计算（垂直）
 *
 * 4. 两种方法的区别：
 *    - 双指针：按列计算，从两端向中间
 *    - 单调栈：按层计算，从左到右
 *
 * 5. 时间复杂度：O(n)
 *    - 双指针：每个元素访问一次
 *    - 单调栈：每个元素入栈出栈各一次
 *
 * 6. 空间复杂度：
 *    - 双指针：O(1)
 *    - 单调栈：O(n)
 *
 * 7. 与盛水容器的区别：
 *    - 盛水容器：找两条线围成的最大矩形
 *    - 接雨水：计算所有凹槽的总水量
 */
