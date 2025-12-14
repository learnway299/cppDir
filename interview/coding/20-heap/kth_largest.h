/**
 * @file kth_largest.h
 * @brief 第K大元素 - 面试题
 *
 * 题目1: 数组中的第K个最大元素 (LeetCode 215)
 * 题目2: 数据流中的第K大元素 (LeetCode 703)
 *
 * 关键要点：
 *
 * 1. 最小堆方法：
 *    - 维护大小为 k 的最小堆
 *    - 堆顶就是第 k 大
 *    - 时间 O(n log k)
 *
 * 2. 快速选择方法：
 *    - 类似快排的分区
 *    - 只递归需要的那一半
 *    - 平均 O(n)，最坏 O(n²)
 *
 * 3. 数据流场景：
 *    - 持续添加元素
 *    - 每次返回当前第 k 大
 *    - 用最小堆维护前 k 大
 *
 * 4. 选择策略：
 *    - 数据量大、k 小：最小堆
 *    - 一次性查询：快速选择
 *    - 流式数据：KthLargest 类
 */

#ifndef KTH_LARGEST_H
#define KTH_LARGEST_H

#include <vector>

namespace KthLargestImpl {

// ==================== 方法1: 最小堆 ====================
int findKthLargest(std::vector<int>& nums, int k);

// ==================== 方法2: 快速选择 ====================
int quickSelect(std::vector<int>& nums, int left, int right, int k);
int findKthLargestQuickSelect(std::vector<int>& nums, int k);

// ==================== 数据流中的第K大 ====================
class KthLargest {
public:
    KthLargest(int k, std::vector<int>& nums);
    int add(int val);

private:
    int k_;
    // TODO: 添加必要的成员变量
};

// ==================== 测试函数 ====================
void runTests();

} // namespace KthLargestImpl

#endif // KTH_LARGEST_H
