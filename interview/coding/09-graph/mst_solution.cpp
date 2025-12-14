/**
 * @file mst_solution.cpp
 * @brief 最小生成树 - 参考答案
 */

#include "mst.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <climits>
#include <cmath>

namespace MST {

// ==================== 并查集辅助类 ====================

class UnionFind {
private:
    std::vector<int> parent, rank_;
    int count;

public:
    UnionFind(int n) : parent(n), rank_(n, 0), count(n) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    bool unite(int x, int y) {
        int rootX = find(x), rootY = find(y);
        if (rootX == rootY) return false;

        if (rank_[rootX] < rank_[rootY]) std::swap(rootX, rootY);
        parent[rootY] = rootX;
        if (rank_[rootX] == rank_[rootY]) rank_[rootX]++;
        --count;
        return true;
    }

    int getCount() { return count; }
};

// ==================== 参考答案实现 ====================

int minCostKruskalSolution(int n, std::vector<std::vector<int>>& edges) {
    // 按权重排序
    std::sort(edges.begin(), edges.end(), [](auto& a, auto& b) {
        return a[2] < b[2];
    });

    UnionFind uf(n);
    int cost = 0;
    int edgesUsed = 0;

    for (auto& e : edges) {
        if (uf.unite(e[0], e[1])) {
            cost += e[2];
            ++edgesUsed;
            if (edgesUsed == n - 1) break;
        }
    }

    return edgesUsed == n - 1 ? cost : -1;
}

int minCostPrimSolution(int n, std::vector<std::vector<int>>& edges) {
    // 建邻接表
    std::vector<std::vector<std::pair<int, int>>> adj(n);
    for (auto& e : edges) {
        adj[e[0]].push_back({e[1], e[2]});
        adj[e[1]].push_back({e[0], e[2]});
    }

    // {权重, 节点}
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    std::vector<bool> inMST(n, false);

    pq.push({0, 0});
    int cost = 0;
    int edgesUsed = 0;

    while (!pq.empty() && edgesUsed < n) {
        auto [w, u] = pq.top();
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;
        cost += w;
        ++edgesUsed;

        for (auto& [v, weight] : adj[u]) {
            if (!inMST[v]) {
                pq.push({weight, v});
            }
        }
    }

    return edgesUsed == n ? cost : -1;
}

int minimumCostSolution(int n, std::vector<std::vector<int>>& connections) {
    std::sort(connections.begin(), connections.end(), [](auto& a, auto& b) {
        return a[2] < b[2];
    });

    UnionFind uf(n + 1);  // 城市从 1 开始
    int cost = 0;
    int edgesUsed = 0;

    for (auto& c : connections) {
        if (uf.unite(c[0], c[1])) {
            cost += c[2];
            ++edgesUsed;
            if (edgesUsed == n - 1) break;
        }
    }

    return edgesUsed == n - 1 ? cost : -1;
}

int minCostConnectPointsSolution(std::vector<std::vector<int>>& points) {
    int n = static_cast<int>(points.size());
    std::vector<int> minDist(n, INT_MAX);
    std::vector<bool> inMST(n, false);

    minDist[0] = 0;
    int cost = 0;

    for (int i = 0; i < n; ++i) {
        // 找最小距离的未加入节点
        int u = -1;
        for (int j = 0; j < n; ++j) {
            if (!inMST[j] && (u == -1 || minDist[j] < minDist[u])) {
                u = j;
            }
        }

        inMST[u] = true;
        cost += minDist[u];

        // 更新其他节点的最小距离
        for (int v = 0; v < n; ++v) {
            if (!inMST[v]) {
                int dist = std::abs(points[u][0] - points[v][0]) +
                           std::abs(points[u][1] - points[v][1]);
                minDist[v] = std::min(minDist[v], dist);
            }
        }
    }

    return cost;
}

int minCostToSupplyWaterSolution(int n, std::vector<int>& wells, std::vector<std::vector<int>>& pipes) {
    // 构建边：虚拟节点 0 到各房子的边（建井成本）
    std::vector<std::tuple<int, int, int>> edges;

    for (int i = 0; i < n; ++i) {
        edges.push_back({wells[i], 0, i + 1});
    }

    for (auto& p : pipes) {
        edges.push_back({p[2], p[0], p[1]});
    }

    // Kruskal
    std::sort(edges.begin(), edges.end());

    UnionFind uf(n + 1);  // 0 是虚拟节点
    int cost = 0;
    int edgesUsed = 0;

    for (auto& [w, u, v] : edges) {
        if (uf.unite(u, v)) {
            cost += w;
            if (++edgesUsed == n) break;
        }
    }

    return cost;
}

// ==================== 测试函数 ====================

void testMSTSolution() {
    std::cout << "=== MST Tests (Solution) ===\n";

    // 测试 Kruskal
    std::vector<std::vector<int>> edges1 = {{0,1,1},{0,2,2},{1,2,5},{1,3,1},{2,3,8}};
    assert(minCostKruskalSolution(4, edges1) == 4);
    std::cout << "  minCostKruskalSolution: PASSED\n";

    // 测试 Prim
    std::vector<std::vector<int>> edges2 = {{0,1,1},{0,2,2},{1,2,5},{1,3,1},{2,3,8}};
    assert(minCostPrimSolution(4, edges2) == 4);
    std::cout << "  minCostPrimSolution: PASSED\n";

    // 测试最低成本联通
    std::vector<std::vector<int>> conn = {{1,2,5},{1,3,6},{2,3,1}};
    assert(minimumCostSolution(3, conn) == 6);
    std::cout << "  minimumCostSolution: PASSED\n";

    // 测试连接所有点
    std::vector<std::vector<int>> points = {{0,0},{2,2},{3,10},{5,2},{7,0}};
    assert(minCostConnectPointsSolution(points) == 20);
    std::cout << "  minCostConnectPointsSolution: PASSED\n";

    // 测试水资源分配
    std::vector<int> wells = {1, 2, 2};
    std::vector<std::vector<int>> pipes = {{1,2,1},{2,3,1}};
    assert(minCostToSupplyWaterSolution(3, wells, pipes) == 3);
    std::cout << "  minCostToSupplyWaterSolution: PASSED\n";
}

} // namespace MST
