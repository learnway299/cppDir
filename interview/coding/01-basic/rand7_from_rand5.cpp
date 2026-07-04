/**
 * @file rand7_from_rand5.cpp
 * @brief rand7 from rand5 - candidate implementation file.
 */

#include "rand7_from_rand5.h"
#include <iostream>
#include <random>

namespace Rand7FromRand5 {

int oldRandom() {
    static thread_local std::mt19937 gen(20260308);
    static thread_local std::uniform_int_distribution<int> dist(1, 5);
    return dist(gen);
}

int newRandom() {
    // TODO: 只能调用 oldRandom()，返回 [1,7] 且等概率
    return 0;
}

void testRand7FromRand5() {
    std::cout << "=== Rand7FromRand5 Tests (User Implementation) ===\n";
    std::cout << "  (No tests yet - implement your solution first)\n";
}

} // namespace Rand7FromRand5
