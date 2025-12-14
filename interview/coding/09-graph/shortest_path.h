/**
 * @file shortest_path.h
 * @brief 最短路径算法 - 题目说明与声明
 *
 * 包含题目：
 * 1. Dijkstra 单源最短路径
 * 2. 网络延迟时间 (LeetCode 743)
 * 3. 二进制矩阵中的最短路径 (LeetCode 1091)
 * 4. 迷宫中离入口最近的出口 (LeetCode 1926)
 * 5. 最小体力消耗路径 (LeetCode 1631)
 * 6. K 站中转内最便宜的航班 (LeetCode 787)
 */

#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include <vector>

namespace ShortestPath {

// ==================== 题目声明 ====================

/**
 * 题目1: Dijkstra 单源最短路径
 *
 * 给定一个加权有向图，找出从源点到所有其他点的最短路径。
 *
 * 思路: 优先队列 + 贪心，每次取距离最小的节点扩展
 * 时间复杂度: O((V + E) log V)
 * 空间复杂度: O(V + E)
 *
 * 示例:
 *   输入: n = 4, edges = [[0,1,1],[0,2,4],[1,2,2],[1,3,6],[2,3,3]], src = 0
 *   输出: [0, 1, 3, 6]（从0到各点的最短距离）
 */
std::vector<int> dijkstra(int n, std::vector<std::vector<int>>& edges, int src);
std::vector<int> dijkstraSolution(int n, std::vector<std::vector<int>>& edges, int src);

/**
 * 题目2: 网络延迟时间
 * LeetCode 743
 *
 * 有 n 个网络节点，标记为 1 到 n。给你一个列表 times，表示信号经过有向边的传递时间。
 * times[i] = (ui, vi, wi)，其中 ui 是源节点，vi 是目标节点，wi 是传递时间。
 * 从某个节点 K 发出一个信号。需要多久才能使所有节点都收到信号？无法全部收到返回 -1。
 *
 * 思路: Dijkstra 求最短路，答案是到所有节点最短距离的最大值
 * 时间复杂度: O((V + E) log V)
 * 空间复杂度: O(V + E)
 *
 * 示例:
 *   输入: times = [[2,1,1],[2,3,1],[3,4,1]], n = 4, k = 2
 *   输出: 2
 */
int networkDelayTime(std::vector<std::vector<int>>& times, int n, int k);
int networkDelayTimeSolution(std::vector<std::vector<int>>& times, int n, int k);

/**
 * 题目3: 二进制矩阵中的最短路径
 * LeetCode 1091
 *
 * 给你一个 n x n 的二进制矩阵 grid 中，返回矩阵中最短畅通路径的长度。
 * 如果不存在这样的路径，返回 -1。
 * 畅通路径是从左上角 (0, 0) 到右下角 (n-1, n-1)，可以向8个方向移动。
 *
 * 思路: BFS，8方向搜索
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n^2)
 *
 * 示例:
 *   输入: grid = [[0,1],[1,0]]
 *   输出: 2
 */
int shortestPathBinaryMatrix(std::vector<std::vector<int>>& grid);
int shortestPathBinaryMatrixSolution(std::vector<std::vector<int>>& grid);

/**
 * 题目4: 迷宫中离入口最近的出口
 * LeetCode 1926
 *
 * 给你一个 m x n 的迷宫矩阵 maze（'.' 表示空格子，'+' 表示墙）和迷宫的入口 entrance。
 * 找到离入口最近的出口，返回最短路径的步数。出口是位于边界上的空格子。
 *
 * 思路: BFS 从入口开始，找到第一个边界上的空格
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m * n)
 *
 * 示例:
 *   输入: maze = [["+","+",".","+"],[".",".",".","+"],["+","+","+","."]], entrance = [1,2]
 *   输出: 1
 */
int nearestExit(std::vector<std::vector<char>>& maze, std::vector<int>& entrance);
int nearestExitSolution(std::vector<std::vector<char>>& maze, std::vector<int>& entrance);

/**
 * 题目5: 最小体力消耗路径
 * LeetCode 1631
 *
 * 给你一个二维整数数组 heights，其中 heights[row][col] 表示格子 (row, col) 的高度。
 * 从左上角走到右下角，一条路径的体力消耗值是路径上相邻格子之间高度差绝对值的最大值。
 * 返回从左上角走到右下角的最小体力消耗值。
 *
 * 思路1: 二分答案 + BFS/DFS 检验
 * 思路2: Dijkstra 变种，边权为高度差
 * 时间复杂度: O(m * n * log(max_height))
 * 空间复杂度: O(m * n)
 *
 * 示例:
 *   输入: heights = [[1,2,2],[3,8,2],[5,3,5]]
 *   输出: 2
 */
int minimumEffortPath(std::vector<std::vector<int>>& heights);
int minimumEffortPathSolution(std::vector<std::vector<int>>& heights);

/**
 * 题目6: K 站中转内最便宜的航班
 * LeetCode 787
 *
 * 有 n 个城市通过 flights 连接。flights[i] = [fromi, toi, pricei] 表示航班。
 * 找出从 src 到 dst 最多经过 k 站中转的最便宜价格，没有则返回 -1。
 *
 * 思路1: Bellman-Ford 变种，最多松弛 k+1 次
 * 思路2: BFS + 剪枝
 * 时间复杂度: O(k * E)
 * 空间复杂度: O(V)
 *
 * 示例:
 *   输入: n = 4, flights = [[0,1,100],[1,2,100],[2,0,100],[1,3,600],[2,3,200]],
 *         src = 0, dst = 3, k = 1
 *   输出: 700
 */
int findCheapestPrice(int n, std::vector<std::vector<int>>& flights, int src, int dst, int k);
int findCheapestPriceSolution(int n, std::vector<std::vector<int>>& flights, int src, int dst, int k);

// ==================== 测试函数声明 ====================

void testShortestPath();          // 测试面试者实现
void testShortestPathSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testShortestPath();
    testShortestPathSolution();
}

} // namespace ShortestPath

#endif // SHORTEST_PATH_H
