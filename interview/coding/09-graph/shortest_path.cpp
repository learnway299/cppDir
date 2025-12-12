/**
 * @file shortest_path.cpp
 * @brief 最短路径算法 - 面试题
 *
 * 包含题目：
 * 1. Dijkstra 单源最短路径
 * 2. 网络延迟时间 (LeetCode 743)
 * 3. 到达目的地的最短距离 (LeetCode 1091)
 * 4. 迷宫中离入口最近的出口 (LeetCode 1926)
 * 5. 最小体力消耗路径 (LeetCode 1631)
 */

#include <vector>
#include <queue>
#include <climits>

using namespace std;

/**
 * 题目1：Dijkstra 单源最短路径
 *
 * 给定一个加权有向图，找出从源点到所有其他点的最短路径。
 *
 * 示例：
 * 输入：n = 4, edges = [[0,1,1],[0,2,4],[1,2,2],[1,3,6],[2,3,3]], src = 0
 * 输出：[0, 1, 3, 6]（从0到各点的最短距离）
 */
class Dijkstra {
public:
    vector<int> shortestPath(int n, vector<vector<int>>& edges, int src) {
        // TODO: 实现 Dijkstra 算法
        return {};
    }
};

/**
 * 题目2：网络延迟时间 (LeetCode 743)
 *
 * 有 n 个网络节点，标记为 1 到 n。给你一个列表 times，表示信号经过有向边的传递时间。
 * times[i] = (ui, vi, wi)，其中 ui 是源节点，vi 是目标节点，wi 是传递时间。
 * 从某个节点 K 发出一个信号。需要多久才能使所有节点都收到信号？无法全部收到返回 -1。
 *
 * 示例：
 * 输入：times = [[2,1,1],[2,3,1],[3,4,1]], n = 4, k = 2
 * 输出：2
 */
class NetworkDelayTime {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        // TODO: 实现网络延迟时间
        return -1;
    }
};

/**
 * 题目3：二进制矩阵中的最短路径 (LeetCode 1091)
 *
 * 给你一个 n x n 的二进制矩阵 grid 中，返回矩阵中最短畅通路径的长度。
 * 如果不存在这样的路径，返回 -1。
 * 畅通路径是从左上角 (0, 0) 到右下角 (n-1, n-1)，可以向8个方向移动。
 *
 * 示例：
 * 输入：grid = [[0,1],[1,0]]
 * 输出：2
 */
class ShortestPathBinaryMatrix {
public:
    int shortestPathBinaryMatrix(vector<vector<int>>& grid) {
        // TODO: 实现二进制矩阵最短路径
        return -1;
    }
};

/**
 * 题目4：迷宫中离入口最近的出口 (LeetCode 1926)
 *
 * 给你一个 m x n 的迷宫矩阵 maze（'.' 表示空格子，'+' 表示墙）和迷宫的入口 entrance。
 * 找到离入口最近的出口，返回最短路径的步数。出口是位于边界上的空格子。
 *
 * 示例：
 * 输入：maze = [["+","+",".","+"],[".",".",".","+"],["+","+","+","."]], entrance = [1,2]
 * 输出：1
 */
class NearestExit {
public:
    int nearestExit(vector<vector<char>>& maze, vector<int>& entrance) {
        // TODO: 实现找最近出口
        return -1;
    }
};

/**
 * 题目5：最小体力消耗路径 (LeetCode 1631)
 *
 * 给你一个二维整数数组 heights，其中 heights[row][col] 表示格子 (row, col) 的高度。
 * 从左上角走到右下角，一条路径的体力消耗值是路径上相邻格子之间高度差绝对值的最大值。
 * 返回从左上角走到右下角的最小体力消耗值。
 *
 * 示例：
 * 输入：heights = [[1,2,2],[3,8,2],[5,3,5]]
 * 输出：2
 */
class MinimumEffortPath {
public:
    int minimumEffortPath(vector<vector<int>>& heights) {
        // TODO: 实现最小体力路径
        return 0;
    }
};

/**
 * 题目6：K 站中转内最便宜的航班 (LeetCode 787)
 *
 * 有 n 个城市通过 flights 连接。flights[i] = [fromi, toi, pricei] 表示航班。
 * 找出从 src 到 dst 最多经过 k 站中转的最便宜价格，没有则返回 -1。
 *
 * 示例：
 * 输入：n = 4, flights = [[0,1,100],[1,2,100],[2,0,100],[1,3,600],[2,3,200]],
 *       src = 0, dst = 3, k = 1
 * 输出：700
 */
class FindCheapestPrice {
public:
    int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        // TODO: 实现 K 站中转最便宜航班
        return -1;
    }
};

int main() {
    return 0;
}
