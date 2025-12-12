/**
 * @file power_of_two_solution.cpp
 * @brief 2的幂 - 解答
 */
#include <iostream>
using namespace std;

// 2的幂: n > 0 且只有一个1
bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// 4的幂: 首先是2的幂，且1在奇数位上
// 0x55555555 = 0101 0101 ... (奇数位为1)
bool isPowerOfFour(int n) {
    return n > 0 && (n & (n - 1)) == 0 && (n & 0x55555555);
}

int main() {
    cout << "isPowerOfTwo(1): " << isPowerOfTwo(1) << "\n";   // 1
    cout << "isPowerOfTwo(16): " << isPowerOfTwo(16) << "\n"; // 1
    cout << "isPowerOfTwo(3): " << isPowerOfTwo(3) << "\n";   // 0

    cout << "isPowerOfFour(1): " << isPowerOfFour(1) << "\n";   // 1
    cout << "isPowerOfFour(16): " << isPowerOfFour(16) << "\n"; // 1
    cout << "isPowerOfFour(8): " << isPowerOfFour(8) << "\n";   // 0
    return 0;
}
