/**
 * @file mst.cpp
 * @brief 最小生成树 - 面试者实现文件
 */

#include "mst.h"
#include <queue>
#include <algorithm>
#include <iostream>

namespace MST {

// ==================== 面试者实现 ====================

int minCostKruskal(int n, std::vector<std::vector<int>>& edges) {
    // TODO: 实现 Kruskal 算法
    // 提示: 按边权排序，依次选择不成环的最小边（需要并查集）
    return 0;
}

int minCostPrim(int n, std::vector<std::vector<int>>& edges) {
    // TODO: 实现 Prim 算法
    // 提示: 从一个点开始，每次选择连接已选点和未选点的最小边
    return 0;
}

int minimumCost(int n, std::vector<std::vector<int>>& connections) {
    // TODO: 实现最低成本联通
    // 提示: Kruskal 算法，注意城市编号从 1 开始
    return -1;
}

int minCostConnectPoints(std::vector<std::vector<int>>& points) {
    // TODO: 实现连接所有点的最小费用
    // 提示: 可用 Prim 或 Kruskal，曼哈顿距离作为边权
    return 0;
}

int minCostToSupplyWater(int n, std::vector<int>& wells, std::vector<std::vector<int>>& pipes) {
    // TODO: 实现水资源分配优化
    // 提示: 引入虚拟节点 0，建井等价于连接到节点 0
    return 0;
}

// ==================== 测试函数 ====================

void testMST() {
    std::cout << "=== MST Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace MST
