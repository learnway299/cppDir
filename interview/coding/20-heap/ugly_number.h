/**
 * @file ugly_number.h
 * @brief 丑数 - 面试题
 *
 * 题目1: 丑数 (LeetCode 263)
 * 判断一个数是否为丑数(质因数只包含2,3,5)
 *
 * 题目2: 丑数 II (LeetCode 264)
 * 返回第n个丑数
 *
 * 关键要点：
 *
 * 1. 丑数定义：
 *    - 只包含因子 2, 3, 5 的正整数
 *    - 1 是第一个丑数
 *
 * 2. 判断丑数：
 *    - 不断除以 2, 3, 5
 *    - 最后看是否为 1
 *
 * 3. 三指针法：
 *    - 每个丑数乘 2, 3, 5 得到新丑数
 *    - 三个指针分别指向要乘的丑数
 *    - 取最小值作为下一个丑数
 *
 * 4. 最小堆法：
 *    - 用堆维护候选丑数
 *    - 每次取最小的
 *    - 用 set 去重
 *
 * 5. 时间复杂度：
 *    - 三指针: O(n)
 *    - 最小堆: O(n log n)
 */

#ifndef UGLY_NUMBER_H
#define UGLY_NUMBER_H

namespace UglyNumberImpl {

// ==================== 判断丑数 ====================
bool isUgly(int n);

// ==================== 第n个丑数 - 三指针法 ====================
int nthUglyNumber(int n);

// ==================== 第n个丑数 - 最小堆法 ====================
int nthUglyNumberHeap(int n);

// ==================== 测试函数 ====================
void runTests();

} // namespace UglyNumberImpl

#endif // UGLY_NUMBER_H
