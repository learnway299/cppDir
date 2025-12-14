/**
 * @file topological_sort.cpp
 * @brief 拓扑排序相关题目 - 面试者实现文件
 */

#include "topological_sort.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace TopologicalSort {

// ==================== 面试者实现 ====================

bool canFinish(int numCourses, std::vector<std::vector<int>>& prerequisites) {
    // TODO: 实现课程表判断
    // 提示: 使用 Kahn 算法（BFS）或 DFS 检测环
    return false;
}

std::vector<int> findOrder(int numCourses, std::vector<std::vector<int>>& prerequisites) {
    // TODO: 实现课程表 II，返回学习顺序
    // 提示: Kahn 算法，维护入度数组
    return {};
}

std::vector<bool> checkIfPrerequisite(int numCourses,
                                       std::vector<std::vector<int>>& prerequisites,
                                       std::vector<std::vector<int>>& queries) {
    // TODO: 实现课程表 IV，判断先决条件查询
    // 提示: 传递闭包或多源 BFS
    return {};
}

std::string alienOrder(std::vector<std::string>& words) {
    // TODO: 实现火星词典
    // 提示: 构建字符依赖图，拓扑排序
    return "";
}

int minimumSemesters(int n, std::vector<std::vector<int>>& relations) {
    // TODO: 实现并行课程
    // 提示: BFS 层序遍历，层数即为学期数
    return -1;
}

// ==================== 测试函数 ====================

void testTopologicalSort() {
    std::cout << "=== Topological Sort Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace TopologicalSort
