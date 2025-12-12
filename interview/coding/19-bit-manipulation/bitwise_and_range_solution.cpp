/**
 * @file bitwise_and_range_solution.cpp
 * @brief 数字范围按位与 - 解答
 */
#include <iostream>
using namespace std;

// 找公共前缀: 不断右移直到相等
// 因为范围内的数按位与，只有公共前缀部分会保留
int rangeBitwiseAnd(int left, int right) {
    int shift = 0;
    while (left < right) {
        left >>= 1;
        right >>= 1;
        shift++;
    }
    return left << shift;
}

// Brian Kernighan: 不断消除right最右边的1
int rangeBitwiseAndBK(int left, int right) {
    while (left < right) {
        right &= (right - 1);
    }
    return right;
}

int main() {
    cout << "rangeBitwiseAnd(5, 7): " << rangeBitwiseAnd(5, 7) << "\n";  // 4
    // 5 = 101, 6 = 110, 7 = 111 -> 101 & 110 & 111 = 100 = 4

    cout << "rangeBitwiseAnd(0, 0): " << rangeBitwiseAnd(0, 0) << "\n";  // 0

    cout << "rangeBitwiseAnd(1, 2147483647): " << rangeBitwiseAnd(1, 2147483647) << "\n";  // 0

    cout << "BK method(5, 7): " << rangeBitwiseAndBK(5, 7) << "\n";  // 4
    return 0;
}
