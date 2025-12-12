/**
 * @file traversal_solution.cpp
 * @brief 图的遍历算法 - 解答
 */

#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>

using namespace std;

/**
 * 题目1：DFS 深度优先遍历
 *
 * 思路：使用递归或栈，标记已访问节点
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V)
 */
class DFSTraversal {
public:
    vector<int> dfs(vector<vector<int>>& adj, int start) {
        vector<int> result;
        vector<bool> visited(adj.size(), false);
        dfsHelper(adj, start, visited, result);
        return result;
    }

private:
    void dfsHelper(vector<vector<int>>& adj, int node,
                   vector<bool>& visited, vector<int>& result) {
        visited[node] = true;
        result.push_back(node);
        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                dfsHelper(adj, neighbor, visited, result);
            }
        }
    }

public:
    // 迭代版本（使用栈）
    vector<int> dfsIterative(vector<vector<int>>& adj, int start) {
        vector<int> result;
        vector<bool> visited(adj.size(), false);
        stack<int> stk;

        stk.push(start);
        while (!stk.empty()) {
            int node = stk.top();
            stk.pop();

            if (visited[node]) continue;
            visited[node] = true;
            result.push_back(node);

            // 逆序压栈以保持顺序
            for (int i = adj[node].size() - 1; i >= 0; --i) {
                if (!visited[adj[node][i]]) {
                    stk.push(adj[node][i]);
                }
            }
        }
        return result;
    }
};

/**
 * 题目2：BFS 广度优先遍历
 *
 * 思路：使用队列，按层遍历
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V)
 */
class BFSTraversal {
public:
    vector<int> bfs(vector<vector<int>>& adj, int start) {
        vector<int> result;
        vector<bool> visited(adj.size(), false);
        queue<int> q;

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

    // 返回层级信息的 BFS
    vector<vector<int>> bfsWithLevels(vector<vector<int>>& adj, int start) {
        vector<vector<int>> levels;
        vector<bool> visited(adj.size(), false);
        queue<int> q;

        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int levelSize = q.size();
            vector<int> currentLevel;

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
};

/**
 * 题目3：岛屿数量 (LeetCode 200)
 *
 * 思路：遍历网格，遇到 '1' 就进行 DFS/BFS 将整个岛屿标记
 * 时间复杂度：O(m * n)
 * 空间复杂度：O(m * n) 最坏情况
 */
class NumIslands {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;

        int count = 0;
        int m = grid.size(), n = grid[0].size();

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == '1') {
                    ++count;
                    dfs(grid, i, j);
                }
            }
        }
        return count;
    }

private:
    void dfs(vector<vector<char>>& grid, int i, int j) {
        int m = grid.size(), n = grid[0].size();
        if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] != '1') {
            return;
        }

        grid[i][j] = '0';  // 标记为已访问
        dfs(grid, i + 1, j);
        dfs(grid, i - 1, j);
        dfs(grid, i, j + 1);
        dfs(grid, i, j - 1);
    }

public:
    // BFS 版本
    int numIslandsBFS(vector<vector<char>>& grid) {
        if (grid.empty()) return 0;

        int count = 0;
        int m = grid.size(), n = grid[0].size();
        int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == '1') {
                    ++count;
                    queue<pair<int, int>> q;
                    q.push({i, j});
                    grid[i][j] = '0';

                    while (!q.empty()) {
                        auto [x, y] = q.front();
                        q.pop();

                        for (auto& dir : dirs) {
                            int nx = x + dir[0], ny = y + dir[1];
                            if (nx >= 0 && nx < m && ny >= 0 && ny < n
                                && grid[nx][ny] == '1') {
                                grid[nx][ny] = '0';
                                q.push({nx, ny});
                            }
                        }
                    }
                }
            }
        }
        return count;
    }
};

/**
 * 题目4：克隆图 (LeetCode 133)
 *
 * 思路：使用哈希表记录原节点到克隆节点的映射，DFS/BFS 遍历
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V)
 */
class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node() : val(0), neighbors(vector<Node*>()) {}
    Node(int _val) : val(_val), neighbors(vector<Node*>()) {}
    Node(int _val, vector<Node*> _neighbors) : val(_val), neighbors(_neighbors) {}
};

