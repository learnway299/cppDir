/**
 * @file kth_largest_solution.cpp
 * @brief 第K大元素 - 解答
 */

#include "kth_largest.h"
#include <queue>
#include <iostream>
#include <algorithm>
#include <cassert>

namespace KthLargestImpl {
namespace Solution {

// ==================== 方法1: 最小堆 ====================

int findKthLargest(std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    for (int n : nums) {
        minHeap.push(n);
        if (minHeap.size() > static_cast<size_t>(k)) minHeap.pop();
    }
    return minHeap.top();
}

// ==================== 方法2: 快速选择 ====================

int quickSelect(std::vector<int>& nums, int left, int right, int k) {
    int pivot = nums[right];
    int i = left;
    for (int j = left; j < right; ++j) {
        if (nums[j] >= pivot) {
            std::swap(nums[i++], nums[j]);
        }
    }
    std::swap(nums[i], nums[right]);

    if (i == k) return nums[i];
    else if (i < k) return quickSelect(nums, i + 1, right, k);
    else return quickSelect(nums, left, i - 1, k);
}

int findKthLargestQuickSelect(std::vector<int>& nums, int k) {
    return quickSelect(nums, 0, nums.size() - 1, k - 1);
}

// ==================== 数据流中的第K大 ====================

class KthLargestSolution {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    int k_;
public:
    KthLargestSolution(int k, std::vector<int>& nums) : k_(k) {
        for (int n : nums) add(n);
    }

    int add(int val) {
        minHeap.push(val);
        if (minHeap.size() > static_cast<size_t>(k_)) minHeap.pop();
        return minHeap.top();
    }
};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Kth Largest Tests ===" << std::endl;

    // 测试最小堆方法
    {
        std::vector<int> nums = {3, 2, 1, 5, 6, 4};
        assert(Solution::findKthLargest(nums, 2) == 5);
    }
    std::cout << "  Find Kth Largest (heap): PASSED" << std::endl;

    // 测试快速选择
    {
        std::vector<int> nums = {3, 2, 3, 1, 2, 4, 5, 5, 6};
        assert(Solution::findKthLargestQuickSelect(nums, 4) == 4);
    }
    std::cout << "  Find Kth Largest (quick select): PASSED" << std::endl;

    // 测试数据流
    {
        std::vector<int> init = {4, 5, 8, 2};
        Solution::KthLargestSolution kl(3, init);
        assert(kl.add(3) == 4);
        assert(kl.add(5) == 5);
        assert(kl.add(10) == 5);
    }
    std::cout << "  Kth Largest in Stream: PASSED" << std::endl;
}

} // namespace KthLargestImpl
