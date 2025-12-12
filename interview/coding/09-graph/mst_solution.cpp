/**
 * @file mst_solution.cpp
 * @brief 最小生成树 - 解答
 */

#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>

using namespace std;

// 并查集
class UnionFind {
private:
    vector<int> parent, rank_;
    int count;

public:
    UnionFind(int n) : parent(n), rank_(n, 0), count(n) {
        iota(parent.begin(), parent.end(), 0);
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

        if (rank_[rootX] < rank_[rootY]) swap(rootX, rootY);
        parent[rootY] = rootX;
        if (rank_[rootX] == rank_[rootY]) rank_[rootX]++;
        --count;
        return true;
    }

    int getCount() { return count; }
};

/**
 * 题目1：Kruskal 算法
 *
 * 思路：按边权排序，依次选择不成环的最小边
 * 时间复杂度：O(E log E)
 * 空间复杂度：O(V)
 */
class Kruskal {
public:
    int minCostKruskal(int n, vector<vector<int>>& edges) {
        // 按权重排序
        sort(edges.begin(), edges.end(), [](auto& a, auto& b) {
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
};

/**
 * 题目2：Prim 算法
 *
 * 思路：从一个点开始，每次选择连接已选点和未选点的最小边
 * 时间复杂度：O(E log V)
 * 空间复杂度：O(V + E)
 */
class Prim {
public:
    int minCostPrim(int n, vector<vector<int>>& edges) {
        // 建邻接表
        vector<vector<pair<int, int>>> adj(n);
        for (auto& e : edges) {
            adj[e[0]].push_back({e[1], e[2]});
            adj[e[1]].push_back({e[0], e[2]});
        }

        // {权重, 节点}
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        vector<bool> inMST(n, false);

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
};

/**
 * 题目3：最低成本联通所有城市 (LeetCode 1135)
 *
 * 思路：Kruskal 算法，注意城市编号从 1 开始
 * 时间复杂度：O(E log E)
 * 空间复杂度：O(V)
 */
class MinimumCost {
public:
    int minimumCost(int n, vector<vector<int>>& connections) {
        sort(connections.begin(), connections.end(), [](auto& a, auto& b) {
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
};

/**
 * 题目4：连接所有点的最小费用 (LeetCode 1584)
 *
 * 思路1：Kruskal - 枚举所有边，O(n^2 log n)
 * 思路2：Prim - 适合稠密图，O(n^2)
 * 空间复杂度：O(n^2) 或 O(n)
 */
class MinCostConnectPoints {
public:
    // Prim 算法（稠密图更优）
    int minCostConnectPoints(vector<vector<int>>& points) {
        int n = points.size();
        vector<int> minDist(n, INT_MAX);
        vector<bool> inMST(n, false);

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
                    int dist = abs(points[u][0] - points[v][0]) +
                               abs(points[u][1] - points[v][1]);
                    minDist[v] = min(minDist[v], dist);
                }
            }
        }

        return cost;
    }

    // Kruskal 算法
    int minCostConnectPointsKruskal(vector<vector<int>>& points) {
        int n = points.size();
        vector<tuple<int, int, int>> edges;  // {dist, u, v}

        // 枚举所有边
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int dist = abs(points[i][0] - points[j][0]) +
                           abs(points[i][1] - points[j][1]);
                edges.push_back({dist, i, j});
            }
        }

        sort(edges.begin(), edges.end());

        UnionFind uf(n);
        int cost = 0;
        int edgesUsed = 0;

        for (auto& [d, u, v] : edges) {
            if (uf.unite(u, v)) {
                cost += d;
                if (++edgesUsed == n - 1) break;
            }
        }

        return cost;
    }
};

/**
 * 题目5：水资源分配优化 (LeetCode 1168)
 *
 * 思路：引入虚拟节点 0，建井等价于连接到节点 0
 * 时间复杂度：O((n + E) log (n + E))
 * 空间复杂度：O(n + E)
 */
class MinCostToSupplyWater {
public:
    int minCostToSupplyWater(int n, vector<int>& wells, vector<vector<int>>& pipes) {
        // 构建边：虚拟节点 0 到各房子的边（建井成本）
        vector<tuple<int, int, int>> edges;

        for (int i = 0; i < n; ++i) {
            edges.push_back({wells[i], 0, i + 1});
        }

        for (auto& p : pipes) {
            edges.push_back({p[2], p[0], p[1]});
        }

        // Kruskal
        sort(edges.begin(), edges.end());

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
};

int main() {
    // 测试 Kruskal
    Kruskal kruskal;
    vector<vector<int>> edges1 = {{0,1,1},{0,2,2},{1,2,5},{1,3,1},{2,3,8}};
    // kruskal.minCostKruskal(4, edges1) -> 4

    // 测试 Prim
    Prim prim;
    // prim.minCostPrim(4, edges1) -> 4

    // 测试最低成本联通
    MinimumCost mc;
    vector<vector<int>> conn = {{1,2,5},{1,3,6},{2,3,1}};
    // mc.minimumCost(3, conn) -> 6

    // 测试连接所有点
    MinCostConnectPoints mccp;
    vector<vector<int>> points = {{0,0},{2,2},{3,10},{5,2},{7,0}};
    // mccp.minCostConnectPoints(points) -> 20

    // 测试水资源分配
    MinCostToSupplyWater mctsw;
    vector<int> wells = {1, 2, 2};
    vector<vector<int>> pipes = {{1,2,1},{2,3,1}};
    // mctsw.minCostToSupplyWater(3, wells, pipes) -> 3

    return 0;
}