class CloneGraph {
public:
    Node* cloneGraph(Node* node) {
        if (!node) return nullptr;

        unordered_map<Node*, Node*> visited;
        return dfs(node, visited);
    }

private:
    Node* dfs(Node* node, unordered_map<Node*, Node*>& visited) {
        if (visited.count(node)) {
            return visited[node];
        }

        Node* clone = new Node(node->val);
        visited[node] = clone;

        for (Node* neighbor : node->neighbors) {
            clone->neighbors.push_back(dfs(neighbor, visited));
        }
        return clone;
    }

public:
    // BFS 版本
    Node* cloneGraphBFS(Node* node) {
        if (!node) return nullptr;

        unordered_map<Node*, Node*> visited;
        queue<Node*> q;

        Node* clone = new Node(node->val);
        visited[node] = clone;
        q.push(node);

        while (!q.empty()) {
            Node* curr = q.front();
            q.pop();

            for (Node* neighbor : curr->neighbors) {
                if (!visited.count(neighbor)) {
                    visited[neighbor] = new Node(neighbor->val);
                    q.push(neighbor);
                }
                visited[curr]->neighbors.push_back(visited[neighbor]);
            }
        }
        return clone;
    }
};

/**
 * 题目5：所有可能的路径 (LeetCode 797)
 *
 * 思路：DFS 回溯，DAG 无需 visited 数组
 * 时间复杂度：O(2^n * n)
 * 空间复杂度：O(n)
 */
class AllPathsSourceTarget {
public:
    vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
        vector<vector<int>> result;
        vector<int> path;
        int target = graph.size() - 1;

        dfs(graph, 0, target, path, result);
        return result;
    }

private:
    void dfs(vector<vector<int>>& graph, int node, int target,
             vector<int>& path, vector<vector<int>>& result) {
        path.push_back(node);

        if (node == target) {
            result.push_back(path);
        } else {
            for (int next : graph[node]) {
                dfs(graph, next, target, path, result);
            }
        }

        path.pop_back();
    }
};

/**
 * 题目6：判断二分图 (LeetCode 785)
 *
 * 思路：染色法，相邻节点必须不同色
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V)
 */
class IsBipartite {
public:
    bool isBipartite(vector<vector<int>>& graph) {
        int n = graph.size();
        vector<int> color(n, 0);  // 0: 未染色, 1: 颜色1, -1: 颜色2

        for (int i = 0; i < n; ++i) {
            if (color[i] == 0) {
                if (!dfs(graph, i, 1, color)) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    bool dfs(vector<vector<int>>& graph, int node, int c, vector<int>& color) {
        color[node] = c;

        for (int neighbor : graph[node]) {
            if (color[neighbor] == c) {
                return false;  // 相邻节点同色
            }
            if (color[neighbor] == 0 && !dfs(graph, neighbor, -c, color)) {
                return false;
            }
        }
        return true;
    }

public:
    // BFS 版本
    bool isBipartiteBFS(vector<vector<int>>& graph) {
        int n = graph.size();
        vector<int> color(n, 0);

        for (int i = 0; i < n; ++i) {
            if (color[i] != 0) continue;

            queue<int> q;
            q.push(i);
            color[i] = 1;

            while (!q.empty()) {
                int node = q.front();
                q.pop();

                for (int neighbor : graph[node]) {
                    if (color[neighbor] == color[node]) {
                        return false;
                    }
                    if (color[neighbor] == 0) {
                        color[neighbor] = -color[node];
                        q.push(neighbor);
                    }
                }
            }
        }
        return true;
    }
};

int main() {
    // 测试 DFS
    DFSTraversal dfs;
    vector<vector<int>> adj = {{1, 2}, {0, 3}, {0, 3}, {1, 2}};
    auto dfsResult = dfs.dfs(adj, 0);
    // 输出: 0 1 3 2

    // 测试 BFS
    BFSTraversal bfs;
    auto bfsResult = bfs.bfs(adj, 0);
    // 输出: 0 1 2 3

    // 测试岛屿数量
    NumIslands islands;
    vector<vector<char>> grid = {
        {'1', '1', '0', '0', '0'},
        {'1', '1', '0', '0', '0'},
        {'0', '0', '1', '0', '0'},
        {'0', '0', '0', '1', '1'}
    };
    // 输出: 3

    // 测试所有路径
    AllPathsSourceTarget paths;
    vector<vector<int>> graph = {{1, 2}, {3}, {3}, {}};
    auto allPaths = paths.allPathsSourceTarget(graph);
    // 输出: [[0,1,3], [0,2,3]]

    // 测试二分图
    IsBipartite bipartite;
    vector<vector<int>> biGraph = {{1, 3}, {0, 2}, {1, 3}, {0, 2}};
    // 输出: true

    return 0;
}
