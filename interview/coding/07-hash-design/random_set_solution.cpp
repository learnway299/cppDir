/**
 * @file random_set_solution.cpp
 * @brief O(1) 随机集合实现 - 解答文件
 *
 * 核心思想：哈希表 + 动态数组
 * - 哈希表：val -> index，O(1) 查找
 * - 数组：存储元素，O(1) 随机访问
 * - 删除技巧：与最后元素交换，然后删除最后一个
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>

/**
 * 题目1: O(1) 时间插入、删除和获取随机元素
 *
 * 数据结构：
 * - vector<int> nums: 存储元素
 * - unordered_map<int, int> valToIndex: val -> index
 *
 * 关键操作：
 * - insert: 尾部添加，O(1)
 * - remove: 与最后元素交换后 pop_back，O(1)
 * - getRandom: 随机索引访问，O(1)
 *
 * 时间复杂度: O(1) 平均
 * 空间复杂度: O(n)
 */
class RandomizedSet {
private:
    std::vector<int> nums;
    std::unordered_map<int, int> valToIndex;
    std::mt19937 rng;

public:
    RandomizedSet() : rng(std::random_device{}()) {}

    bool insert(int val) {
        if (valToIndex.count(val)) {
            return false;
        }

        valToIndex[val] = nums.size();
        nums.push_back(val);
        return true;
    }

    bool remove(int val) {
        auto it = valToIndex.find(val);
        if (it == valToIndex.end()) {
            return false;
        }

        int index = it->second;
        int lastVal = nums.back();

        // 将最后一个元素移动到待删除位置
        nums[index] = lastVal;
        valToIndex[lastVal] = index;

        // 删除最后一个元素
        nums.pop_back();
        valToIndex.erase(val);

        return true;
    }

    int getRandom() {
        std::uniform_int_distribution<int> dist(0, nums.size() - 1);
        return nums[dist(rng)];
    }
};

/**
 * 题目2: O(1) 时间插入、删除和获取随机元素 - 允许重复
 *
 * 与 RandomizedSet 的区别：
 * - 每个值可能对应多个索引
 * - 使用 unordered_map<int, unordered_set<int>> 存储 val -> indices
 *
 * 删除策略：
 * - 从该值的索引集合中取一个索引
 * - 与最后元素交换
 * - 更新相关索引集合
 *
 * 时间复杂度: O(1) 平均
 * 空间复杂度: O(n)
 */
class RandomizedCollection {
private:
    std::vector<int> nums;
    std::unordered_map<int, std::unordered_set<int>> valToIndices;
    std::mt19937 rng;

public:
    RandomizedCollection() : rng(std::random_device{}()) {}

    bool insert(int val) {
        bool notExist = !valToIndices.count(val) || valToIndices[val].empty();

        valToIndices[val].insert(nums.size());
        nums.push_back(val);

        return notExist;
    }

    bool remove(int val) {
        if (!valToIndices.count(val) || valToIndices[val].empty()) {
            return false;
        }

        // 获取要删除的索引
        int removeIdx = *valToIndices[val].begin();
        int lastVal = nums.back();
        int lastIdx = nums.size() - 1;

        // 将最后一个元素移动到待删除位置
        nums[removeIdx] = lastVal;

        // 更新索引集合
        valToIndices[val].erase(removeIdx);
        valToIndices[lastVal].erase(lastIdx);

        if (removeIdx != lastIdx) {
            valToIndices[lastVal].insert(removeIdx);
        }

        nums.pop_back();

        return true;
    }

    int getRandom() {
        std::uniform_int_distribution<int> dist(0, nums.size() - 1);
        return nums[dist(rng)];
    }
};

/**
 * 题目3: 黑名单中的随机数
 *
 * 思路：映射法
 * - 有效范围 [0, n - blacklist.size())
 * - 将落在有效范围内的黑名单数字映射到范围外的白名单数字
 *
 * 构造过程：
 * 1. 找出 [n - blacklist.size(), n) 区间内的白名单数字
 * 2. 将 [0, n - blacklist.size()) 区间内的黑名单数字映射到这些白名单数字
 *
 * pick 时只需在 [0, whitelistSize) 范围内随机，如果命中映射则返回映射值
 *
 * 时间复杂度: 构造 O(B), pick O(1)
 * 空间复杂度: O(B)，B = blacklist.size()
 */
