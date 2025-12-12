/**
 * @file shortest_path_solution.cpp
 * @brief 最短路径算法 - 解答
 */

#include <vector>
#include <queue>
#include <climits>

using namespace std;

/**
 * 题目1：Dijkstra 单源最短路径
 *
 * 思路：优先队列 + 贪心，每次取距离最小的节点扩展
 * 时间复杂度：O((V + E) log V)
 * 空间复杂度：O(V + E)
 */
class Dijkstra {
public:
    vector<int> shortestPath(int n, vector<vector<int>>& edges, int src) {
        vector<vector<pair<int, int>>> adj(n);
        for (auto& e : edges) {
            adj[e[0]].push_back({e[1], e[2]});
        }

        vector<int> dist(n, INT_MAX);
        dist[src] = 0;

        // {距离, 节点}
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
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
};

/**
 * 题目2：网络延迟时间 (LeetCode 743)
 *
 * 思路：Dijkstra 求最短路，答案是到所有节点最短距离的最大值
 * 时间复杂度：O((V + E) log V)
 * 空间复杂度：O(V + E)
 */
class NetworkDelayTime {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<pair<int, int>>> adj(n + 1);
        for (auto& t : times) {
            adj[t[0]].push_back({t[1], t[2]});
        }

        vector<int> dist(n + 1, INT_MAX);
        dist[k] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
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
            result = max(result, dist[i]);
        }
        return result;
    }
};

/**
 * 题目3：二进制矩阵中的最短路径 (LeetCode 1091)
 *
 * 思路：BFS，8方向搜索
 * 时间复杂度：O(n^2)
 * 空间复杂度：O(n^2)
 */
