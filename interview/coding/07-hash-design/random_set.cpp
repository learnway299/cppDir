/**
 * @file random_set.cpp
 * @brief O(1) 随机集合实现 - 题目文件
 *
 * 设计支持 O(1) 插入、删除和随机获取的数据结构。
 * 核心思想：哈希表 + 动态数组
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>

/**
 * 题目1: O(1) 时间插入、删除和获取随机元素
 *
 * 实现 RandomizedSet 类：
 *   - RandomizedSet() 初始化对象
 *   - bool insert(int val) 当元素 val 不存在时，向集合中插入该项，并返回 true；
 *     否则，返回 false
 *   - bool remove(int val) 当元素 val 存在时，从集合中移除该项，并返回 true；
 *     否则，返回 false
 *   - int getRandom() 随机返回现有集合中的一项（保证调用此方法时集合中至少存在一个元素）。
 *     每个元素应该有相同的概率被返回
 *
 * 你必须实现类的所有函数，并满足每个函数的平均时间复杂度为 O(1)。
 *
 * 示例:
 *   RandomizedSet randomizedSet = new RandomizedSet();
 *   randomizedSet.insert(1);     // 返回 true
 *   randomizedSet.remove(2);     // 返回 false
 *   randomizedSet.insert(2);     // 返回 true
 *   randomizedSet.getRandom();   // 返回 1 或 2
 *   randomizedSet.remove(1);     // 返回 true
 *   randomizedSet.insert(2);     // 返回 false
 *   randomizedSet.getRandom();   // 返回 2
 *
 * 思路:
 *   - 数组：存储元素，支持 O(1) 随机访问
 *   - 哈希表：val -> index，支持 O(1) 查找
 *   - 删除时，将待删除元素与最后一个元素交换，然后删除最后一个
 */
class RandomizedSet {
public:
    RandomizedSet() {
        // TODO: 初始化
    }

    bool insert(int val) {
        // TODO: 实现插入
        return false;
    }

    bool remove(int val) {
        // TODO: 实现删除
        return false;
    }

    int getRandom() {
        // TODO: 实现随机获取
        return 0;
    }
};

/**
 * 题目2: O(1) 时间插入、删除和获取随机元素 - 允许重复
 *
 * RandomizedCollection 是一种包含数字集合（可能是重复的）的数据结构。
 * 它应该支持插入和删除特定元素，以及删除随机元素。
 *
 * 实现 RandomizedCollection 类:
 *   - RandomizedCollection() 初始化空的 RandomizedCollection 对象
 *   - bool insert(int val) 将一个 val 项插入到集合中，即使该项已经存在。
 *     如果该项不存在，返回 true；否则返回 false
 *   - bool remove(int val) 如果存在，从集合中移除一个 val 项。
 *     如果该项存在，返回 true；否则返回 false。
 *     注意，如果 val 在集合中出现多次，我们只删除其中一个
 *   - int getRandom() 从现有集合中随机获取一个元素。
 *     每个元素被返回的概率与其在集合中的数量呈正比。
 *
 * 示例:
 *   RandomizedCollection collection = new RandomizedCollection();
 *   collection.insert(1);   // 返回 true
 *   collection.insert(1);   // 返回 false
 *   collection.insert(2);   // 返回 true
 *   collection.getRandom(); // 返回 1 的概率为 2/3，返回 2 的概率为 1/3
 *   collection.remove(1);   // 返回 true
 *   collection.getRandom(); // 返回 1 和 2 的概率均为 1/2
 */
class RandomizedCollection {
public:
    RandomizedCollection() {
        // TODO: 初始化
    }

    bool insert(int val) {
        // TODO: 实现插入
        return false;
    }

    bool remove(int val) {
        // TODO: 实现删除
        return false;
    }

    int getRandom() {
        // TODO: 实现随机获取
        return 0;
    }
};

/**
 * 题目3: 黑名单中的随机数
 *
 * 给定一个整数 n 和一个无重复黑名单整数数组 blacklist。
 * 设计一个算法，从 [0, n - 1] 范围内的任意整数中选取一个未加入黑名单 blacklist 的整数。
 * 任何在上述范围内且不在黑名单 blacklist 中的整数都应该有同等的可能性被返回。
 *
 * 优化你的算法，使它最小化调用随机函数的次数。
 *
 * 实现 Solution 类:
 *   - Solution(int n, int[] blacklist) 初始化对象
 *   - int pick() 返回一个在 [0, n - 1] 范围内的随机整数且该整数不在 blacklist 中
 *
 * 示例:
 *   Solution solution = new Solution(7, [2, 3, 5]);
 *   solution.pick(); // 返回 0, 1, 4, 6 中的任一个，概率相等
 *
 * 思路: 将黑名单数字映射到 [n - blacklist.size(), n) 区间的白名单数字
 */
class Solution {
public:
    Solution(int n, std::vector<int>& blacklist) {
        // TODO: 初始化
    }

    int pick() {
        // TODO: 实现随机选取
        return 0;
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

int main() {
    std::cout << "=== Random Set Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testRandomizedSet();
    // testRandomizedCollection();
    // testBlacklistRandom();

    std::cout << "All tests passed!\n";
    return 0;
}
