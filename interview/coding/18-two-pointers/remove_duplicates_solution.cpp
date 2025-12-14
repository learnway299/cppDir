/**
 * @file remove_duplicates_solution.cpp
 * @brief 删除有序数组中的重复项 - 参考答案
 */

#include "remove_duplicates.h"
#include <iostream>
#include <cassert>

namespace RemoveDuplicatesImpl {

namespace Solution {

// ==================== 题目1: 删除重复项（每个元素最多出现一次）====================
int removeDuplicates(std::vector<int>& nums) {
    if (nums.empty()) return 0;

    int slow = 0;
    for (size_t fast = 1; fast < nums.size(); ++fast) {
        if (nums[fast] != nums[slow]) {
            nums[++slow] = nums[fast];
        }
    }
    return slow + 1;
}

// ==================== 题目2: 删除重复项（每个元素最多出现两次）====================
int removeDuplicates2(std::vector<int>& nums) {
    if (nums.size() <= 2) return static_cast<int>(nums.size());

    int slow = 2;
    for (size_t fast = 2; fast < nums.size(); ++fast) {
        if (nums[fast] != nums[slow - 2]) {
            nums[slow++] = nums[fast];
        }
    }
    return slow;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Remove Duplicates Tests ===" << std::endl;

    // 测试删除重复项 I
    {
        std::vector<int> nums = {1, 1, 2};
        int len = Solution::removeDuplicates(nums);
        assert(len == 2);
        assert(nums[0] == 1 && nums[1] == 2);
    }
    std::cout << "  Remove Duplicates I (basic): PASSED" << std::endl;

    {
        std::vector<int> nums = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
        int len = Solution::removeDuplicates(nums);
        assert(len == 5);  // [0,1,2,3,4]
    }
    std::cout << "  Remove Duplicates I (long): PASSED" << std::endl;

    // 测试删除重复项 II
    {
        std::vector<int> nums = {1, 1, 1, 2, 2, 3};
        int len = Solution::removeDuplicates2(nums);
        assert(len == 5);  // [1,1,2,2,3]
    }
    std::cout << "  Remove Duplicates II (basic): PASSED" << std::endl;

    {
        std::vector<int> nums = {0, 0, 1, 1, 1, 1, 2, 3, 3};
        int len = Solution::removeDuplicates2(nums);
        assert(len == 7);  // [0,0,1,1,2,3,3]
    }
    std::cout << "  Remove Duplicates II (complex): PASSED" << std::endl;
}

} // namespace RemoveDuplicatesImpl

/**
 * 关键要点：
 *
 * 1. 快慢指针原理：
 *    - slow: 已处理区域的末尾（不包含重复元素）
 *    - fast: 扫描整个数组
 *    - [0, slow] 是已处理的有效区域
 *    - [fast, n-1] 是待处理区域
 *
 * 2. 删除重复项 I（每个元素最多 1 次）：
 *    - nums[fast] != nums[slow] 时，写入新值
 *    - 因为数组有序，相同元素必然相邻
 *    - 只保留第一次出现的元素
 *
 * 3. 删除重复项 II（每个元素最多 2 次）：
 *    - nums[fast] != nums[slow-2] 时，写入新值
 *    - 为什么比较 slow-2：
 *      * slow-2 是倒数第二个已保留元素
 *      * 如果 nums[fast] == nums[slow-2]，说明已经有至少 2 个相同元素
 *      * 如果 nums[fast] != nums[slow-2]，可以安全写入
 *
 * 4. 泛化：保留 k 个相同元素
 *    - 初始化 slow = k
 *    - 从 fast = k 开始
 *    - 当 nums[fast] != nums[slow-k] 时，写入
 *
 * 5. 时间复杂度：O(n)
 *    - fast 遍历整个数组一次
 *
 * 6. 空间复杂度：O(1)
 *    - 原地修改，只使用常数额外空间
 *
 * 7. 原地修改技巧：
 *    - 不需要真的删除元素
 *    - 只需将有效元素移到数组前面
 *    - 返回新长度，后面的元素不影响结果
 */
