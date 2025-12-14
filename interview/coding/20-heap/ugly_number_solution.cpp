/**
 * @file ugly_number_solution.cpp
 * @brief 丑数 - 解答
 */

#include "ugly_number.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <iostream>
#include <cassert>
#include <algorithm>

namespace UglyNumberImpl {
namespace Solution {

// ==================== 判断丑数 ====================

bool isUgly(int n) {
    if (n <= 0) return false;
    while (n % 2 == 0) n /= 2;
    while (n % 3 == 0) n /= 3;
    while (n % 5 == 0) n /= 5;
    return n == 1;
}

// ==================== 第n个丑数 - 三指针法 ====================

int nthUglyNumber(int n) {
    std::vector<int> ugly(n);
    ugly[0] = 1;
    int p2 = 0, p3 = 0, p5 = 0;

    for (int i = 1; i < n; ++i) {
        int next2 = ugly[p2] * 2;
        int next3 = ugly[p3] * 3;
        int next5 = ugly[p5] * 5;

        ugly[i] = std::min({next2, next3, next5});

        if (ugly[i] == next2) p2++;
        if (ugly[i] == next3) p3++;
        if (ugly[i] == next5) p5++;
    }
    return ugly[n - 1];
}

// ==================== 第n个丑数 - 最小堆法 ====================

int nthUglyNumberHeap(int n) {
    std::priority_queue<long, std::vector<long>, std::greater<long>> minHeap;
    std::unordered_set<long> seen;
    minHeap.push(1);
    seen.insert(1);

    std::vector<int> factors = {2, 3, 5};
    long ugly = 1;

    for (int i = 0; i < n; ++i) {
        ugly = minHeap.top();
        minHeap.pop();
        for (int f : factors) {
            long next = ugly * f;
            if (seen.find(next) == seen.end()) {
                seen.insert(next);
                minHeap.push(next);
            }
        }
    }
    return ugly;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Ugly Number Tests ===" << std::endl;

    // 测试判断丑数
    {
        assert(Solution::isUgly(6) == true);   // 2 * 3
        assert(Solution::isUgly(8) == true);   // 2^3
        assert(Solution::isUgly(14) == false); // 2 * 7
        assert(Solution::isUgly(1) == true);
    }
    std::cout << "  Is Ugly Number: PASSED" << std::endl;

    // 测试三指针法
    {
        assert(Solution::nthUglyNumber(10) == 12);
        assert(Solution::nthUglyNumber(1) == 1);
    }
    std::cout << "  Nth Ugly Number (pointers): PASSED" << std::endl;

    // 测试最小堆法
    {
        assert(Solution::nthUglyNumberHeap(10) == 12);
        assert(Solution::nthUglyNumberHeap(1) == 1);
    }
    std::cout << "  Nth Ugly Number (heap): PASSED" << std::endl;

    // 验证两种方法一致
    {
        for (int i = 1; i <= 20; ++i) {
            assert(Solution::nthUglyNumber(i) == Solution::nthUglyNumberHeap(i));
        }
    }
    std::cout << "  Both methods consistent: PASSED" << std::endl;
}

} // namespace UglyNumberImpl
