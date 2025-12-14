/**
 * @file remove_duplicates.h
 * @brief 删除有序数组中的重复项 - 题目说明与声明
 *
 * 快慢指针经典问题：原地删除数组中的重复元素
 *
 * LeetCode 26: Remove Duplicates from Sorted Array
 * LeetCode 80: Remove Duplicates from Sorted Array II
 */

#ifndef REMOVE_DUPLICATES_H
#define REMOVE_DUPLICATES_H

#include <vector>

namespace RemoveDuplicatesImpl {

/**
 * 题目1: 删除有序数组中的重复项 (LeetCode 26)
 *
 * 给定一个有序数组 nums，原地删除重复出现的元素，
 * 使每个元素只出现一次，返回删除后数组的新长度。
 *
 * 示例:
 *   输入: nums = [1,1,2]
 *   输出: 2, nums = [1,2,_]
 *
 * 提示:
 *   - 使用快慢指针
 *   - slow 指向已处理区域的末尾
 *   - fast 扫描整个数组
 *   - 当 nums[fast] != nums[slow] 时，将 nums[fast] 写入 nums[++slow]
 *
 * 要求:
 *   - 时间复杂度 O(n)
 *   - 空间复杂度 O(1)
 */
int removeDuplicates(std::vector<int>& nums);

/**
 * 题目2: 删除有序数组中的重复项 II (LeetCode 80)
 *
 * 给定一个有序数组 nums，原地删除重复出现的元素，
 * 使每个元素最多出现两次，返回删除后数组的新长度。
 *
 * 示例:
 *   输入: nums = [1,1,1,2,2,3]
 *   输出: 5, nums = [1,1,2,2,3,_]
 *
 * 提示:
 *   - 使用快慢指针
 *   - 当 nums[fast] != nums[slow-2] 时，写入新值
 *   - 泛化：保留 k 个，比较 nums[fast] 和 nums[slow-k]
 *
 * 要求:
 *   - 时间复杂度 O(n)
 *   - 空间复杂度 O(1)
 */
int removeDuplicates2(std::vector<int>& nums);

// ==================== 测试函数 ====================

void runTests();

} // namespace RemoveDuplicatesImpl

#endif // REMOVE_DUPLICATES_H
