/**
 * @file divide_without_operator.h
 * @brief 不用除法实现除法 - 面试题
 */

#ifndef DIVIDE_WITHOUT_OPERATOR_H
#define DIVIDE_WITHOUT_OPERATOR_H

namespace DivideWithoutOperatorImpl {

/**
 * 题目: 两数相除 (LeetCode 29)
 * 给定两个整数，被除数 dividend 和除数 divisor。
 * 将两数相除，要求不使用乘法、除法和 mod 运算符。
 * 返回被除数 dividend 除以除数 divisor 得到的商。
 *
 * 示例:
 * 输入: dividend = 10, divisor = 3
 * 输出: 3
 * 解释: 10/3 = 3.33...，截断后为 3
 *
 * 输入: dividend = 7, divisor = -3
 * 输出: -2
 *
 * 提示: 使用位移实现倍增，注意溢出处理
 */
int divide(int dividend, int divisor);

// 测试函数
void runTests();

} // namespace DivideWithoutOperatorImpl

#endif // DIVIDE_WITHOUT_OPERATOR_H
