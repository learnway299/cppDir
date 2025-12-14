/**
 * @file container_water_solution.cpp
 * @brief 盛最多水的容器 - 参考答案
 */

#include "container_water.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace ContainerWaterImpl {

namespace Solution {

int maxArea(std::vector<int>& height) {
    int left = 0, right = static_cast<int>(height.size()) - 1;
    int maxWater = 0;

    while (left < right) {
        int water = std::min(height[left], height[right]) * (right - left);
        maxWater = std::max(maxWater, water);

        // 移动较矮的一边
        if (height[left] < height[right]) {
            ++left;
        } else {
            --right;
        }
    }
    return maxWater;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Container Water Tests ===" << std::endl;

    {
        std::vector<int> height = {1, 8, 6, 2, 5, 4, 8, 3, 7};
        assert(Solution::maxArea(height) == 49);  // min(8,7) * 7 = 49
    }
    std::cout << "  Max Area (basic): PASSED" << std::endl;

    {
        std::vector<int> height = {1, 1};
        assert(Solution::maxArea(height) == 1);
    }
    std::cout << "  Max Area (simple): PASSED" << std::endl;

    {
        std::vector<int> height = {4, 3, 2, 1, 4};
        assert(Solution::maxArea(height) == 16);  // min(4,4) * 4 = 16
    }
    std::cout << "  Max Area (symmetric): PASSED" << std::endl;
}

} // namespace ContainerWaterImpl

/**
 * 关键要点：
 *
 * 1. 双指针思路：
 *    - 从两端向中间移动
 *    - 每次移动较矮的一边
 *    - 保证不会错过最大面积
 *
 * 2. 为什么移动较矮的边：
 *    - 面积 = min(height[left], height[right]) * width
 *    - 移动较高的边：
 *      * 高度不变或变小（因为取 min）
 *      * 宽度变小（指针相向移动）
 *      * 面积必然不会变大
 *    - 移动较矮的边：
 *      * 高度可能变大（换成更高的边）
 *      * 宽度变小
 *      * 有机会找到更大面积
 *
 * 3. 正确性证明：
 *    - 假设最优解是 (i, j)，i < j
 *    - 不妨设 height[i] <= height[j]
 *    - 从 (0, n-1) 开始，最终会遍历到 (i, j)
 *    - 因为我们总是移动较矮的边，所以不会跳过最优解
 *
 * 4. 时间复杂度：O(n)
 *    - 每次循环至少移动一个指针
 *    - 最多移动 n 次
 *
 * 5. 空间复杂度：O(1)
 *    - 只使用常数额外空间
 *
 * 6. 与接雨水的区别：
 *    - 盛水容器：找两条线围成的最大矩形面积
 *    - 接雨水：计算所有凹槽的水量总和
 */
