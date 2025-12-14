/**
 * @file move_zeroes.h
 * @brief 移动零 - 题目说明与声明
 *
 * 快慢指针经典问题：将所有零移到数组末尾
 *
 * LeetCode 283: Move Zeroes
 */

#ifndef MOVE_ZEROES_H
#define MOVE_ZEROES_H

#include <vector>

namespace MoveZeroesImpl {

/**
 * 题目: 移动零 (LeetCode 283)
 *
 * 给定一个数组 nums，编写一个函数将所有 0 移动到数组的末尾，
 * 同时保持非零元素的相对顺序。
 *
 * 示例:
 *   输入: nums = [0,1,0,3,12]
 *   输出: [1,3,12,0,0]
 *
 * 要求:
 *   - 必须在原数组上操作，不能使用额外数组
 *   - 尽量减少操作次数
 *
 * 提示:
 *   - 使用快慢指针
 *   - slow 指向下一个非零元素应该放置的位置
 *   - fast 扫描整个数组
 *   - 遇到非零元素时，与 slow 位置交换
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
void moveZeroes(std::vector<int>& nums);

// ==================== 测试函数 ====================

void runTests();

} // namespace MoveZeroesImpl

#endif // MOVE_ZEROES_H
