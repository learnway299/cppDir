/**
 * @file topological_sort_solution.cpp
 * @brief 拓扑排序 - 解答
 */

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

/**
 * 题目1：课程表 (LeetCode 207)
 *
 * 思路1：BFS（Kahn算法）- 入度为0的节点入队
 * 思路2：DFS - 检测是否有环
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V + E)
 */
class CanFinish {
public:
    // BFS Kahn算法
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<int> indegree(numCourses, 0);
        vector<vector<int>> adj(numCourses);

        // 建图
        for (auto& pre : prerequisites) {
            adj[pre[1]].push_back(pre[0]);
            indegree[pre[0]]++;
        }

        // 入度为0的节点入队
        queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }

        int count = 0;
        while (!q.empty()) {
            int course = q.front();
            q.pop();
            ++count;

            for (int next : adj[course]) {
                if (--indegree[next] == 0) {
                    q.push(next);
                }
            }
        }

        return count == numCourses;
    }

    // DFS 检测环
    bool canFinishDFS(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        for (auto& pre : prerequisites) {
            adj[pre[1]].push_back(pre[0]);
        }

        // 0: 未访问, 1: 正在访问, 2: 已完成
        vector<int> state(numCourses, 0);

        for (int i = 0; i < numCourses; ++i) {
            if (hasCycle(adj, i, state)) {
                return false;
            }
        }
        return true;
    }

private:
    bool hasCycle(vector<vector<int>>& adj, int node, vector<int>& state) {
        if (state[node] == 1) return true;   // 发现环
        if (state[node] == 2) return false;  // 已处理

        state[node] = 1;  // 正在访问

        for (int next : adj[node]) {
            if (hasCycle(adj, next, state)) {
                return true;
            }
        }

        state[node] = 2;  // 完成访问
        return false;
    }
};

/**
 * 题目2：课程表 II (LeetCode 210)
 *
 * 思路：拓扑排序，记录排序结果
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V + E)
 */
class FindOrder {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        vector<int> indegree(numCourses, 0);
        vector<vector<int>> adj(numCourses);

        for (auto& pre : prerequisites) {
            adj[pre[1]].push_back(pre[0]);
            indegree[pre[0]]++;
        }

        queue<int> q;
        for (int i = 0; i < numCourses; ++i) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }

        vector<int> order;
        while (!q.empty()) {
            int course = q.front();
            q.pop();
            order.push_back(course);

            for (int next : adj[course]) {
                if (--indegree[next] == 0) {
                    q.push(next);
                }
            }
        }

        if (order.size() != numCourses) {
            return {};  // 存在环
        }
        return order;
    }

    // DFS 版本（后序遍历逆序）
    vector<int> findOrderDFS(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        for (auto& pre : prerequisites) {
            adj[pre[1]].push_back(pre[0]);
        }

        vector<int> state(numCourses, 0);
        vector<int> order;

        for (int i = 0; i < numCourses; ++i) {
            if (!dfs(adj, i, state, order)) {
                return {};
            }
        }

        reverse(order.begin(), order.end());
        return order;
    }

private:
    bool dfs(vector<vector<int>>& adj, int node,
             vector<int>& state, vector<int>& order) {
        if (state[node] == 1) return false;  // 环
        if (state[node] == 2) return true;

        state[node] = 1;
        for (int next : adj[node]) {
            if (!dfs(adj, next, state, order)) {
                return false;
            }
        }
        state[node] = 2;
        order.push_back(node);
        return true;
    }
};

/**
 * 题目3：课程表 IV (LeetCode 1462)
 *
 * 思路：Floyd-Warshall 或 BFS 预处理可达性
 * 时间复杂度：O(n^3) 或 O(n * (V + E))
 * 空间复杂度：O(n^2)
 */
class CheckIfPrerequisite {
public:
    vector<bool> checkIfPrerequisite(int numCourses,
                                      vector<vector<int>>& prerequisites,
                                      vector<vector<int>>& queries) {
        // 可达性矩阵
        vector<vector<bool>> reachable(numCourses,
                                        vector<bool>(numCourses, false));

        // 建图
        vector<vector<int>> adj(numCourses);
        for (auto& pre : prerequisites) {
            adj[pre[0]].push_back(pre[1]);
            reachable[pre[0]][pre[1]] = true;
        }

        // Floyd-Warshall 传递闭包
        for (int k = 0; k < numCourses; ++k) {
            for (int i = 0; i < numCourses; ++i) {
                for (int j = 0; j < numCourses; ++j) {
                    reachable[i][j] = reachable[i][j] ||
                                      (reachable[i][k] && reachable[k][j]);
                }
            }
        }

        vector<bool> result;
        for (auto& q : queries) {
            result.push_back(reachable[q[0]][q[1]]);
        }
        return result;
    }