class Solution {
private:
    int whitelistSize;
    std::unordered_map<int, int> blackToWhite;
    std::mt19937 rng;

public:
    Solution(int n, std::vector<int>& blacklist) : rng(std::random_device{}()) {
        whitelistSize = n - blacklist.size();

        // 将黑名单转为集合，方便查找
        std::unordered_set<int> blackSet(blacklist.begin(), blacklist.end());

        // 找出 [whitelistSize, n) 区间内的白名单数字
        std::vector<int> whitesInTail;
        for (int i = whitelistSize; i < n; ++i) {
            if (!blackSet.count(i)) {
                whitesInTail.push_back(i);
            }
        }

        // 将 [0, whitelistSize) 区间内的黑名单数字映射到尾部白名单
        int idx = 0;
        for (int b : blacklist) {
            if (b < whitelistSize) {
                blackToWhite[b] = whitesInTail[idx++];
            }
        }
    }

    int pick() {
        std::uniform_int_distribution<int> dist(0, whitelistSize - 1);
        int r = dist(rng);

        // 如果是黑名单数字，返回映射的白名单数字
        auto it = blackToWhite.find(r);
        if (it != blackToWhite.end()) {
            return it->second;
        }
        return r;
    }
};

/**
 * 变体1: 带权重的随机选择
 *
 * 给定一个正整数数组 w，其中 w[i] 表示下标 i 的权重。
 * 实现 pickIndex() 方法，按权重随机返回下标。
 *
 * 思路：前缀和 + 二分查找
 * - 构建前缀和数组
 * - 随机一个 [1, sum] 的数
 * - 二分查找第一个 >= 该数的前缀和位置
 */
class WeightedRandom {
private:
    std::vector<int> prefixSum;
    std::mt19937 rng;

public:
    WeightedRandom(std::vector<int>& w) : rng(std::random_device{}()) {
        prefixSum.resize(w.size());
        prefixSum[0] = w[0];
        for (size_t i = 1; i < w.size(); ++i) {
            prefixSum[i] = prefixSum[i - 1] + w[i];
        }
    }

    int pickIndex() {
        std::uniform_int_distribution<int> dist(1, prefixSum.back());
        int target = dist(rng);

        // 二分查找第一个 >= target 的位置
        return std::lower_bound(prefixSum.begin(), prefixSum.end(), target) - prefixSum.begin();
    }
};

/**
 * 变体2: 链表随机节点（蓄水池抽样）
 *
 * 给定一个单链表，随机选择链表的一个节点，返回其值。
 * 要求每个节点被选中的概率相等。
 *
 * 思路：蓄水池抽样 (Reservoir Sampling)
 * - 遍历链表，第 i 个节点以 1/i 的概率替换当前结果
 * - 最终每个节点被选中的概率都是 1/n
 *
 * 证明：第 k 个节点被选中的概率
 * = (1/k) * (k/(k+1)) * ((k+1)/(k+2)) * ... * ((n-1)/n)
 * = 1/n
 */
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

class ReservoirSampling {
private:
    ListNode* head;
    std::mt19937 rng;

public:
    ReservoirSampling(ListNode* head) : head(head), rng(std::random_device{}()) {}

    int getRandom() {
        int result = head->val;
        ListNode* curr = head->next;
        int i = 2;

        while (curr) {
            std::uniform_int_distribution<int> dist(1, i);
            if (dist(rng) == 1) {  // 1/i 的概率
                result = curr->val;
            }
            curr = curr->next;
            ++i;
        }

        return result;
    }

    // 蓄水池抽样：选择 k 个元素
    std::vector<int> getKRandom(int k) {
        std::vector<int> reservoir;
        ListNode* curr = head;

        // 先填满蓄水池
        for (int i = 0; i < k && curr; ++i) {
            reservoir.push_back(curr->val);
            curr = curr->next;
        }

        int i = k + 1;
        while (curr) {
            std::uniform_int_distribution<int> dist(1, i);
            int j = dist(rng);
            if (j <= k) {  // k/i 的概率进入蓄水池
                reservoir[j - 1] = curr->val;
            }
            curr = curr->next;
            ++i;
        }

        return reservoir;
    }
};

