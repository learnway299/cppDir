/**
 * @file rand7_from_rand5_solution.cpp
 * @brief rand7 from rand5 - reference solution.
 */

#include "rand7_from_rand5.h"
#include <array>
#include <cassert>
#include <iostream>

namespace Rand7FromRand5 {

/**
 * 鎬濊矾: 鎷掔粷閲囨牱
 *
 * 1. oldRandom()-1 寰楀埌 0..4
 * 2. 涓ゆ璋冪敤缁勫悎鎴?0..24 鐨勭瓑姒傜巼鏁存暟:
 *      x = a * 5 + b
 * 3. 浠呬繚鐣?0..20锛?1 涓暟锛屾濂芥槸 7 鐨勫€嶆暟锛? * 4. 鏄犲皠涓?1..7:
 *      x % 7 + 1
 *
 * 鏈熸湜寰幆娆℃暟 = 25/21锛屾湡鏈?O(1)
 */
int newRandomSolution() {
    while (true) {
        int a = oldRandom() - 1;  // 0..4
        int b = oldRandom() - 1;  // 0..4
        int x = a * 5 + b;        // 0..24, uniform

        if (x < 21) {
            return (x % 7) + 1;   // 1..7, uniform
        }
    }
}

namespace {

double chiSquareScore(const std::array<int, 8>& cnt, int samples) {
    const double expected = static_cast<double>(samples) / 7.0;
    double score = 0.0;
    for (int i = 1; i <= 7; ++i) {
        const double diff = static_cast<double>(cnt[i]) - expected;
        score += diff * diff / expected;
    }
    return score;
}

} // namespace

void testRand7FromRand5Solution() {
    std::cout << "=== Rand7FromRand5 Tests (Solution) ===\n";

    constexpr int samples = 70000;
    std::array<int, 8> cnt{};

    for (int i = 0; i < samples; ++i) {
        int v = newRandomSolution();
        assert(v >= 1 && v <= 7);
        ++cnt[v];
    }
    std::cout << "  range check: PASSED\n";

    int minCount = cnt[1], maxCount = cnt[1];
    for (int i = 2; i <= 7; ++i) {
        if (cnt[i] < minCount) minCount = cnt[i];
        if (cnt[i] > maxCount) maxCount = cnt[i];
    }

    // Loose threshold to avoid flaky statistical failures.
    assert(maxCount - minCount < 2000);
    std::cout << "  rough uniformity (max-min): PASSED\n";

    const double score = chiSquareScore(cnt, samples);
    assert(score < 25.0);
    std::cout << "  chi-square check: PASSED\n";
}

} // namespace Rand7FromRand5


