/**
 * @file divide_without_operator_solution.cpp
 * @brief 不用除法实现除法 - 解答
 */
#include <iostream>
#include <climits>
using namespace std;

// 位运算实现除法
// 通过倍增法，每次尝试减去 divisor * 2^k
int divide(int dividend, int divisor) {
    // 处理溢出情况
    if (dividend == INT_MIN && divisor == -1) return INT_MAX;
    if (dividend == INT_MIN && divisor == 1) return INT_MIN;

    // 记录结果符号
    bool negative = (dividend < 0) ^ (divisor < 0);

    // 转为负数处理，避免INT_MIN转正数溢出
    long a = dividend < 0 ? dividend : -(long)dividend;
    long b = divisor < 0 ? divisor : -(long)divisor;

    int result = 0;
    while (a <= b) {  // 负数比较，a <= b 表示 |a| >= |b|
        long temp = b;
        int count = 1;
        // 倍增找到最大的 2^k 使得 temp * 2^k <= a (负数)
        while (a <= temp + temp && temp + temp < 0) {
            temp += temp;
            count += count;
        }
        a -= temp;
        result += count;
    }

    return negative ? -result : result;
}

int main() {
    cout << "divide(10, 3): " << divide(10, 3) << "\n";    // 3
    cout << "divide(7, -3): " << divide(7, -3) << "\n";    // -2
    cout << "divide(-2147483648, -1): " << divide(-2147483648, -1) << "\n";  // 2147483647
    cout << "divide(1, 1): " << divide(1, 1) << "\n";      // 1
    return 0;
}