    // BFS 版本
    vector<bool> checkIfPrerequisiteBFS(int numCourses,
                                         vector<vector<int>>& prerequisites,
                                         vector<vector<int>>& queries) {
        vector<vector<int>> adj(numCourses);
        for (auto& pre : prerequisites) {
            adj[pre[0]].push_back(pre[1]);
        }

        // 对每个节点 BFS 找所有可达节点
        vector<unordered_set<int>> reachable(numCourses);
        for (int i = 0; i < numCourses; ++i) {
            queue<int> q;
            q.push(i);
            while (!q.empty()) {
                int node = q.front();
                q.pop();
                for (int next : adj[node]) {
                    if (reachable[i].insert(next).second) {
                        q.push(next);
                    }
                }
            }
        }

        vector<bool> result;
        for (auto& q : queries) {
            result.push_back(reachable[q[0]].count(q[1]) > 0);
        }
        return result;
    }
};

/**
 * 题目4：外星文字典 (LeetCode 269)
 *
 * 思路：从相邻单词推导字母顺序，建图后拓扑排序
 * 时间复杂度：O(C)，C 是所有字符总数
 * 空间复杂度：O(1) 或 O(U)，U 是唯一字符数
 */
class AlienOrder {
public:
    string alienOrder(vector<string>& words) {
        // 初始化所有字符
        unordered_map<char, int> indegree;
        unordered_map<char, unordered_set<char>> adj;

        for (auto& word : words) {
            for (char c : word) {
                indegree[c] = 0;
            }
        }

        // 建图：比较相邻单词
        for (int i = 0; i < words.size() - 1; ++i) {
            string& w1 = words[i];
            string& w2 = words[i + 1];

            // 检查非法情况：前缀但 w1 更长
            if (w1.length() > w2.length() &&
                w1.substr(0, w2.length()) == w2) {
                return "";
            }

            // 找第一个不同的字符
            for (int j = 0; j < min(w1.length(), w2.length()); ++j) {
                if (w1[j] != w2[j]) {
                    if (adj[w1[j]].insert(w2[j]).second) {
                        indegree[w2[j]]++;
                    }
                    break;
                }
            }
        }

        // 拓扑排序
        queue<char> q;
        for (auto& [c, deg] : indegree) {
            if (deg == 0) {
                q.push(c);
            }
        }

        string result;
        while (!q.empty()) {
            char c = q.front();
            q.pop();
            result += c;

            for (char next : adj[c]) {
                if (--indegree[next] == 0) {
                    q.push(next);
                }
            }
        }

        if (result.length() != indegree.size()) {
            return "";  // 存在环
        }
        return result;
    }
};

/**
 * 题目5：并行课程 (LeetCode 1136)
 *
 * 思路：拓扑排序按层处理，层数即为学期数
 * 时间复杂度：O(V + E)
 * 空间复杂度：O(V + E)
 */
class MinimumSemesters {
public:
    int minimumSemesters(int n, vector<vector<int>>& relations) {
        vector<int> indegree(n + 1, 0);
        vector<vector<int>> adj(n + 1);

        for (auto& rel : relations) {
            adj[rel[0]].push_back(rel[1]);
            indegree[rel[1]]++;
        }

        queue<int> q;
        for (int i = 1; i <= n; ++i) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }

        int semesters = 0;
        int count = 0;

        while (!q.empty()) {
            int size = q.size();
            ++semesters;

            for (int i = 0; i < size; ++i) {
                int course = q.front();
                q.pop();
                ++count;

                for (int next : adj[course]) {
                    if (--indegree[next] == 0) {
                        q.push(next);
                    }
                }
            }
        }

        return count == n ? semesters : -1;
    }
};

int main() {
    // 测试课程表
    CanFinish cf;
    vector<vector<int>> pre1 = {{1, 0}};
    // cf.canFinish(2, pre1) -> true

    vector<vector<int>> pre2 = {{1, 0}, {0, 1}};
    // cf.canFinish(2, pre2) -> false

    // 测试课程表 II
    FindOrder fo;
    vector<vector<int>> pre3 = {{1, 0}, {2, 0}, {3, 1}, {3, 2}};
    // fo.findOrder(4, pre3) -> [0, 1, 2, 3] 或 [0, 2, 1, 3]

    // 测试外星文字典
    AlienOrder ao;
    vector<string> words = {"wrt", "wrf", "er", "ett", "rftt"};
    // ao.alienOrder(words) -> "wertf"

    // 测试并行课程
    MinimumSemesters ms;
    vector<vector<int>> rel = {{1, 3}, {2, 3}};
    // ms.minimumSemesters(3, rel) -> 2

    return 0;
}
