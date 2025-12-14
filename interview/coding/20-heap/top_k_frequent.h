/**
 * @file top_k_frequent.h
 * @brief 前K个高频元素 - 面试题
 *
 * 题目1: 前K个高频元素 (LeetCode 347)
 * 题目2: 前K个高频单词 (LeetCode 692)
 *
 * 关键要点：
 *
 * 1. 统计频率：
 *    - 使用 unordered_map 统计
 *
 * 2. 最小堆维护 Top K：
 *    - 堆大小为 k
 *    - 超过 k 时弹出最小的
 *    - 最后堆里就是前 k 个
 *
 * 3. 自定义比较器：
 *    - 数字: 按频率升序
 *    - 单词: 频率升序，同频字典序降序
 *
 * 4. 其他方法：
 *    - 桶排序: O(n)
 *    - 快速选择: O(n) 平均
 *
 * 5. 时间复杂度：O(n log k)
 * 6. 空间复杂度：O(n)
 */

#ifndef TOP_K_FREQUENT_H
#define TOP_K_FREQUENT_H

#include <vector>
#include <string>

namespace TopKFrequentImpl {

// ==================== 前K个高频元素 ====================
std::vector<int> topKFrequent(std::vector<int>& nums, int k);

// ==================== 前K个高频单词 ====================
std::vector<std::string> topKFrequentWords(std::vector<std::string>& words, int k);

// ==================== 测试函数 ====================
void runTests();

} // namespace TopKFrequentImpl

#endif // TOP_K_FREQUENT_H
