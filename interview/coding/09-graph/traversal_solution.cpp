/**
 * @file traversal_solution.cpp
 * @brief 图的遍历算法 - 参考答案
 */

#include "traversal.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <stack>
#include <unordered_map>

namespace GraphTraversal {

// ==================== DFS 辅助函数 ====================

static void dfsHelper(std::vector<std::vector<int>>& adj, int node,
                      std::vector<bool>& visited, std::vector<int>& result) {
    visited[node] = true;
    result.push_back(node);
    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            dfsHelper(adj, neighbor, visited, result);
        }
    }
}

// ==================== 参考答案实现 ====================

std::vector<int> dfsSolution(std::vector<std::vector<int>>& adj, int start) {
    std::vector<int> result;
    std::vector<bool> visited(adj.size(), false);
    dfsHelper(adj, start, visited, result);
    return result;
}

std::vector<int> dfsIterativeSolution(std::vector<std::vector<int>>& adj, int start) {
    std::vector<int> result;
    std::vector<bool> visited(adj.size(), false);
    std::stack<int> stk;

    stk.push(start);
    while (!stk.empty()) {
        int node = stk.top();
        stk.pop();

        if (visited[node]) continue;
        visited[node] = true;
        result.push_back(node);

        // 逆序压栈以保持顺序
        for (int i = static_cast<int>(adj[node].size()) - 1; i >= 0; --i) {
            if (!visited[adj[node][i]]) {
                stk.push(adj[node][i]);
            }
        }
    }
    return result;
}

std::vector<int> bfsSolution(std::vector<std::vector<int>>& adj, int start) {
    std::vector<int> result;
    std::vector<bool> visited(adj.size(), false);
    std::queue<int> q;

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        result.push_back(node);

        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    return result;
}

std::vector<std::vector<int>> bfsWithLevelsSolution(std::vector<std::vector<int>>& adj, int start) {
    std::vector<std::vector<int>> levels;
    std::vector<bool> visited(adj.size(), false);
    std::queue<int> q;

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int levelSize = static_cast<int>(q.size());
        std::vector<int> currentLevel;

        for (int i = 0; i < levelSize; ++i) {
            int node = q.front();
            q.pop();
            currentLevel.push_back(node);

            for (int neighbor : adj[node]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        levels.push_back(currentLevel);
    }
    return levels;
}

// 岛屿 DFS 辅助函数
static void numIslandsDfs(std::vector<std::vector<char>>& grid, int i, int j) {
    int m = static_cast<int>(grid.size());
    int n = static_cast<int>(grid[0].size());
    if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] != '1') {
        return;
    }

    grid[i][j] = '0';  // 标记为已访问
    numIslandsDfs(grid, i + 1, j);
    numIslandsDfs(grid, i - 1, j);
    numIslandsDfs(grid, i, j + 1);
    numIslandsDfs(grid, i, j - 1);
}

int numIslandsSolution(std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;

    int count = 0;
    int m = static_cast<int>(grid.size());
    int n = static_cast<int>(grid[0].size());

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] == '1') {
                ++count;
                numIslandsDfs(grid, i, j);
            }
        }
    }
    return count;
}

// 克隆图 DFS 辅助函数
static Node* cloneGraphDfs(Node* node, std::unordered_map<Node*, Node*>& visited) {
    if (visited.count(node)) {
        return visited[node];
    }

    Node* clone = new Node(node->val);
    visited[node] = clone;

    for (Node* neighbor : node->neighbors) {
        clone->neighbors.push_back(cloneGraphDfs(neighbor, visited));
    }
    return clone;
}

Node* cloneGraphSolution(Node* node) {
    if (!node) return nullptr;

    std::unordered_map<Node*, Node*> visited;
    return cloneGraphDfs(node, visited);
}

