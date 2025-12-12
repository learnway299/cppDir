/**
 * @file hashmap_impl_solution.cpp
 * @brief 哈希表实现与应用 - 解答文件
 *
 * 哈希表核心要点：
 * 1. 哈希函数：将键映射到桶索引
 * 2. 冲突解决：链地址法、开放定址法
 * 3. 负载因子：动态扩容
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>

/**
 * 题目1: 设计哈希集合
 *
 * 解法: 链地址法
 * 时间复杂度: O(n/k)，k 为桶数量
 * 空间复杂度: O(k + n)
 */
class MyHashSet {
private:
    static const int BUCKET_SIZE = 769;  // 选择质数减少冲突
    std::vector<std::list<int>> buckets;

    int hash(int key) {
        return key % BUCKET_SIZE;
    }

public:
    MyHashSet() : buckets(BUCKET_SIZE) {}

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

/**
 * 简化版本：使用位图（适用于键范围已知）
 */
class MyHashSetBitmap {
private:
    std::vector<bool> data;

public:
    MyHashSetBitmap() : data(1000001, false) {}

    void add(int key) { data[key] = true; }
    void remove(int key) { data[key] = false; }
    bool contains(int key) { return data[key]; }
};

/**
 * 题目2: 设计哈希映射
 *
 * 解法: 链地址法
 */
class MyHashMap {
private:
    static const int BUCKET_SIZE = 769;
    std::vector<std::list<std::pair<int, int>>> buckets;

    int hash(int key) {
        return key % BUCKET_SIZE;
    }

public:
    MyHashMap() : buckets(BUCKET_SIZE) {}

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

/**
 * 题目3: 两数之和
 *
 * 解法: 哈希表
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<int> twoSum(std::vector<int>& nums, int target) {
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

/**
 * 题目4: 存在重复元素
 *
 * 解法: 哈希集合
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool containsDuplicate(std::vector<int>& nums) {
    std::unordered_set<int> seen;

    for (int num : nums) {
        if (seen.count(num)) {
            return true;
        }
        seen.insert(num);
    }

    return false;
}

/**
 * 题目5: 存在重复元素 II
 *
 * 解法: 滑动窗口 + 哈希集合
 * 时间复杂度: O(n)
 * 空间复杂度: O(min(n, k))
 */
bool containsNearbyDuplicate(std::vector<int>& nums, int k) {
    std::unordered_set<int> window;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        if (window.count(nums[i])) {
            return true;
        }
        window.insert(nums[i]);

        // 维护窗口大小为 k
        if (static_cast<int>(window.size()) > k) {
            window.erase(nums[i - k]);
        }
    }

    return false;
}

/**
 * 题目6: 字母异位词分组
 *
 * 解法: 排序作为键
 * 时间复杂度: O(n * k * log k)，k 为字符串平均长度
 * 空间复杂度: O(n * k)
 */
std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
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

/**
 * 使用字符计数作为键（更优）
 * 时间复杂度: O(n * k)
 */
std::vector<std::vector<std::string>> groupAnagramsCount(std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const std::string& str : strs) {
        std::vector<int> count(26, 0);
        for (char c : str) {
            ++count[c - 'a'];
        }

        // 构建键
        std::string key;
        for (int c : count) {
            key += std::to_string(c) + "#";
        }

        groups[key].push_back(str);
    }

    std::vector<std::vector<std::string>> result;
    for (auto& [key, group] : groups) {
        result.push_back(std::move(group));
    }

    return result;
}

/**
 * 题目7: 同构字符串
 *
 * 解法: 双向映射
 * 时间复杂度: O(n)
 * 空间复杂度: O(字符集大小)
 */
bool isIsomorphic(std::string s, std::string t) {
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

/**
 * 题目8: 有效的字母异位词
 *
 * 解法: 字符计数
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool isAnagram(std::string s, std::string t) {
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

/**
 * 题目9: 最长连续序列
 *
 * 解法: 哈希集合
 * 只从序列起点开始计数
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int longestConsecutive(std::vector<int>& nums) {
    std::unordered_set<int> numSet(nums.begin(), nums.end());
    int longest = 0;

    for (int num : numSet) {
        // 只从序列起点开始
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

/**
 * 题目10: 前 K 个高频元素
 *
 * 解法1: 桶排序
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int num : nums) {
        ++freq[num];
    }

    // 桶排序：buckets[i] 存储出现 i 次的元素
    int n = nums.size();
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

/**
 * 解法2: 小顶堆
 * 时间复杂度: O(n log k)
 * 空间复杂度: O(n)
 */
std::vector<int> topKFrequentHeap(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int num : nums) {
        ++freq[num];
    }

    // 小顶堆，按频率排序
    auto cmp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.second > b.second;
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> pq(cmp);

    for (auto& [num, count] : freq) {
        pq.push({num, count});
        if (static_cast<int>(pq.size()) > k) {
            pq.pop();
        }
    }

    std::vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().first);
        pq.pop();
    }

    return result;
}


