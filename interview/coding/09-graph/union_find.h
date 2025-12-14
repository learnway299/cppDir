/**
 * @file union_find.h
 * @brief 并查集 - 题目说明与声明
 *
 * 包含题目：
 * 1. 并查集模板
 * 2. 省份数量 (LeetCode 547)
 * 3. 冗余连接 (LeetCode 684)
 * 4. 账户合并 (LeetCode 721)
 * 5. 等式方程的可满足性 (LeetCode 990)
 * 6. 最长连续序列 (LeetCode 128)
 */

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>
#include <string>

namespace UnionFindProblems {

// ==================== 并查集模板类 ====================

/**
 * 并查集数据结构
 *
 * 支持操作：
 * - find(x): 查找 x 的根节点（带路径压缩）
 * - unite(x, y): 合并 x 和 y 所在的集合（按秩合并）
 * - connected(x, y): 判断 x 和 y 是否在同一集合
 * - getCount(): 获取当前连通分量数
 *
 * 优化技巧：
 * 1. 路径压缩：find 时将节点直接指向根
 * 2. 按秩合并：将小树合并到大树
 *
 * 时间复杂度: 均摊 O(α(n))，α 是阿克曼函数的反函数，近似 O(1)
 */
class UnionFind {
public:
    UnionFind(int n);
    int find(int x);
    void unite(int x, int y);
    bool connected(int x, int y);
    int getCount();

private:
    std::vector<int> parent;
    std::vector<int> rank_;
    int count;
};

// ==================== 题目声明 ====================

/**
 * 题目1: 省份数量
 * LeetCode 547
 *
 * 有 n 个城市，其中一些彼此相连，另一些没有相连。如果城市 a 与城市 b 直接相连，
 * 且城市 b 与城市 c 直接相连，那么城市 a 与城市 c 间接相连。
 * 省份是一组直接或间接相连的城市，组内不含其他没有相连的城市。
 * 给你一个 n x n 的矩阵 isConnected，返回省份的数量。
 *
 * 思路: 并查集合并相连的城市，返回连通分量数
 * 时间复杂度: O(n^2 * α(n))
 * 空间复杂度: O(n)
 *
 * 示例:
 *   输入: isConnected = [[1,1,0],[1,1,0],[0,0,1]]
 *   输出: 2
 */
int findCircleNum(std::vector<std::vector<int>>& isConnected);
int findCircleNumSolution(std::vector<std::vector<int>>& isConnected);

/**
 * 题目2: 冗余连接
 * LeetCode 684
 *
 * 在本问题中，树指的是一个连通且无环的无向图。
 * 给定一个有 n 个节点（节点值从 1 到 n）的树，再添加一条边后会形成环。
 * 返回一条可以删去的边，使得结果图是一个有 n 个节点的树。
 *
 * 思路: 按顺序加边，第一条使图成环的边就是答案
 * 时间复杂度: O(n * α(n))
 * 空间复杂度: O(n)
 *
 * 示例:
 *   输入: edges = [[1,2],[1,3],[2,3]]
 *   输出: [2,3]
 */
std::vector<int> findRedundantConnection(std::vector<std::vector<int>>& edges);
std::vector<int> findRedundantConnectionSolution(std::vector<std::vector<int>>& edges);

/**
 * 题目3: 账户合并
 * LeetCode 721
 *
 * 给定一个列表 accounts，每个元素 accounts[i] 是一个字符串列表，
 * 其中第一个元素是名称，其余元素是该账户的邮箱。
 * 现在需要合并这些账户。如果两个账户有相同的邮箱，则这两个账户属于同一人。
 *
 * 思路:
 * 1. 用邮箱作为并查集的元素
 * 2. 同一账户的邮箱合并到一起
 * 3. 按根节点分组，排序输出
 *
 * 时间复杂度: O(n * k * α(nk))，k 是平均邮箱数
 * 空间复杂度: O(nk)
 *
 * 示例:
 *   输入: accounts = [["John","johnsmith@mail.com","john_newyork@mail.com"],
 *                     ["John","johnsmith@mail.com","john00@mail.com"],
 *                     ["Mary","mary@mail.com"],
 *                     ["John","johnnybravo@mail.com"]]
 *   输出: [["John","john00@mail.com","john_newyork@mail.com","johnsmith@mail.com"],
 *          ["Mary","mary@mail.com"],
 *          ["John","johnnybravo@mail.com"]]
 */
std::vector<std::vector<std::string>> accountsMerge(std::vector<std::vector<std::string>>& accounts);
std::vector<std::vector<std::string>> accountsMergeSolution(std::vector<std::vector<std::string>>& accounts);

/**
 * 题目4: 等式方程的可满足性
 * LeetCode 990
 *
 * 给定一个由表示变量之间关系的字符串方程组成的数组，
 * 每个字符串方程 equations[i] 的长度为 4，并采用两种不同的形式之一："a==b" 或 "a!=b"。
 * 返回 true 当且仅当可以将整数分配给变量名，以便满足所有给定的方程。
 *
 * 思路:
 * 1. 先处理所有 == 方程，将相等的变量合并
 * 2. 再处理所有 != 方程，检查是否矛盾
 *
 * 时间复杂度: O(n * α(26))
 * 空间复杂度: O(26) = O(1)
 *
 * 示例:
 *   输入: equations = ["a==b","b!=a"]
 *   输出: false
 *
 *   输入: equations = ["a==b","b==c","a==c"]
 *   输出: true
 */
bool equationsPossible(std::vector<std::string>& equations);
bool equationsPossibleSolution(std::vector<std::string>& equations);

/**
 * 题目5: 最长连续序列
 * LeetCode 128
 *
 * 给定一个未排序的整数数组 nums，找出数字连续的最长序列的长度。
 * 要求算法的时间复杂度为 O(n)。
 *
 * 思路1 (并查集): 将相邻的数合并（如果存在），记录每个集合的大小
 * 思路2 (哈希集合): 从序列起点开始计数，更简洁
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 *
 * 示例:
 *   输入: nums = [100,4,200,1,3,2]
 *   输出: 4（最长连续序列是 [1, 2, 3, 4]）
 */
int longestConsecutive(std::vector<int>& nums);
int longestConsecutiveSolution(std::vector<int>& nums);

// ==================== 测试函数声明 ====================

void testUnionFind();          // 测试面试者实现
void testUnionFindSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testUnionFind();
    testUnionFindSolution();
}

} // namespace UnionFindProblems

#endif // UNION_FIND_H