/**
 * 变体3: 随机打乱数组 (Fisher-Yates 洗牌算法)
 *
 * 给你一个整数数组 nums，设计算法随机打乱一个没有重复元素的数组。
 * 实现 shuffle() 方法随机打乱数组，reset() 方法恢复原数组。
 *
 * Fisher-Yates 算法：
 * - 从最后一个元素开始，随机选择一个 [0, i] 范围内的元素与其交换
 * - 每种排列的概率都是 1/n!
 */
class ShuffleArray {
private:
    std::vector<int> original;
    std::vector<int> shuffled;
    std::mt19937 rng;

public:
    ShuffleArray(std::vector<int>& nums)
        : original(nums), shuffled(nums), rng(std::random_device{}()) {}

    std::vector<int> reset() {
        shuffled = original;
        return shuffled;
    }

    std::vector<int> shuffle() {
        int n = shuffled.size();
        for (int i = n - 1; i > 0; --i) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(rng);
            std::swap(shuffled[i], shuffled[j]);
        }
        return shuffled;
    }
};


// ==================== 测试代码 ====================
#include <cassert>
#include <set>

void testRandomizedSet() {
    RandomizedSet randomizedSet;
    assert(randomizedSet.insert(1) == true);
    assert(randomizedSet.remove(2) == false);
    assert(randomizedSet.insert(2) == true);

    // getRandom 应该返回 1 或 2
    std::set<int> results;
    for (int i = 0; i < 100; ++i) {
        results.insert(randomizedSet.getRandom());
    }
    assert(results.count(1) && results.count(2));

    assert(randomizedSet.remove(1) == true);
    assert(randomizedSet.insert(2) == false);
    assert(randomizedSet.getRandom() == 2);

    std::cout << "RandomizedSet: PASSED\n";
}

void testRandomizedCollection() {
    RandomizedCollection collection;
    assert(collection.insert(1) == true);
    assert(collection.insert(1) == false);
    assert(collection.insert(2) == true);

    // getRandom 应该返回 1（概率 2/3）或 2（概率 1/3）
    int count1 = 0, count2 = 0;
    for (int i = 0; i < 3000; ++i) {
        int r = collection.getRandom();
        if (r == 1) ++count1;
        else if (r == 2) ++count2;
    }
    // 粗略检查比例（允许误差）
    assert(count1 > count2);

    assert(collection.remove(1) == true);

    std::cout << "RandomizedCollection: PASSED\n";
}

void testBlacklistRandom() {
    std::vector<int> blacklist = {2, 3, 5};
    Solution solution(7, blacklist);

    std::set<int> results;
    for (int i = 0; i < 100; ++i) {
        int r = solution.pick();
        results.insert(r);
        // 确保不在黑名单中
        assert(r != 2 && r != 3 && r != 5);
    }
    // 应该能覆盖所有白名单数字 [0, 1, 4, 6]
    assert(results.size() == 4);

    std::cout << "BlacklistRandom: PASSED\n";
}

void testWeightedRandom() {
    std::vector<int> weights = {1, 3};
    WeightedRandom wr(weights);

    int count0 = 0, count1 = 0;
    for (int i = 0; i < 4000; ++i) {
        if (wr.pickIndex() == 0) ++count0;
        else ++count1;
    }
    // index 1 的概率应该是 index 0 的 3 倍
    assert(count1 > count0 * 2);

    std::cout << "WeightedRandom: PASSED\n";
}

void testReservoirSampling() {
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);

    ReservoirSampling rs(head);

    std::set<int> results;
    for (int i = 0; i < 100; ++i) {
        results.insert(rs.getRandom());
    }
    // 应该能覆盖所有节点值
    assert(results.size() == 3);

    // 清理
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }

    std::cout << "ReservoirSampling: PASSED\n";
}

void testShuffleArray() {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    ShuffleArray sa(nums);

    // 验证 reset
    auto reset = sa.reset();
    assert(reset == nums);

    // 验证 shuffle 产生不同结果
    bool different = false;
    for (int i = 0; i < 10; ++i) {
        auto shuffled = sa.shuffle();
        if (shuffled != nums) {
            different = true;
            break;
        }
    }
    assert(different);

    std::cout << "ShuffleArray: PASSED\n";
}

int main() {
    std::cout << "=== Random Set Solution Tests ===\n";

    testRandomizedSet();
    testRandomizedCollection();
    testBlacklistRandom();
    testWeightedRandom();
    testReservoirSampling();
    testShuffleArray();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
