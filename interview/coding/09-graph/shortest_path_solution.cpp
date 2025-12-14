/**
 * @file shortest_path_solution.cpp
 * @brief 最短路径算法 - 参考答案
 */

#include "shortest_path.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <climits>
#include <tuple>
#include <cmath>

namespace ShortestPath {

// ==================== 参考答案实现 ====================

std::vector<int> dijkstraSolution(int n, std::vector<std::vector<int>>& edges, int src) {
    std::vector<std::vector<std::pair<int, int>>> adj(n);
    for (auto& e : edges) {
        adj[e[0]].push_back({e[1], e[2]});
    }

    std::vector<int> dist(n, INT_MAX);
    dist[src] = 0;

    // {距离, 节点}
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;  // 已找到更短路径

        for (auto& [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    return dist;
}

int networkDelayTimeSolution(std::vector<std::vector<int>>& times, int n, int k) {
    std::vector<std::vector<std::pair<int, int>>> adj(n + 1);
    for (auto& t : times) {
        adj[t[0]].push_back({t[1], t[2]});
    }

    std::vector<int> dist(n + 1, INT_MAX);
    dist[k] = 0;

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    pq.push({0, k});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;

        for (auto& [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    int result = 0;
    for (int i = 1; i <= n; ++i) {
        if (dist[i] == INT_MAX) return -1;
        result = std::max(result, dist[i]);
    }
    return result;
}

int shortestPathBinaryMatrixSolution(std::vector<std::vector<int>>& grid) {
    int n = static_cast<int>(grid.size());
    if (grid[0][0] == 1 || grid[n-1][n-1] == 1) return -1;

    int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

    std::queue<std::pair<int, int>> q;
    q.push({0, 0});
    grid[0][0] = 1;  // 标记访问
    int steps = 1;

    while (!q.empty()) {
        int size = static_cast<int>(q.size());
        for (int i = 0; i < size; ++i) {
            auto [x, y] = q.front();
            q.pop();

            if (x == n - 1 && y == n - 1) return steps;

            for (auto& dir : dirs) {
                int nx = x + dir[0], ny = y + dir[1];
                if (nx >= 0 && nx < n && ny >= 0 && ny < n && grid[nx][ny] == 0) {
                    grid[nx][ny] = 1;
                    q.push({nx, ny});
                }
            }
        }
        ++steps;
    }
    return -1;
}

int nearestExitSolution(std::vector<std::vector<char>>& maze, std::vector<int>& entrance) {
    int m = static_cast<int>(maze.size());
    int n = static_cast<int>(maze[0].size());
    int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    std::queue<std::pair<int, int>> q;
    q.push({entrance[0], entrance[1]});
    maze[entrance[0]][entrance[1]] = '+';

    int steps = 0;
    while (!q.empty()) {
        ++steps;
        int size = static_cast<int>(q.size());

        for (int i = 0; i < size; ++i) {
            auto [x, y] = q.front();
            q.pop();

            for (auto& dir : dirs) {
                int nx = x + dir[0], ny = y + dir[1];
                if (nx < 0 || nx >= m || ny < 0 || ny >= n || maze[nx][ny] == '+') {
                    continue;
                }

                // 是否为出口（边界上）
                if (nx == 0 || nx == m - 1 || ny == 0 || ny == n - 1) {
                    return steps;
                }

                maze[nx][ny] = '+';
                q.push({nx, ny});
            }
        }
    }
    return -1;
}

int minimumEffortPathSolution(std::vector<std::vector<int>>& heights) {
    int m = static_cast<int>(heights.size());
    int n = static_cast<int>(heights[0].size());
    int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    std::vector<std::vector<int>> effort(m, std::vector<int>(n, INT_MAX));
    effort[0][0] = 0;

    // {体力消耗, x, y}
    std::priority_queue<std::tuple<int, int, int>,
                       std::vector<std::tuple<int, int, int>>,
                       std::greater<>> pq;
    pq.push({0, 0, 0});

    while (!pq.empty()) {
        auto [e, x, y] = pq.top();
        pq.pop();

        if (x == m - 1 && y == n - 1) return e;
        if (e > effort[x][y]) continue;

        for (auto& dir : dirs) {
            int nx = x + dir[0], ny = y + dir[1];
            if (nx < 0 || nx >= m || ny < 0 || ny >= n) continue;

            int newEffort = std::max(e, std::abs(heights[nx][ny] - heights[x][y]));
            if (newEffort < effort[nx][ny]) {
                effort[nx][ny] = newEffort;
                pq.push({newEffort, nx, ny});
            }
        }
    }
    return 0;
}

int findCheapestPriceSolution(int n, std::vector<std::vector<int>>& flights, int src, int dst, int k) {
    std::vector<int> dist(n, INT_MAX);
    dist[src] = 0;

    // 最多 k+1 条边
    for (int i = 0; i <= k; ++i) {
        std::vector<int> temp = dist;  // 防止同一轮多次更新
        for (auto& f : flights) {
            int u = f[0], v = f[1], w = f[2];
            if (dist[u] != INT_MAX && dist[u] + w < temp[v]) {
                temp[v] = dist[u] + w;
            }
        }
        dist = temp;
    }

    return dist[dst] == INT_MAX ? -1 : dist[dst];
}

// ==================== 测试函数 ====================

void testShortestPathSolution() {
    std::cout << "=== Shortest Path Tests (Solution) ===\n";

    // 测试 Dijkstra
    std::vector<std::vector<int>> edges = {{0,1,1},{0,2,4},{1,2,2},{1,3,6},{2,3,3}};
    auto dist = dijkstraSolution(4, edges, 0);
    assert(dist[0] == 0);
    assert(dist[1] == 1);
    assert(dist[2] == 3);
    assert(dist[3] == 6);
    std::cout << "  dijkstraSolution: PASSED\n";

    // 测试网络延迟时间
    std::vector<std::vector<int>> times = {{2,1,1},{2,3,1},{3,4,1}};
    assert(networkDelayTimeSolution(times, 4, 2) == 2);
    std::cout << "  networkDelayTimeSolution: PASSED\n";

    // 测试二进制矩阵最短路径
    std::vector<std::vector<int>> grid = {{0,0,0},{1,1,0},{1,1,0}};
    assert(shortestPathBinaryMatrixSolution(grid) == 4);
    std::cout << "  shortestPathBinaryMatrixSolution: PASSED\n";

    // 测试最小体力路径
    std::vector<std::vector<int>> heights = {{1,2,2},{3,8,2},{5,3,5}};
    assert(minimumEffortPathSolution(heights) == 2);
    std::cout << "  minimumEffortPathSolution: PASSED\n";

    // 测试 K 站中转
    std::vector<std::vector<int>> flights = {{0,1,100},{1,2,100},{2,0,100},{1,3,600},{2,3,200}};
    assert(findCheapestPriceSolution(4, flights, 0, 3, 1) == 700);
    std::cout << "  findCheapestPriceSolution: PASSED\n";
}

} // namespace ShortestPath
