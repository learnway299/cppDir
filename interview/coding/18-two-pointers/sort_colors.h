/**
 * @file sort_colors.h
 * @brief 颜色分类 - 题目说明与声明
 *
 * 三指针经典问题：荷兰国旗问题（Dutch National Flag Problem）
 *
 * LeetCode 75: Sort Colors
 */

#ifndef SORT_COLORS_H
#define SORT_COLORS_H

#include <vector>

namespace SortColorsImpl {

/**
 * 题目: 颜色分类 (LeetCode 75)
 *
 * 给定一个包含红色、白色和蓝色，共 n 个元素的数组 nums，
 * 原地对它们进行排序，使得相同颜色的元素相邻，
 * 并按照红色、白色、蓝色顺序排列。
 *
 * 用整数 0、1 和 2 分别表示红色、白色和蓝色。
 *
 * 示例:
 *   输入: nums = [2,0,2,1,1,0]
 *   输出: [0,0,1,1,2,2]
 *
 * 要求:
 *   - 不能使用库的 sort 函数
 *   - 一次遍历（one-pass）
 *   - 空间复杂度 O(1)
 *
 * 提示 - 三指针（荷兰国旗问题）：
 *   - p0: 下一个 0 应该放置的位置
 *   - curr: 当前扫描的位置
 *   - p2: 下一个 2 应该放置的位置
 *   - 维护：[0, p0) 全是 0，[p0, curr) 全是 1，(p2, n-1] 全是 2
 *
 * 算法流程：
 *   - nums[curr] == 0: 与 p0 交换，p0++, curr++
 *   - nums[curr] == 1: curr++
 *   - nums[curr] == 2: 与 p2 交换，p2--（注意 curr 不动）
 */
void sortColors(std::vector<int>& nums);

// ==================== 测试函数 ====================

void runTests();

} // namespace SortColorsImpl

#endif // SORT_COLORS_H
