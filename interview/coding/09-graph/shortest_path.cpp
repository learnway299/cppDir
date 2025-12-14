/**
 * @file shortest_path.cpp
 * @brief 最短路径算法 - 面试者实现文件
 */

#include "shortest_path.h"
#include <queue>
#include <climits>
#include <iostream>

namespace ShortestPath {

// ==================== 面试者实现 ====================

std::vector<int> dijkstra(int n, std::vector<std::vector<int>>& edges, int src) {
    // TODO: 实现 Dijkstra 算法
    // 提示: 优先队列 + 贪心，每次取距离最小的节点扩展
    return {};
}

int networkDelayTime(std::vector<std::vector<int>>& times, int n, int k) {
    // TODO: 实现网络延迟时间
    // 提示: Dijkstra 求最短路，答案是到所有节点最短距离的最大值
    return -1;
}

int shortestPathBinaryMatrix(std::vector<std::vector<int>>& grid) {
    // TODO: 实现二进制矩阵最短路径
    // 提示: BFS，8方向搜索
    return -1;
}

int nearestExit(std::vector<std::vector<char>>& maze, std::vector<int>& entrance) {
    // TODO: 实现找最近出口
    // 提示: BFS 从入口开始，找到第一个边界上的空格
    return -1;
}

int minimumEffortPath(std::vector<std::vector<int>>& heights) {
    // TODO: 实现最小体力路径
    // 提示: Dijkstra 变种或二分答案 + BFS
    return 0;
}

int findCheapestPrice(int n, std::vector<std::vector<int>>& flights, int src, int dst, int k) {
    // TODO: 实现 K 站中转最便宜航班
    // 提示: Bellman-Ford 变种，最多松弛 k+1 次
    return -1;
}

// ==================== 测试函数 ====================

void testShortestPath() {
    std::cout << "=== Shortest Path Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace ShortestPath
