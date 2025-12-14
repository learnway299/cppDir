/**
 * @file bloomfilter.h
 * @brief 布隆过滤器 - 面试题
 *
 * 题目：实现布隆过滤器
 *
 * 要求：
 * 1. 实现 BloomFilter 类，支持以下操作：
 *    - BloomFilter(size_t size, int numHashFunctions): 构造函数，指定位数组大小和哈希函数数量
 *    - void add(const std::string& key): 添加元素到布隆过滤器
 *    - bool mightContain(const std::string& key) const: 判断元素是否可能存在
 *
 * 2. 理解布隆过滤器的特性：
 *    - 如果返回 false，元素一定不存在
 *    - 如果返回 true，元素可能存在（有假阳性）
 *    - 无法删除元素
 *
 * 3. 实现多个哈希函数（使用不同的种子）
 *
 * 时间复杂度：
 * - add: O(k)，k 为哈希函数数量
 * - mightContain: O(k)
 *
 * 空间复杂度：O(m)，m 为位数组大小
 */

#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <string>
#include <vector>
#include <functional>

namespace BloomFilterImpl {

class BloomFilter {
private:
    std::vector<bool> bits_;
    size_t size_;
    int numHashFunctions_;
    std::hash<std::string> hasher_;

    // 使用不同的种子生成多个哈希值
    size_t hash(const std::string& key, int seed) const;

public:
    BloomFilter(size_t size, int numHashFunctions);

    void add(const std::string& key);

    bool mightContain(const std::string& key) const;

    // 估算假阳性率: (1 - e^(-kn/m))^k
    // k = 哈希函数数, n = 插入元素数, m = 位数组大小
    double estimateFalsePositiveRate(int insertedCount) const;
};

void runTests();

} // namespace BloomFilterImpl

#endif // BLOOMFILTER_H