// 所有路径 DFS 辅助函数
static void allPathsDfs(std::vector<std::vector<int>>& graph, int node, int target,
                        std::vector<int>& path, std::vector<std::vector<int>>& result) {
    path.push_back(node);

    if (node == target) {
        result.push_back(path);
    } else {
        for (int next : graph[node]) {
            allPathsDfs(graph, next, target, path, result);
        }
    }

    path.pop_back();
}

std::vector<std::vector<int>> allPathsSourceTargetSolution(std::vector<std::vector<int>>& graph) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    int target = static_cast<int>(graph.size()) - 1;

    allPathsDfs(graph, 0, target, path, result);
    return result;
}

// 二分图 DFS 辅助函数
static bool isBipartiteDfs(std::vector<std::vector<int>>& graph, int node, int c, std::vector<int>& color) {
    color[node] = c;

    for (int neighbor : graph[node]) {
        if (color[neighbor] == c) {
            return false;  // 相邻节点同色
        }
        if (color[neighbor] == 0 && !isBipartiteDfs(graph, neighbor, -c, color)) {
            return false;
        }
    }
    return true;
}

bool isBipartiteSolution(std::vector<std::vector<int>>& graph) {
    int n = static_cast<int>(graph.size());
    std::vector<int> color(n, 0);  // 0: 未染色, 1: 颜色1, -1: 颜色2

    for (int i = 0; i < n; ++i) {
        if (color[i] == 0) {
            if (!isBipartiteDfs(graph, i, 1, color)) {
                return false;
            }
        }
    }
    return true;
}

// ==================== 测试函数 ====================

void testGraphTraversalSolution() {
    std::cout << "=== Graph Traversal Tests (Solution) ===\n";

    // 测试 DFS
    std::vector<std::vector<int>> adj = {{1, 2}, {0, 3}, {0, 3}, {1, 2}};
    auto dfsResult = dfsSolution(adj, 0);
    assert(dfsResult.size() == 4);
    assert(dfsResult[0] == 0);
    std::cout << "  dfsSolution: PASSED\n";

    // 测试 DFS 迭代
    std::vector<std::vector<int>> adj2 = {{1, 2}, {0, 3}, {0, 3}, {1, 2}};
    auto dfsIterResult = dfsIterativeSolution(adj2, 0);
    assert(dfsIterResult.size() == 4);
    std::cout << "  dfsIterativeSolution: PASSED\n";

    // 测试 BFS
    std::vector<std::vector<int>> adj3 = {{1, 2}, {0, 3}, {0, 3}, {1, 2}};
    auto bfsResult = bfsSolution(adj3, 0);
    assert(bfsResult.size() == 4);
    assert(bfsResult[0] == 0);
    std::cout << "  bfsSolution: PASSED\n";

    // 测试 BFS with levels
    std::vector<std::vector<int>> adj4 = {{1, 2}, {0, 3}, {0, 3}, {1, 2}};
    auto levels = bfsWithLevelsSolution(adj4, 0);
    assert(!levels.empty());
    std::cout << "  bfsWithLevelsSolution: PASSED\n";

    // 测试岛屿数量
    std::vector<std::vector<char>> grid = {
        {'1', '1', '0', '0', '0'},
        {'1', '1', '0', '0', '0'},
        {'0', '0', '1', '0', '0'},
        {'0', '0', '0', '1', '1'}
    };
    assert(numIslandsSolution(grid) == 3);
    std::cout << "  numIslandsSolution: PASSED\n";

    // 测试所有路径
    std::vector<std::vector<int>> graph = {{1, 2}, {3}, {3}, {}};
    auto allPaths = allPathsSourceTargetSolution(graph);
    assert(allPaths.size() == 2);
    std::cout << "  allPathsSourceTargetSolution: PASSED\n";

    // 测试二分图
    std::vector<std::vector<int>> biGraph = {{1, 3}, {0, 2}, {1, 3}, {0, 2}};
    assert(isBipartiteSolution(biGraph) == true);
    std::cout << "  isBipartiteSolution: PASSED\n";
}

} // namespace GraphTraversal
