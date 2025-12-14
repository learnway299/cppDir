/**
 * @file bloomfilter.cpp
 * @brief 布隆过滤器 - 面试题实现
 */

#include "bloomfilter.h"

namespace BloomFilterImpl {

size_t BloomFilter::hash(const std::string& key, int seed) const {
    // TODO: 实现哈希函数，使用不同的种子生成不同的哈希值
    return 0;
}

BloomFilter::BloomFilter(size_t size, int numHashFunctions)
    : bits_(size, false), size_(size), numHashFunctions_(numHashFunctions) {
    // TODO: 初始化成员变量（已在初始化列表中完成）
}

void BloomFilter::add(const std::string& key) {
    // TODO: 实现添加元素
    // 提示：对每个哈希函数，计算哈希值并设置对应位为 true
}

bool BloomFilter::mightContain(const std::string& key) const {
    // TODO: 实现判断元素可能存在
    // 提示：检查所有哈希函数对应的位是否都为 true
    return false;
}

double BloomFilter::estimateFalsePositiveRate(int insertedCount) const {
    // TODO: 实现估算假阳性率
    // 公式: (1 - e^(-kn/m))^k
    // k = numHashFunctions_, n = insertedCount, m = size_
    return 0.0;
}

} // namespace BloomFilterImpl
