/**
 * @file topological_sort_solution.cpp
 * @brief 拓扑排序相关题目 - 参考答案
 */

#include "topological_sort.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace TopologicalSort {

// ==================== 辅助函数 ====================

// DFS 检测环辅助函数
static bool hasCycle(std::vector<std::vector<int>>& adj, int node, std::vector<int>& state) {
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

// ==================== 参考答案实现 ====================

bool canFinishSolution(int numCourses, std::vector<std::vector<int>>& prerequisites) {
    std::vector<int> indegree(numCourses, 0);
    std::vector<std::vector<int>> adj(numCourses);

    // 建图
    for (auto& pre : prerequisites) {
        adj[pre[1]].push_back(pre[0]);
        indegree[pre[0]]++;
    }

    // 入度为0的节点入队
    std::queue<int> q;
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

std::vector<int> findOrderSolution(int numCourses, std::vector<std::vector<int>>& prerequisites) {
    std::vector<int> indegree(numCourses, 0);
    std::vector<std::vector<int>> adj(numCourses);

    for (auto& pre : prerequisites) {
        adj[pre[1]].push_back(pre[0]);
        indegree[pre[0]]++;
    }

    std::queue<int> q;
    for (int i = 0; i < numCourses; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    std::vector<int> order;
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

    if (order.size() != static_cast<size_t>(numCourses)) {
        return {};  // 存在环
    }
    return order;
}

std::vector<bool> checkIfPrerequisiteSolution(int numCourses,
                                               std::vector<std::vector<int>>& prerequisites,
                                               std::vector<std::vector<int>>& queries) {
    // 可达性矩阵
    std::vector<std::vector<bool>> reachable(numCourses,
                                              std::vector<bool>(numCourses, false));

    // 建图
    for (auto& pre : prerequisites) {
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

    std::vector<bool> result;
    for (auto& q : queries) {
        result.push_back(reachable[q[0]][q[1]]);
    }
    return result;
}

std::string alienOrderSolution(std::vector<std::string>& words) {
    // 初始化所有字符
    std::unordered_map<char, int> indegree;
    std::unordered_map<char, std::unordered_set<char>> adj;

    for (auto& word : words) {
        for (char c : word) {
            indegree[c] = 0;
        }
    }

    // 建图：比较相邻单词
    for (size_t i = 0; i < words.size() - 1; ++i) {
        std::string& w1 = words[i];
        std::string& w2 = words[i + 1];

        // 检查非法情况：前缀但 w1 更长
        if (w1.length() > w2.length() &&
            w1.substr(0, w2.length()) == w2) {
            return "";
        }

        // 找第一个不同的字符
        for (size_t j = 0; j < std::min(w1.length(), w2.length()); ++j) {
            if (w1[j] != w2[j]) {
                if (adj[w1[j]].insert(w2[j]).second) {
                    indegree[w2[j]]++;
                }
                break;
            }
        }
    }

    // 拓扑排序
    std::queue<char> q;
    for (auto& [c, deg] : indegree) {
        if (deg == 0) {
            q.push(c);
        }
    }

    std::string result;
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

int minimumSemestersSolution(int n, std::vector<std::vector<int>>& relations) {
    std::vector<int> indegree(n + 1, 0);
    std::vector<std::vector<int>> adj(n + 1);

    for (auto& rel : relations) {
        adj[rel[0]].push_back(rel[1]);
        indegree[rel[1]]++;
    }

    std::queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    int semesters = 0;
    int count = 0;

    while (!q.empty()) {
        int size = static_cast<int>(q.size());
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

// ==================== 测试函数 ====================

void testTopologicalSortSolution() {
    std::cout << "=== Topological Sort Tests (Solution) ===\n";

    // 测试课程表（无环）
    std::vector<std::vector<int>> pre1 = {{1, 0}};
    assert(canFinishSolution(2, pre1) == true);
    std::cout << "  canFinishSolution (no cycle): PASSED\n";

    // 测试课程表（有环）
    std::vector<std::vector<int>> pre2 = {{1, 0}, {0, 1}};
    assert(canFinishSolution(2, pre2) == false);
    std::cout << "  canFinishSolution (has cycle): PASSED\n";

    // 测试课程表 II
    std::vector<std::vector<int>> pre3 = {{1, 0}, {2, 0}, {3, 1}, {3, 2}};
    auto order = findOrderSolution(4, pre3);
    assert(order.size() == 4);
    assert(order[0] == 0);  // 0 必须是第一个
    std::cout << "  findOrderSolution: PASSED\n";

    // 测试课程表 IV
    std::vector<std::vector<int>> pre4 = {{1, 0}};
    std::vector<std::vector<int>> queries = {{0, 1}, {1, 0}};
    auto prereqResult = checkIfPrerequisiteSolution(2, pre4, queries);
    assert(prereqResult.size() == 2);
    std::cout << "  checkIfPrerequisiteSolution: PASSED\n";

    // 测试外星文字典
    std::vector<std::string> words = {"wrt", "wrf", "er", "ett", "rftt"};
    std::string alienResult = alienOrderSolution(words);
    assert(!alienResult.empty());
    std::cout << "  alienOrderSolution: PASSED\n";

    // 测试并行课程
    std::vector<std::vector<int>> rel = {{1, 3}, {2, 3}};
    assert(minimumSemestersSolution(3, rel) == 2);
    std::cout << "  minimumSemestersSolution: PASSED\n";
}

} // namespace TopologicalSort
