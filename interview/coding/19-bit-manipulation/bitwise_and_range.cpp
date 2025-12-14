/**
 * @file bitwise_and_range.cpp
 * @brief 数字范围按位与 - 面试者实现
 */

#include "bitwise_and_range.h"

namespace BitwiseAndRangeImpl {

// 题目: 数字范围按位与 (LeetCode 201)
// 方法1: 找公共前缀
int rangeBitwiseAnd(int left, int right) {
    // TODO: 同时右移left和right直到相等，再左移回来
    return 0;
}

// 方法2: Brian Kernighan算法
int rangeBitwiseAndBK(int left, int right) {
    // TODO: 不断消除right的最低位1，直到right <= left
    return 0;
}

} // namespace BitwiseAndRangeImpl
