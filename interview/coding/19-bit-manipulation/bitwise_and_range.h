/**
 * @file bitwise_and_range.h
 * @brief 数字范围按位与 - 面试题
 */

#ifndef BITWISE_AND_RANGE_H
#define BITWISE_AND_RANGE_H

namespace BitwiseAndRangeImpl {

/**
 * 题目: 数字范围按位与 (LeetCode 201)
 * 给你两个整数 left 和 right，表示区间 [left, right]，
 * 返回此区间内所有数字按位与的结果（包含 left 、right 端点）。
 *
 * 示例:
 * 输入: left = 5, right = 7
 * 输出: 4
 * 解释: 5 = 101, 6 = 110, 7 = 111，按位与结果为 100 = 4
 *
 * 提示: 找到left和right的公共前缀
 */
int rangeBitwiseAnd(int left, int right);

/**
 * 方法2: Brian Kernighan算法
 * 不断消除right的最低位1
 */
int rangeBitwiseAndBK(int left, int right);

// 测试函数
void runTests();

} // namespace BitwiseAndRangeImpl

#endif // BITWISE_AND_RANGE_H
