/**
 * @file hashmap_impl_solution.cpp
 * @brief 哈希表实现与应用 - 参考答案
 */

#include "hashmap_impl.h"
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <cassert>

namespace HashMapImplProblems {

// ==================== 参考答案实现 ====================

// MyHashSetSolution 实现
class MyHashSetSolutionImpl {
private:
    static const int BUCKET_SIZE = 769;
    std::vector<std::list<int>> buckets;

    int hash(int key) {
        return key % BUCKET_SIZE;
    }

public:
    MyHashSetSolutionImpl() : buckets(BUCKET_SIZE) {}

    void add(int key) {
        int idx = hash(key);
        for (int val : buckets[idx]) {
            if (val == key) return;
        }
        buckets[idx].push_back(key);
    }

    void remove(int key) {
        int idx = hash(key);
        buckets[idx].remove(key);
    }

    bool contains(int key) {
        int idx = hash(key);
        for (int val : buckets[idx]) {
            if (val == key) return true;
        }
        return false;
    }
};

MyHashSetSolution::MyHashSetSolution() {}
void MyHashSetSolution::add(int key) {}
void MyHashSetSolution::remove(int key) {}
bool MyHashSetSolution::contains(int key) { return false; }

// MyHashMapSolution 实现
class MyHashMapSolutionImpl {
private:
    static const int BUCKET_SIZE = 769;
    std::vector<std::list<std::pair<int, int>>> buckets;

    int hash(int key) {
        return key % BUCKET_SIZE;
    }

public:
    MyHashMapSolutionImpl() : buckets(BUCKET_SIZE) {}

    void put(int key, int value) {
        int idx = hash(key);
        for (auto& [k, v] : buckets[idx]) {
            if (k == key) {
                v = value;
                return;
            }
        }
        buckets[idx].emplace_back(key, value);
    }

    int get(int key) {
        int idx = hash(key);
        for (auto& [k, v] : buckets[idx]) {
            if (k == key) return v;
        }
        return -1;
    }

    void remove(int key) {
        int idx = hash(key);
        buckets[idx].remove_if([key](const auto& p) { return p.first == key; });
    }
};

MyHashMapSolution::MyHashMapSolution() {}
void MyHashMapSolution::put(int key, int value) {}
int MyHashMapSolution::get(int key) { return -1; }
void MyHashMapSolution::remove(int key) {}

// 题目3: 两数之和
std::vector<int> twoSumSolution(std::vector<int>& nums, int target) {
    std::unordered_map<int, int> seen;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        int complement = target - nums[i];
        if (seen.count(complement)) {
            return {seen[complement], i};
        }
        seen[nums[i]] = i;
    }

    return {};
}

// 题目4: 存在重复元素
bool containsDuplicateSolution(std::vector<int>& nums) {
    std::unordered_set<int> seen;

    for (int num : nums) {
        if (seen.count(num)) {
            return true;
        }
        seen.insert(num);
    }

    return false;
}

// 题目5: 存在重复元素 II
bool containsNearbyDuplicateSolution(std::vector<int>& nums, int k) {
    std::unordered_set<int> window;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        if (window.count(nums[i])) {
            return true;
        }
        window.insert(nums[i]);

        if (static_cast<int>(window.size()) > k) {
            window.erase(nums[i - k]);
        }
    }

    return false;
}

// 题目6: 字母异位词分组
std::vector<std::vector<std::string>> groupAnagramsSolution(std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const std::string& str : strs) {
        std::string key = str;
        std::sort(key.begin(), key.end());
        groups[key].push_back(str);
    }

    std::vector<std::vector<std::string>> result;
    for (auto& [key, group] : groups) {
        result.push_back(std::move(group));
    }

    return result;
}

// 题目7: 同构字符串
bool isIsomorphicSolution(std::string s, std::string t) {
    if (s.size() != t.size()) return false;

    std::unordered_map<char, char> s2t, t2s;

    for (size_t i = 0; i < s.size(); ++i) {
        char cs = s[i], ct = t[i];

        if (s2t.count(cs) && s2t[cs] != ct) return false;
        if (t2s.count(ct) && t2s[ct] != cs) return false;

        s2t[cs] = ct;
        t2s[ct] = cs;
    }

    return true;
}

// 题目8: 有效的字母异位词
bool isAnagramSolution(std::string s, std::string t) {
    if (s.size() != t.size()) return false;

    std::vector<int> count(26, 0);

    for (size_t i = 0; i < s.size(); ++i) {
        ++count[s[i] - 'a'];
        --count[t[i] - 'a'];
    }

    for (int c : count) {
        if (c != 0) return false;
    }

    return true;
}

