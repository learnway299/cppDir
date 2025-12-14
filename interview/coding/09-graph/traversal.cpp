/**
 * @file traversal.cpp
 * @brief 图的遍历算法 - 面试者实现文件
 */

#include "traversal.h"
#include <queue>
#include <stack>
#include <unordered_map>
#include <iostream>

namespace GraphTraversal {

// ==================== 面试者实现 ====================

std::vector<int> dfs(std::vector<std::vector<int>>& adj, int start) {
    // TODO: 实现 DFS 遍历
    return {};
}

std::vector<int> dfsIterative(std::vector<std::vector<int>>& adj, int start) {
    // TODO: 实现迭代版 DFS
    return {};
}

std::vector<int> bfs(std::vector<std::vector<int>>& adj, int start) {
    // TODO: 实现 BFS 遍历
    return {};
}

std::vector<std::vector<int>> bfsWithLevels(std::vector<std::vector<int>>& adj, int start) {
    // TODO: 实现带层级信息的 BFS
    return {};
}

int numIslands(std::vector<std::vector<char>>& grid) {
    // TODO: 实现岛屿数量统计
    return 0;
}

Node* cloneGraph(Node* node) {
    // TODO: 实现图的克隆
    return nullptr;
}

std::vector<std::vector<int>> allPathsSourceTarget(std::vector<std::vector<int>>& graph) {
    // TODO: 实现找出所有路径
    return {};
}

bool isBipartite(std::vector<std::vector<int>>& graph) {
    // TODO: 实现二分图判断
    return false;
}

// ==================== 测试函数 ====================

void testGraphTraversal() {
    std::cout << "=== Graph Traversal Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace GraphTraversal
