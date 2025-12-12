/**
 * @file counting_bits_solution.cpp
 * @brief 比特位计数 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

// 位1的个数 - Brian Kernighan 算法
int hammingWeight(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // 消除最低位的1
        count++;
    }
    return count;
}

// 比特位计数 - 动态规划
// dp[i] = dp[i >> 1] + (i & 1)
vector<int> countBits(int n) {
    vector<int> dp(n + 1);
    for (int i = 1; i <= n; ++i) {
        dp[i] = dp[i >> 1] + (i & 1);
    }
    return dp;
}

int main() {
    cout << "hammingWeight(11): " << hammingWeight(11) << "\n";  // 3
    cout << "hammingWeight(128): " << hammingWeight(128) << "\n"; // 1

    auto bits = countBits(5);
    cout << "countBits(5): ";
    for (int b : bits) cout << b << " ";  // 0 1 1 2 1 2
    cout << "\n";
    return 0;
}
