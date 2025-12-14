/**
 * @file traversal.h
 * @brief 图的遍历算法 - 题目说明与声明
 *
 * 包含题目：
 * 1. DFS 深度优先遍历（邻接表）
 * 2. BFS 广度优先遍历（邻接表）
 * 3. 岛屿数量 (LeetCode 200)
 * 4. 克隆图 (LeetCode 133)
 * 5. 所有路径 (LeetCode 797)
 * 6. 判断二分图 (LeetCode 785)
 */

#ifndef GRAPH_TRAVERSAL_H
#define GRAPH_TRAVERSAL_H

#include <vector>
#include <string>

namespace GraphTraversal {

// ==================== 图节点定义 ====================

struct Node {
    int val;
    std::vector<Node*> neighbors;
    Node() : val(0), neighbors(std::vector<Node*>()) {}
    Node(int _val) : val(_val), neighbors(std::vector<Node*>()) {}
    Node(int _val, std::vector<Node*> _neighbors) : val(_val), neighbors(_neighbors) {}
};

// ==================== 题目声明 ====================

/**
 * 题目1: DFS 深度优先遍历
 *
 * 给定一个图的邻接表表示，从指定节点开始进行深度优先遍历
 * 返回遍历顺序
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 *
 * 示例:
 *   输入: adj = [[1,2], [0,3], [0,3], [1,2]], start = 0
 *   输出: [0, 1, 3, 2] 或其他有效的 DFS 序列
 */
std::vector<int> dfs(std::vector<std::vector<int>>& adj, int start);
std::vector<int> dfsSolution(std::vector<std::vector<int>>& adj, int start);

// 迭代版本（使用栈）
std::vector<int> dfsIterative(std::vector<std::vector<int>>& adj, int start);
std::vector<int> dfsIterativeSolution(std::vector<std::vector<int>>& adj, int start);

/**
 * 题目2: BFS 广度优先遍历
 *
 * 给定一个图的邻接表表示，从指定节点开始进行广度优先遍历
 * 返回遍历顺序
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 *
 * 示例:
 *   输入: adj = [[1,2], [0,3], [0,3], [1,2]], start = 0
 *   输出: [0, 1, 2, 3]
 */
std::vector<int> bfs(std::vector<std::vector<int>>& adj, int start);
std::vector<int> bfsSolution(std::vector<std::vector<int>>& adj, int start);

// 返回层级信息的 BFS
std::vector<std::vector<int>> bfsWithLevels(std::vector<std::vector<int>>& adj, int start);
std::vector<std::vector<int>> bfsWithLevelsSolution(std::vector<std::vector<int>>& adj, int start);

/**
 * 题目3: 岛屿数量
 * LeetCode 200
 *
 * 给你一个由 '1'（陆地）和 '0'（水）组成的二维网格，
 * 请你计算网格中岛屿的数量。
 *
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m * n) 最坏情况
 *
 * 示例:
 *   输入: grid = [
 *     ["1","1","0","0","0"],
 *     ["1","1","0","0","0"],
 *     ["0","0","1","0","0"],
 *     ["0","0","0","1","1"]
 *   ]
 *   输出: 3
 */
int numIslands(std::vector<std::vector<char>>& grid);
int numIslandsSolution(std::vector<std::vector<char>>& grid);

/**
 * 题目4: 克隆图
 * LeetCode 133
 *
 * 给你无向连通图中一个节点的引用，请你返回该图的深拷贝（克隆）。
 *
 * 思路: 使用哈希表记录原节点到克隆节点的映射
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 */
Node* cloneGraph(Node* node);
Node* cloneGraphSolution(Node* node);

/**
 * 题目5: 所有可能的路径
 * LeetCode 797
 *
 * 给你一个有 n 个节点的有向无环图（DAG），
 * 请你找出所有从节点 0 到节点 n-1 的路径并输出。
 *
 * 思路: DFS 回溯，DAG 无需 visited 数组
 * 时间复杂度: O(2^n * n)
 * 空间复杂度: O(n)
 *
 * 示例:
 *   输入: graph = [[1,2],[3],[3],[]]
 *   输出: [[0,1,3],[0,2,3]]
 */
std::vector<std::vector<int>> allPathsSourceTarget(std::vector<std::vector<int>>& graph);
std::vector<std::vector<int>> allPathsSourceTargetSolution(std::vector<std::vector<int>>& graph);

/**
 * 题目6: 判断二分图
 * LeetCode 785
 *
 * 如果图是二分图，返回 true；否则，返回 false。
 *
 * 思路: 染色法，相邻节点必须不同色
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 *
 * 示例:
 *   输入: graph = [[1,3],[0,2],[1,3],[0,2]]
 *   输出: true
 */
bool isBipartite(std::vector<std::vector<int>>& graph);
bool isBipartiteSolution(std::vector<std::vector<int>>& graph);

// ==================== 测试函数声明 ====================

void testGraphTraversal();          // 测试面试者实现
void testGraphTraversalSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testGraphTraversal();
    testGraphTraversalSolution();
}

} // namespace GraphTraversal

#endif // GRAPH_TRAVERSAL_H
