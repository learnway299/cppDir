/**
 * @file sort_colors_solution.cpp
 * @brief 颜色分类 - 参考答案
 */

#include "sort_colors.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace SortColorsImpl {

namespace Solution {

void sortColors(std::vector<int>& nums) {
    int p0 = 0, curr = 0, p2 = static_cast<int>(nums.size()) - 1;

    while (curr <= p2) {
        if (nums[curr] == 0) {
            std::swap(nums[p0++], nums[curr++]);
        } else if (nums[curr] == 2) {
            std::swap(nums[curr], nums[p2--]);
        } else {
            ++curr;
        }
    }
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Sort Colors Tests ===" << std::endl;

    {
        std::vector<int> nums = {2, 0, 2, 1, 1, 0};
        Solution::sortColors(nums);
        assert((nums == std::vector<int>{0, 0, 1, 1, 2, 2}));
    }
    std::cout << "  Sort Colors (basic): PASSED" << std::endl;

    {
        std::vector<int> nums = {2, 0, 1};
        Solution::sortColors(nums);
        assert((nums == std::vector<int>{0, 1, 2}));
    }
    std::cout << "  Sort Colors (simple): PASSED" << std::endl;

    {
        std::vector<int> nums = {0};
        Solution::sortColors(nums);
        assert(nums[0] == 0);
    }
    std::cout << "  Sort Colors (single): PASSED" << std::endl;

    {
        std::vector<int> nums = {1, 2, 0};
        Solution::sortColors(nums);
        assert((nums == std::vector<int>{0, 1, 2}));
    }
    std::cout << "  Sort Colors (reversed): PASSED" << std::endl;
}

} // namespace SortColorsImpl

/**
 * 关键要点：
 *
 * 1. 荷兰国旗问题（Dutch National Flag Problem）：
 *    - 由 Edsger Dijkstra 提出
 *    - 将数组分为三部分：小于、等于、大于某个值
 *    - 使用三个指针维护三个区域
 *
 * 2. 三指针含义：
 *    - p0: 下一个 0 应该放置的位置
 *    - curr: 当前扫描的位置
 *    - p2: 下一个 2 应该放置的位置
 *    - 不变式：[0, p0) 全是 0，[p0, curr) 全是 1，(p2, n-1] 全是 2
 *
 * 3. 处理逻辑：
 *    - nums[curr] == 0:
 *      * 与 p0 交换
 *      * p0++, curr++（换过来的必然是 1，可以直接 curr++）
 *    - nums[curr] == 1:
 *      * 已经在正确位置，curr++ 继续扫描
 *    - nums[curr] == 2:
 *      * 与 p2 交换
 *      * p2--，但 curr 不动（换过来的元素还未检查）
 *
 * 4. 为什么遇到 0 时 curr 可以移动：
 *    - p0 指向的位置要么是 1（已处理过），要么等于 curr（未处理）
 *    - 如果 p0 < curr，那 p0 位置必然是 1（因为 0 都被移到前面了）
 *    - 交换后 curr 位置变成 1，可以安全移动
 *
 * 5. 为什么遇到 2 时 curr 不能移动：
 *    - p2 指向的位置还未被处理过
 *    - 换过来的可能是 0、1 或 2，需要重新判断
 *
 * 6. 循环条件：curr <= p2
 *    - p2 右边的元素都已经是 2
 *    - curr 超过 p2 说明所有元素都已处理
 *
 * 7. 时间复杂度：O(n)
 *    - 一次遍历，每个元素最多被访问两次
 *
 * 8. 空间复杂度：O(1)
 *    - 原地排序，只使用常数额外空间
 *
 * 9. 推广：三路快排（3-way Quick Sort）
 *    - 相同的思想可以用于优化快速排序
 *    - 当数组中有大量重复元素时效率更高
 */
