/**
 * @file top_k_frequent.cpp
 * @brief 前K个高频元素 - 面试题实现
 */

#include "top_k_frequent.h"

namespace TopKFrequentImpl {

// ==================== 前K个高频元素 ====================

std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
    // TODO: 实现前K个高频元素
    // 1. 使用 unordered_map 统计每个元素的频率
    // 2. 使用最小堆（按频率）维护前 k 个高频元素
    // 3. 遍历频率表，将元素加入堆
    //    - 如果堆大小 > k，弹出频率最小的
    // 4. 将堆中元素转换为结果数组
    return {};
}

// ==================== 前K个高频单词 ====================

std::vector<std::string> topKFrequentWords(std::vector<std::string>& words, int k) {
    // TODO: 实现前K个高频单词
    // 1. 使用 unordered_map 统计每个单词的频率
    // 2. 使用最小堆维护前 k 个高频单词
    //    - 比较器：频率小的优先，同频字典序大的优先
    // 3. 遍历频率表，将单词加入堆
    //    - 如果堆大小 > k，弹出最小的
    // 4. 将堆中元素按正确顺序转换为结果数组
    //    - 注意：需要反向填充（从后往前）
    return {};
}

} // namespace TopKFrequentImpl
