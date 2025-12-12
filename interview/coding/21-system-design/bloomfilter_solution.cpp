/**
 * @file bloomfilter_solution.cpp
 * @brief 布隆过滤器 - 解答
 */
#include <string>
#include <vector>
#include <functional>
#include <iostream>
using namespace std;

class BloomFilter {
    vector<bool> bits_;
    size_t size_;
    int numHashFunctions_;
    hash<string> hasher_;

    // 使用不同的种子生成多个哈希值
    size_t hash(const string& key, int seed) const {
        return hasher_(key + to_string(seed)) % size_;
    }

public:
    BloomFilter(size_t size, int numHashFunctions)
        : bits_(size, false), size_(size), numHashFunctions_(numHashFunctions) {}

    void add(const string& key) {
        for (int i = 0; i < numHashFunctions_; ++i) {
            size_t idx = hash(key, i);
            bits_[idx] = true;
        }
    }

    bool mightContain(const string& key) const {
        for (int i = 0; i < numHashFunctions_; ++i) {
            size_t idx = hash(key, i);
            if (!bits_[idx]) return false;
        }
        return true;  // 可能存在 (有假阳性)
    }

    // 估算假阳性率: (1 - e^(-kn/m))^k
    // k = 哈希函数数, n = 插入元素数, m = 位数组大小
    double estimateFalsePositiveRate(int insertedCount) const {
        double k = numHashFunctions_;
        double n = insertedCount;
        double m = size_;
        double exp_val = exp(-k * n / m);
        return pow(1.0 - exp_val, k);
    }
};

int main() {
    BloomFilter bf(1000, 3);

    // 添加元素
    bf.add("apple");
    bf.add("banana");
    bf.add("cherry");

    // 测试存在性
    cout << "mightContain('apple'): " << bf.mightContain("apple") << "\n";   // 1
    cout << "mightContain('banana'): " << bf.mightContain("banana") << "\n"; // 1
    cout << "mightContain('grape'): " << bf.mightContain("grape") << "\n";   // 可能是0或1

    // 假阳性测试
    int falsePositives = 0;
    int tests = 10000;
    for (int i = 0; i < tests; ++i) {
        if (bf.mightContain("test" + to_string(i))) {
            falsePositives++;
        }
    }
    cout << "False positive rate (empirical): "
         << (double)falsePositives / tests * 100 << "%\n";

    cout << "False positive rate (estimated): "
         << bf.estimateFalsePositiveRate(3) * 100 << "%\n";

    return 0;
}