// ==================== 测试代码 ====================
#include <cassert>

void testMyHashSet() {
    MyHashSet myHashSet;
    myHashSet.add(1);
    myHashSet.add(2);
    assert(myHashSet.contains(1) == true);
    assert(myHashSet.contains(3) == false);
    myHashSet.add(2);
    assert(myHashSet.contains(2) == true);
    myHashSet.remove(2);
    assert(myHashSet.contains(2) == false);

    std::cout << "MyHashSet: PASSED\n";
}

void testMyHashMap() {
    MyHashMap myHashMap;
    myHashMap.put(1, 1);
    myHashMap.put(2, 2);
    assert(myHashMap.get(1) == 1);
    assert(myHashMap.get(3) == -1);
    myHashMap.put(2, 1);
    assert(myHashMap.get(2) == 1);
    myHashMap.remove(2);
    assert(myHashMap.get(2) == -1);

    std::cout << "MyHashMap: PASSED\n";
}

void testTwoSum() {
    std::vector<int> nums = {2, 7, 11, 15};
    std::vector<int> expected = {0, 1};
    assert(twoSum(nums, 9) == expected);

    std::cout << "twoSum: PASSED\n";
}

void testContainsDuplicate() {
    std::vector<int> nums1 = {1, 2, 3, 1};
    assert(containsDuplicate(nums1) == true);

    std::vector<int> nums2 = {1, 2, 3, 4};
    assert(containsDuplicate(nums2) == false);

    std::cout << "containsDuplicate: PASSED\n";
}

void testContainsNearbyDuplicate() {
    std::vector<int> nums1 = {1, 2, 3, 1};
    assert(containsNearbyDuplicate(nums1, 3) == true);

    std::vector<int> nums2 = {1, 2, 3, 1, 2, 3};
    assert(containsNearbyDuplicate(nums2, 2) == false);

    std::cout << "containsNearbyDuplicate: PASSED\n";
}

void testGroupAnagrams() {
    std::vector<std::string> strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    auto result = groupAnagrams(strs);
    assert(result.size() == 3);

    auto result2 = groupAnagramsCount(strs);
    assert(result2.size() == 3);

    std::cout << "groupAnagrams: PASSED\n";
}

void testIsIsomorphic() {
    assert(isIsomorphic("egg", "add") == true);
    assert(isIsomorphic("foo", "bar") == false);
    assert(isIsomorphic("paper", "title") == true);

    std::cout << "isIsomorphic: PASSED\n";
}

void testIsAnagram() {
    assert(isAnagram("anagram", "nagaram") == true);
    assert(isAnagram("rat", "car") == false);

    std::cout << "isAnagram: PASSED\n";
}

void testLongestConsecutive() {
    std::vector<int> nums1 = {100, 4, 200, 1, 3, 2};
    assert(longestConsecutive(nums1) == 4);

    std::vector<int> nums2 = {0, 3, 7, 2, 5, 8, 4, 6, 0, 1};
    assert(longestConsecutive(nums2) == 9);

    std::cout << "longestConsecutive: PASSED\n";
}

void testTopKFrequent() {
    std::vector<int> nums = {1, 1, 1, 2, 2, 3};
    auto result = topKFrequent(nums, 2);
    std::sort(result.begin(), result.end());
    std::vector<int> expected = {1, 2};
    assert(result == expected);

    auto result2 = topKFrequentHeap(nums, 2);
    std::sort(result2.begin(), result2.end());
    assert(result2 == expected);

    std::cout << "topKFrequent: PASSED\n";
}

int main() {
    std::cout << "=== HashMap Implementation Solution Tests ===\n";

    testMyHashSet();
    testMyHashMap();
    testTwoSum();
    testContainsDuplicate();
    testContainsNearbyDuplicate();
    testGroupAnagrams();
    testIsIsomorphic();
    testIsAnagram();
    testLongestConsecutive();
    testTopKFrequent();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
