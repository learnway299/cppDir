/**
 * @file mst.cpp
 * @brief 最小生成树 - 面试题
 *
 * 包含题目：
 * 1. Kruskal 算法
 * 2. Prim 算法
 * 3. 最低成本联通所有城市 (LeetCode 1135)
 * 4. 连接所有点的最小费用 (LeetCode 1584)
 * 5. 水资源分配优化 (LeetCode 1168)
 */

#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

/**
 * 题目1：Kruskal 算法
 *
 * 给定一个无向加权图，使用 Kruskal 算法求最小生成树的权值和。
 *
 * 示例：
 * 输入：n = 4, edges = [[0,1,1],[0,2,2],[1,2,5],[1,3,1],[2,3,8]]
 * 输出：4
 */
class Kruskal {
public:
    int minCostKruskal(int n, vector<vector<int>>& edges) {
        // TODO: 实现 Kruskal 算法
        return 0;
    }
};

/**
 * 题目2：Prim 算法
 *
 * 给定一个无向加权图，使用 Prim 算法求最小生成树的权值和。
 *
 * 示例：
 * 输入：n = 4, edges = [[0,1,1],[0,2,2],[1,2,5],[1,3,1],[2,3,8]]
 * 输出：4
 */
class Prim {
public:
    int minCostPrim(int n, vector<vector<int>>& edges) {
        // TODO: 实现 Prim 算法
        return 0;
    }
};

/**
 * 题目3：最低成本联通所有城市 (LeetCode 1135)
 *
 * 想象一下你是个城市基建规划者，地图上有 n 座城市，它们按以 1 到 n 的次序编号。
 * 给你整数 n 和一个数组 connections，其中 connections[i] = [xi, yi, costi]
 * 表示将城市 xi 和城市 yi 连接所要的成本。
 * 返回连通所有城市的最低成本，如果不能连通所有城市，返回 -1。
 *
 * 示例：
 * 输入：n = 3, connections = [[1,2,5],[1,3,6],[2,3,1]]
 * 输出：6
 */
class MinimumCost {
public:
    int minimumCost(int n, vector<vector<int>>& connections) {
        // TODO: 实现最低成本联通
        return -1;
    }
};

/**
 * 题目4：连接所有点的最小费用 (LeetCode 1584)
 *
 * 给你一个 points 数组，表示 2D 平面上的一些点，其中 points[i] = [xi, yi]。
 * 连接点 [xi, yi] 和点 [xj, yj] 的费用为它们之间的曼哈顿距离：|xi - xj| + |yi - yj|。
 * 返回将所有点连接的最小总费用。
 *
 * 示例：
 * 输入：points = [[0,0],[2,2],[3,10],[5,2],[7,0]]
 * 输出：20
 */
class MinCostConnectPoints {
public:
    int minCostConnectPoints(vector<vector<int>>& points) {
        // TODO: 实现连接所有点的最小费用
        return 0;
    }
};

/**
 * 题目5：水资源分配优化 (LeetCode 1168)
 *
 * 村里面一共有 n 栋房子。我们希望通过建造水井和铺设管道来为所有房子供水。
 * 对于每个房子 i，我们可以花费 wells[i-1] 来建造水井，
 * 或者用 pipes[j] = [house1j, house2j, costj] 的成本铺设管道。
 * 请你帮忙计算为所有房子都供水的最低总成本。
 *
 * 示例：
 * 输入：n = 3, wells = [1,2,2], pipes = [[1,2,1],[2,3,1]]
 * 输出：3
 */
class MinCostToSupplyWater {
public:
    int minCostToSupplyWater(int n, vector<int>& wells, vector<vector<int>>& pipes) {
        // TODO: 实现水资源分配优化
        return 0;
    }
};

int main() {
    return 0;
}