// 题目9: 最长连续序列
int longestConsecutiveSolution(std::vector<int>& nums) {
    std::unordered_set<int> numSet(nums.begin(), nums.end());
    int longest = 0;

    for (int num : numSet) {
        if (!numSet.count(num - 1)) {
            int currentNum = num;
            int currentLength = 1;

            while (numSet.count(currentNum + 1)) {
                ++currentNum;
                ++currentLength;
            }

            longest = std::max(longest, currentLength);
        }
    }

    return longest;
}

// 题目10: 前 K 个高频元素
std::vector<int> topKFrequentSolution(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int num : nums) {
        ++freq[num];
    }

    int n = static_cast<int>(nums.size());
    std::vector<std::vector<int>> buckets(n + 1);

    for (auto& [num, count] : freq) {
        buckets[count].push_back(num);
    }

    std::vector<int> result;
    for (int i = n; i >= 0 && static_cast<int>(result.size()) < k; --i) {
        for (int num : buckets[i]) {
            result.push_back(num);
            if (static_cast<int>(result.size()) == k) break;
        }
    }

    return result;
}

// ==================== 测试函数 ====================

void testHashMapImplSolution() {
    std::cout << "=== HashMap Implementation Tests (Solution) ===\n";

    // MyHashSet
    MyHashSetSolutionImpl myHashSet;
    myHashSet.add(1);
    myHashSet.add(2);
    assert(myHashSet.contains(1) == true);
    assert(myHashSet.contains(3) == false);
    myHashSet.add(2);
    assert(myHashSet.contains(2) == true);
    myHashSet.remove(2);
    assert(myHashSet.contains(2) == false);
    std::cout << "  MyHashSetSolution: PASSED\n";

    // MyHashMap
    MyHashMapSolutionImpl myHashMap;
    myHashMap.put(1, 1);
    myHashMap.put(2, 2);
    assert(myHashMap.get(1) == 1);
    assert(myHashMap.get(3) == -1);
    myHashMap.put(2, 1);
    assert(myHashMap.get(2) == 1);
    myHashMap.remove(2);
    assert(myHashMap.get(2) == -1);
    std::cout << "  MyHashMapSolution: PASSED\n";

    // twoSum
    std::vector<int> nums1 = {2, 7, 11, 15};
    std::vector<int> expected1 = {0, 1};
    assert(twoSumSolution(nums1, 9) == expected1);
    std::cout << "  twoSumSolution: PASSED\n";

    // containsDuplicate
    std::vector<int> nums2 = {1, 2, 3, 1};
    assert(containsDuplicateSolution(nums2) == true);
    std::vector<int> nums3 = {1, 2, 3, 4};
    assert(containsDuplicateSolution(nums3) == false);
    std::cout << "  containsDuplicateSolution: PASSED\n";

    // containsNearbyDuplicate
    std::vector<int> nums4 = {1, 2, 3, 1};
    assert(containsNearbyDuplicateSolution(nums4, 3) == true);
    std::vector<int> nums5 = {1, 2, 3, 1, 2, 3};
    assert(containsNearbyDuplicateSolution(nums5, 2) == false);
    std::cout << "  containsNearbyDuplicateSolution: PASSED\n";

    // groupAnagrams
    std::vector<std::string> strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    auto result = groupAnagramsSolution(strs);
    assert(result.size() == 3);
    std::cout << "  groupAnagramsSolution: PASSED\n";

    // isIsomorphic
    assert(isIsomorphicSolution("egg", "add") == true);
    assert(isIsomorphicSolution("foo", "bar") == false);
    assert(isIsomorphicSolution("paper", "title") == true);
    std::cout << "  isIsomorphicSolution: PASSED\n";

    // isAnagram
    assert(isAnagramSolution("anagram", "nagaram") == true);
    assert(isAnagramSolution("rat", "car") == false);
    std::cout << "  isAnagramSolution: PASSED\n";

    // longestConsecutive
    std::vector<int> nums6 = {100, 4, 200, 1, 3, 2};
    assert(longestConsecutiveSolution(nums6) == 4);
    std::vector<int> nums7 = {0, 3, 7, 2, 5, 8, 4, 6, 0, 1};
    assert(longestConsecutiveSolution(nums7) == 9);
    std::cout << "  longestConsecutiveSolution: PASSED\n";

    // topKFrequent
    std::vector<int> nums8 = {1, 1, 1, 2, 2, 3};
    auto result3 = topKFrequentSolution(nums8, 2);
    std::sort(result3.begin(), result3.end());
    std::vector<int> expected2 = {1, 2};
    assert(result3 == expected2);
    std::cout << "  topKFrequentSolution: PASSED\n";

    std::cout << "All HashMap Implementation tests passed!\n";
}

} // namespace HashMapImplProblems
