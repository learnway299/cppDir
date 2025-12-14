/**
 * @file move_zeroes_solution.cpp
 * @brief 移动零 - 参考答案
 */

#include "move_zeroes.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace MoveZeroesImpl {

namespace Solution {

void moveZeroes(std::vector<int>& nums) {
    int slow = 0;
    for (size_t fast = 0; fast < nums.size(); ++fast) {
        if (nums[fast] != 0) {
            std::swap(nums[slow++], nums[fast]);
        }
    }
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Move Zeroes Tests ===" << std::endl;

    {
        std::vector<int> nums = {0, 1, 0, 3, 12};
        Solution::moveZeroes(nums);
        assert((nums == std::vector<int>{1, 3, 12, 0, 0}));
    }
    std::cout << "  Move Zeroes (basic): PASSED" << std::endl;

    {
        std::vector<int> nums = {0};
        Solution::moveZeroes(nums);
        assert(nums[0] == 0);
    }
    std::cout << "  Move Zeroes (single zero): PASSED" << std::endl;

    {
        std::vector<int> nums = {1, 2, 3};
        Solution::moveZeroes(nums);
        assert((nums == std::vector<int>{1, 2, 3}));
    }
    std::cout << "  Move Zeroes (no zeros): PASSED" << std::endl;

    {
        std::vector<int> nums = {0, 0, 1};
        Solution::moveZeroes(nums);
        assert((nums == std::vector<int>{1, 0, 0}));
    }
    std::cout << "  Move Zeroes (leading zeros): PASSED" << std::endl;
}

} // namespace MoveZeroesImpl

/**
 * 关键要点：
 *
 * 1. 快慢指针原理：
 *    - slow: 下一个非零元素应该放置的位置
 *    - fast: 扫描数组找非零元素
 *    - [0, slow) 是已处理的非零元素
 *    - [fast, n) 是待处理区域
 *
 * 2. 交换操作：
 *    - 遇到非零元素时，与 slow 位置交换
 *    - 交换后 slow++ 移动到下一个位置
 *    - 这样保证了非零元素的相对顺序
 *
 * 3. 为什么用交换而不是赋值：
 *    - 赋值需要两次遍历：
 *      * 第一次把非零元素移到前面
 *      * 第二次把后面填充为 0
 *    - 交换只需一次遍历，更高效
 *
 * 4. 优化：避免自己和自己交换
 *    可以改为：
 *    if (nums[fast] != 0) {
 *        if (slow != fast) {
 *            std::swap(nums[slow], nums[fast]);
 *        }
 *        slow++;
 *    }
 *
 * 5. 时间复杂度：O(n)
 *    - fast 遍历整个数组一次
 *
 * 6. 空间复杂度：O(1)
 *    - 原地修改，只使用常数额外空间
 *
 * 7. 推广：移动任意值到末尾
 *    - 只需修改判断条件 nums[fast] != target
 */
