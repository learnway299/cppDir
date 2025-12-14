/**
 * @file kth_largest.cpp
 * @brief 第K大元素 - 面试题实现
 */

#include "kth_largest.h"

namespace KthLargestImpl {

// ==================== 方法1: 最小堆 ====================

int findKthLargest(std::vector<int>& nums, int k) {
    // TODO: 实现使用最小堆查找第 k 大元素
    // 1. 创建大小为 k 的最小堆
    // 2. 遍历数组，维护堆中始终是最大的 k 个元素
    // 3. 返回堆顶（第 k 大）
    return 0;
}

// ==================== 方法2: 快速选择 ====================

int quickSelect(std::vector<int>& nums, int left, int right, int k) {
    // TODO: 实现快速选择算法
    // 1. 选择 pivot（通常选最右边元素）
    // 2. 分区：将 >= pivot 的元素放左边，< pivot 的放右边
    // 3. 根据 pivot 位置决定继续在哪一半查找
    return 0;
}

int findKthLargestQuickSelect(std::vector<int>& nums, int k) {
    // TODO: 调用 quickSelect
    return quickSelect(nums, 0, nums.size() - 1, k - 1);
}

// ==================== 数据流中的第K大 ====================

KthLargest::KthLargest(int k, std::vector<int>& nums) : k_(k) {
    // TODO: 初始化数据结构
    // 提示：使用最小堆维护前 k 大元素
}

int KthLargest::add(int val) {
    // TODO: 添加新元素并返回第 k 大
    // 1. 将元素加入堆
    // 2. 如果堆大小超过 k，弹出最小元素
    // 3. 返回堆顶
    return 0;
}

} // namespace KthLargestImpl
