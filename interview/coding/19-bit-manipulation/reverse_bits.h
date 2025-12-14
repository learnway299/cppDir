/**
 * @file reverse_bits.h
 * @brief 颠倒二进制位 - 面试题
 */

#ifndef REVERSE_BITS_H
#define REVERSE_BITS_H

#include <cstdint>

namespace ReverseBitsImpl {

/**
 * 题目: 颠倒二进制位 (LeetCode 190)
 * 颠倒给定的32位无符号整数的二进制位。
 *
 * 示例:
 * 输入: 43261596 (二进制: 00000010100101000001111010011100)
 * 输出: 964176192 (二进制: 00111001011110000010100101000000)
 *
 * 提示: 可以逐位翻转，也可以使用分治法
 */
uint32_t reverseBits(uint32_t n);

/**
 * 方法2: 分治法
 * 通过掩码和位运算，分层次交换位
 */
uint32_t reverseBitsDivide(uint32_t n);

// 测试函数
void runTests();

} // namespace ReverseBitsImpl

#endif // REVERSE_BITS_H
