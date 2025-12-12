/**
 * @file reverse_bits_solution.cpp
 * @brief 颠倒二进制位 - 解答
 */
#include <cstdint>
#include <iostream>
#include <bitset>
using namespace std;

// 方法1: 逐位翻转
uint32_t reverseBits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; ++i) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}

// 方法2: 分治法 (更高效)
uint32_t reverseBitsDivide(uint32_t n) {
    n = ((n & 0xffff0000) >> 16) | ((n & 0x0000ffff) << 16);
    n = ((n & 0xff00ff00) >> 8)  | ((n & 0x00ff00ff) << 8);
    n = ((n & 0xf0f0f0f0) >> 4)  | ((n & 0x0f0f0f0f) << 4);
    n = ((n & 0xcccccccc) >> 2)  | ((n & 0x33333333) << 2);
    n = ((n & 0xaaaaaaaa) >> 1)  | ((n & 0x55555555) << 1);
    return n;
}

int main() {
    uint32_t n = 43261596;
    cout << "Original: " << bitset<32>(n) << "\n";

    uint32_t r1 = reverseBits(n);
    cout << "Reversed: " << bitset<32>(r1) << "\n";
    cout << "Value: " << r1 << "\n";  // 964176192

    uint32_t r2 = reverseBitsDivide(n);
    cout << "Divide method: " << r2 << "\n";
    return 0;
}
