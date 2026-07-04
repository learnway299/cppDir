/**
 * @file rand7_from_rand5.h
 * @brief Build rand7() from rand5() interview problem.
 *
 * Evaluation focus:
 * 1. Uniform vs non-uniform randomness
 * 2. Rejection sampling
 * 3. Why naive sum/mod introduces bias
 */

#ifndef RAND7_FROM_RAND5_H
#define RAND7_FROM_RAND5_H

namespace Rand7FromRand5 {

// ==================== 题目声明 ====================

/**
 * 已有随机函数:
 * - 等概率返回 [1, 5]
 */
int oldRandom();

/**
 * 题目:
 * - 只能调用 oldRandom()，实现 newRandom()，使其等概率返回 [1, 7]
 */
int newRandom();
int newRandomSolution();

// ==================== 测试函数声明 ====================

void testRand7FromRand5();
void testRand7FromRand5Solution();

inline void runTests() {
    testRand7FromRand5();
    testRand7FromRand5Solution();
}

} // namespace Rand7FromRand5

#endif // RAND7_FROM_RAND5_H
