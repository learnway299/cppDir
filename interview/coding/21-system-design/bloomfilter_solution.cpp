/**
 * @file bloomfilter_solution.cpp
 * @brief 布隆过滤器 - 参考解答
 */

#include "bloomfilter.h"
#include <iostream>
#include <cmath>

namespace BloomFilterImpl {
namespace Solution {

size_t hash(const std::string& key, int seed, size_t size) {
    std::hash<std::string> hasher;
    return hasher(key + std::to_string(seed)) % size;
}

class BloomFilter {
private:
    std::vector<bool> bits_;
    size_t size_;
    int numHashFunctions_;

    size_t hash(const std::string& key, int seed) const {
        std::hash<std::string> hasher;
        return hasher(key + std::to_string(seed)) % size_;
    }

public:
    BloomFilter(size_t size, int numHashFunctions)
        : bits_(size, false), size_(size), numHashFunctions_(numHashFunctions) {}

    void add(const std::string& key) {
        for (int i = 0; i < numHashFunctions_; ++i) {
            size_t idx = hash(key, i);
            bits_[idx] = true;
        }
    }

    bool mightContain(const std::string& key) const {
        for (int i = 0; i < numHashFunctions_; ++i) {
            size_t idx = hash(key, i);
            if (!bits_[idx]) return false;
        }
        return true;  // 可能存在 (有假阳性)
    }

    double estimateFalsePositiveRate(int insertedCount) const {
        double k = numHashFunctions_;
        double n = insertedCount;
        double m = static_cast<double>(size_);
        double exp_val = std::exp(-k * n / m);
        return std::pow(1.0 - exp_val, k);
    }
};

void runSolutionTests() {
    std::cout << "=== Bloom Filter Solution ===" << std::endl;

    BloomFilter bf(1000, 3);

    // 添加元素
    bf.add("apple");
    bf.add("banana");
    bf.add("cherry");

    // 测试存在性
    std::cout << "mightContain('apple'): " << bf.mightContain("apple") << std::endl;   // 1
    std::cout << "mightContain('banana'): " << bf.mightContain("banana") << std::endl; // 1
    std::cout << "mightContain('grape'): " << bf.mightContain("grape") << std::endl;   // 可能是0或1

    // 假阳性测试
    int falsePositives = 0;
    int tests = 10000;
    for (int i = 0; i < tests; ++i) {
        if (bf.mightContain("test" + std::to_string(i))) {
            falsePositives++;
        }
    }
    std::cout << "False positive rate (empirical): "
              << static_cast<double>(falsePositives) / tests * 100 << "%" << std::endl;

    std::cout << "False positive rate (estimated): "
              << bf.estimateFalsePositiveRate(3) * 100 << "%" << std::endl;

    std::cout << "\nBloom filter tests completed!" << std::endl;
}

} // namespace Solution

void runTests() {
    Solution::runSolutionTests();
}

} // namespace BloomFilterImpl
