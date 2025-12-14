/**
 * @file mst.h
 * @brief 最小生成树 - 题目说明与声明
 *
 * 包含题目：
 * 1. Kruskal 算法
 * 2. Prim 算法
 * 3. 最低成本联通所有城市 (LeetCode 1135)
 * 4. 连接所有点的最小费用 (LeetCode 1584)
 * 5. 水资源分配优化 (LeetCode 1168)
 */

#ifndef MST_H
#define MST_H

#include <vector>

namespace MST {

// ==================== 题目声明 ====================

/**
 * 题目1: Kruskal 算法
 *
 * 给定一个无向加权图，使用 Kruskal 算法求最小生成树的权值和。
 *
 * 思路: 按边权排序，依次选择不成环的最小边
 * 时间复杂度: O(E log E)
 * 空间复杂度: O(V)
 *
 * 示例:
 *   输入: n = 4, edges = [[0,1,1],[0,2,2],[1,2,5],[1,3,1],[2,3,8]]
 *   输出: 4
 */
int minCostKruskal(int n, std::vector<std::vector<int>>& edges);
int minCostKruskalSolution(int n, std::vector<std::vector<int>>& edges);

/**
 * 题目2: Prim 算法
 *
 * 给定一个无向加权图，使用 Prim 算法求最小生成树的权值和。
 *
 * 思路: 从一个点开始，每次选择连接已选点和未选点的最小边
 * 时间复杂度: O(E log V) 或 O(V^2)
 * 空间复杂度: O(V + E)
 *
 * 示例:
 *   输入: n = 4, edges = [[0,1,1],[0,2,2],[1,2,5],[1,3,1],[2,3,8]]
 *   输出: 4
 */
int minCostPrim(int n, std::vector<std::vector<int>>& edges);
int minCostPrimSolution(int n, std::vector<std::vector<int>>& edges);

/**
 * 题目3: 最低成本联通所有城市
 * LeetCode 1135
 *
 * 想象一下你是个城市基建规划者，地图上有 n 座城市，它们按以 1 到 n 的次序编号。
 * 给你整数 n 和一个数组 connections，其中 connections[i] = [xi, yi, costi]
 * 表示将城市 xi 和城市 yi 连接所要的成本。
 * 返回连通所有城市的最低成本，如果不能连通所有城市，返回 -1。
 *
 * 思路: Kruskal 算法，注意城市编号从 1 开始
 * 时间复杂度: O(E log E)
 * 空间复杂度: O(V)
 *
 * 示例:
 *   输入: n = 3, connections = [[1,2,5],[1,3,6],[2,3,1]]
 *   输出: 6
 */
int minimumCost(int n, std::vector<std::vector<int>>& connections);
int minimumCostSolution(int n, std::vector<std::vector<int>>& connections);

/**
 * 题目4: 连接所有点的最小费用
 * LeetCode 1584
 *
 * 给你一个 points 数组，表示 2D 平面上的一些点，其中 points[i] = [xi, yi]。
 * 连接点 [xi, yi] 和点 [xj, yj] 的费用为它们之间的曼哈顿距离：|xi - xj| + |yi - yj|。
 * 返回将所有点连接的最小总费用。
 *
 * 思路1: Kruskal - 枚举所有边，O(n^2 log n)
 * 思路2: Prim - 适合稠密图，O(n^2)
 *
 * 空间复杂度: O(n^2) 或 O(n)
 *
 * 示例:
 *   输入: points = [[0,0],[2,2],[3,10],[5,2],[7,0]]
 *   输出: 20
 */
int minCostConnectPoints(std::vector<std::vector<int>>& points);
int minCostConnectPointsSolution(std::vector<std::vector<int>>& points);

/**
 * 题目5: 水资源分配优化
 * LeetCode 1168
 *
 * 村里面一共有 n 栋房子。我们希望通过建造水井和铺设管道来为所有房子供水。
 * 对于每个房子 i，我们可以花费 wells[i-1] 来建造水井，
 * 或者用 pipes[j] = [house1j, house2j, costj] 的成本铺设管道。
 * 请你帮忙计算为所有房子都供水的最低总成本。
 *
 * 思路: 引入虚拟节点 0，建井等价于连接到节点 0
 * 时间复杂度: O((n + E) log (n + E))
 * 空间复杂度: O(n + E)
 *
 * 示例:
 *   输入: n = 3, wells = [1,2,2], pipes = [[1,2,1],[2,3,1]]
 *   输出: 3
 */
int minCostToSupplyWater(int n, std::vector<int>& wells, std::vector<std::vector<int>>& pipes);
int minCostToSupplyWaterSolution(int n, std::vector<int>& wells, std::vector<std::vector<int>>& pipes);

// ==================== 测试函数声明 ====================

void testMST();          // 测试面试者实现
void testMSTSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMST();
    testMSTSolution();
}

} // namespace MST

#endif // MST_H
