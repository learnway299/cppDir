/**
 * @file top_k_frequent_solution.cpp
 * @brief 前K个高频元素 - 解答
 */

#include "top_k_frequent.h"
#include <unordered_map>
#include <queue>
#include <iostream>
#include <cassert>
#include <algorithm>

namespace TopKFrequentImpl {
namespace Solution {

// ==================== 前K个高频元素 ====================

std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int n : nums) freq[n]++;

    // 最小堆，按频率排序
    auto cmp = [](std::pair<int,int>& a, std::pair<int,int>& b) {
        return a.second > b.second;
    };
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, decltype(cmp)> minHeap(cmp);

    for (auto& [num, cnt] : freq) {
        minHeap.push({num, cnt});
        if (minHeap.size() > static_cast<size_t>(k)) minHeap.pop();
    }

    std::vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().first);
        minHeap.pop();
    }
    return result;
}

// ==================== 前K个高频单词 ====================

std::vector<std::string> topKFrequentWords(std::vector<std::string>& words, int k) {
    std::unordered_map<std::string, int> freq;
    for (const std::string& w : words) freq[w]++;

    // 最小堆: 频率低的优先，同频字典序大的优先
    auto cmp = [](std::pair<std::string,int>& a, std::pair<std::string,int>& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    };
    std::priority_queue<std::pair<std::string,int>, std::vector<std::pair<std::string,int>>, decltype(cmp)> minHeap(cmp);

    for (auto& [word, cnt] : freq) {
        minHeap.push({word, cnt});
        if (minHeap.size() > static_cast<size_t>(k)) minHeap.pop();
    }

    std::vector<std::string> result(k);
    for (int i = k - 1; i >= 0; --i) {
        result[i] = minHeap.top().first;
        minHeap.pop();
    }
    return result;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Top K Frequent Tests ===" << std::endl;

    // 测试前K个高频元素
    {
        std::vector<int> nums = {1, 1, 1, 2, 2, 3};
        auto result = Solution::topKFrequent(nums, 2);
        std::sort(result.begin(), result.end());
        assert(result.size() == 2);
        assert(std::find(result.begin(), result.end(), 1) != result.end());
        assert(std::find(result.begin(), result.end(), 2) != result.end());
    }
    std::cout << "  Top K Frequent Elements: PASSED" << std::endl;

    // 测试前K个高频单词
    {
        std::vector<std::string> words = {"i", "love", "leetcode", "i", "love", "coding"};
        auto result = Solution::topKFrequentWords(words, 2);
        assert(result.size() == 2);
        assert(result[0] == "i");
        assert(result[1] == "love");
    }
    std::cout << "  Top K Frequent Words: PASSED" << std::endl;

    // 测试单个元素
    {
        std::vector<int> nums = {1};
        auto result = Solution::topKFrequent(nums, 1);
        assert(result.size() == 1);
        assert(result[0] == 1);
    }
    std::cout << "  Top K Frequent (single): PASSED" << std::endl;
}

} // namespace TopKFrequentImpl