class ShortestPathBinaryMatrix {
public:
    int shortestPathBinaryMatrix(vector<vector<int>>& grid) {
        int n = grid.size();
        if (grid[0][0] == 1 || grid[n-1][n-1] == 1) return -1;

        int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

        queue<pair<int, int>> q;
        q.push({0, 0});
        grid[0][0] = 1;  // 标记访问
        int steps = 1;

        while (!q.empty()) {
            int size = q.size();
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
};

/**
 * 题目4：迷宫中离入口最近的出口 (LeetCode 1926)
 *
 * 思路：BFS 从入口开始，找到第一个边界上的空格
 * 时间复杂度：O(m * n)
 * 空间复杂度：O(m * n)
 */
class NearestExit {
public:
    int nearestExit(vector<vector<char>>& maze, vector<int>& entrance) {
        int m = maze.size(), n = maze[0].size();
        int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

        queue<pair<int, int>> q;
        q.push({entrance[0], entrance[1]});
        maze[entrance[0]][entrance[1]] = '+';

        int steps = 0;
        while (!q.empty()) {
            ++steps;
            int size = q.size();

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
};

/**
 * 题目5：最小体力消耗路径 (LeetCode 1631)
 *
 * 思路1：二分答案 + BFS/DFS 检验
 * 思路2：Dijkstra 变种，边权为高度差
 * 时间复杂度：O(m * n * log(max_height))
 * 空间复杂度：O(m * n)
 */
class MinimumEffortPath {
public:
    // Dijkstra 变种
    int minimumEffortPath(vector<vector<int>>& heights) {
        int m = heights.size(), n = heights[0].size();
        int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

        vector<vector<int>> effort(m, vector<int>(n, INT_MAX));
        effort[0][0] = 0;

        // {体力消耗, x, y}
        priority_queue<tuple<int, int, int>,
                       vector<tuple<int, int, int>>,
                       greater<>> pq;
        pq.push({0, 0, 0});

        while (!pq.empty()) {
            auto [e, x, y] = pq.top();
            pq.pop();

            if (x == m - 1 && y == n - 1) return e;
            if (e > effort[x][y]) continue;

            for (auto& dir : dirs) {
                int nx = x + dir[0], ny = y + dir[1];
                if (nx < 0 || nx >= m || ny < 0 || ny >= n) continue;

                int newEffort = max(e, abs(heights[nx][ny] - heights[x][y]));
                if (newEffort < effort[nx][ny]) {
                    effort[nx][ny] = newEffort;
                    pq.push({newEffort, nx, ny});
                }
            }
        }
        return 0;
    }

    // 二分答案 + BFS
    int minimumEffortPathBinarySearch(vector<vector<int>>& heights) {
        int m = heights.size(), n = heights[0].size();
        int left = 0, right = 1e6;

        while (left < right) {
            int mid = left + (right - left) / 2;
            if (canReach(heights, mid)) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }

private:
    bool canReach(vector<vector<int>>& heights, int maxEffort) {
        int m = heights.size(), n = heights[0].size();
        int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

        vector<vector<bool>> visited(m, vector<bool>(n, false));
        queue<pair<int, int>> q;
        q.push({0, 0});
        visited[0][0] = true;

        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();

            if (x == m - 1 && y == n - 1) return true;

            for (auto& dir : dirs) {
                int nx = x + dir[0], ny = y + dir[1];
                if (nx < 0 || nx >= m || ny < 0 || ny >= n || visited[nx][ny]) continue;

                if (abs(heights[nx][ny] - heights[x][y]) <= maxEffort) {
                    visited[nx][ny] = true;
                    q.push({nx, ny});
                }
            }
        }
        return false;
    }
};

/**
 * 题目6：K 站中转内最便宜的航班 (LeetCode 787)
 *
 * 思路1：Bellman-Ford 变种，最多松弛 k+1 次
 * 思路2：BFS + 剪枝
 * 时间复杂度：O(k * E)
 * 空间复杂度：O(V)
 */
class FindCheapestPrice {
public:
    // Bellman-Ford 变种
    int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        vector<int> dist(n, INT_MAX);
        dist[src] = 0;

        // 最多 k+1 条边
        for (int i = 0; i <= k; ++i) {
            vector<int> temp = dist;  // 防止同一轮多次更新
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

    // BFS + 剪枝
    int findCheapestPriceBFS(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        vector<vector<pair<int, int>>> adj(n);
        for (auto& f : flights) {
            adj[f[0]].push_back({f[1], f[2]});
        }

        vector<int> dist(n, INT_MAX);
        queue<pair<int, int>> q;  // {node, cost}
        q.push({src, 0});
        dist[src] = 0;

        int stops = 0;
        while (!q.empty() && stops <= k) {
            int size = q.size();
            for (int i = 0; i < size; ++i) {
                auto [u, cost] = q.front();
                q.pop();

                for (auto& [v, w] : adj[u]) {
                    if (cost + w < dist[v]) {
                        dist[v] = cost + w;
                        q.push({v, dist[v]});
                    }
                }
            }
            ++stops;
        }

        return dist[dst] == INT_MAX ? -1 : dist[dst];
    }
};

int main() {
    // 测试 Dijkstra
    Dijkstra dij;
    vector<vector<int>> edges = {{0,1,1},{0,2,4},{1,2,2},{1,3,6},{2,3,3}};
    auto dist = dij.shortestPath(4, edges, 0);
    // dist = [0, 1, 3, 6]

    // 测试网络延迟时间
    NetworkDelayTime ndt;
    vector<vector<int>> times = {{2,1,1},{2,3,1},{3,4,1}};
    // ndt.networkDelayTime(times, 4, 2) -> 2

    // 测试最小体力路径
    MinimumEffortPath mep;
    vector<vector<int>> heights = {{1,2,2},{3,8,2},{5,3,5}};
    // mep.minimumEffortPath(heights) -> 2

    // 测试 K 站中转
    FindCheapestPrice fcp;
    vector<vector<int>> flights = {{0,1,100},{1,2,100},{2,0,100},{1,3,600},{2,3,200}};
    // fcp.findCheapestPrice(4, flights, 0, 3, 1) -> 700

    return 0;
}
