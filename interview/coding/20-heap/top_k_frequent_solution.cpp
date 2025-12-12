/**
 * @file top_k_frequent_solution.cpp
 * @brief 前K个高频元素 - 解答
 */
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <iostream>
using namespace std;

// 前K个高频元素 - 最小堆
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int n : nums) freq[n]++;

    // 最小堆，按频率排序
    auto cmp = [](pair<int,int>& a, pair<int,int>& b) {
        return a.second > b.second;
    };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> minHeap(cmp);

    for (auto& [num, cnt] : freq) {
        minHeap.push({num, cnt});
        if (minHeap.size() > k) minHeap.pop();
    }

    vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().first);
        minHeap.pop();
    }
    return result;
}

// 前K个高频单词 - 需要按频率降序，同频按字典序
vector<string> topKFrequentWords(vector<string>& words, int k) {
    unordered_map<string, int> freq;
    for (const string& w : words) freq[w]++;

    // 最小堆: 频率低的优先，同频字典序大的优先
    auto cmp = [](pair<string,int>& a, pair<string,int>& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    };
    priority_queue<pair<string,int>, vector<pair<string,int>>, decltype(cmp)> minHeap(cmp);

    for (auto& [word, cnt] : freq) {
        minHeap.push({word, cnt});
        if (minHeap.size() > k) minHeap.pop();
    }

    vector<string> result(k);
    for (int i = k - 1; i >= 0; --i) {
        result[i] = minHeap.top().first;
        minHeap.pop();
    }
    return result;
}

int main() {
    vector<int> nums = {1,1,1,2,2,3};
    auto res1 = topKFrequent(nums, 2);
    cout << "topKFrequent([1,1,1,2,2,3], k=2): ";
    for (int n : res1) cout << n << " ";  // 1 2
    cout << "\n";

    vector<string> words = {"i","love","leetcode","i","love","coding"};
    auto res2 = topKFrequentWords(words, 2);
    cout << "topKFrequentWords: ";
    for (const string& w : res2) cout << w << " ";  // i love
    cout << "\n";
    return 0;
}
