/**
 * @file traversal.cpp
 * @brief 图的遍历算法 - 面试题
 *
 * 包含题目：
 * 1. DFS 深度优先遍历（邻接表）
 * 2. BFS 广度优先遍历（邻接表）
 * 3. 岛屿数量 (LeetCode 200)
 * 4. 克隆图 (LeetCode 133)
 * 5. 所有路径 (LeetCode 797)
 * 6. 判断二分图 (LeetCode 785)
 */

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

/**
 * 题目1：DFS 深度优先遍历
 *
 * 给定一个图的邻接表表示，从指定节点开始进行深度优先遍历
 * 返回遍历顺序
 *
 * 示例：
 * 输入：adj = [[1,2], [0,3], [0,3], [1,2]], start = 0
 * 输出：[0, 1, 3, 2] 或其他有效的 DFS 序列
 */
class DFSTraversal {
public:
    vector<int> dfs(vector<vector<int>>& adj, int start) {
        // TODO: 实现 DFS 遍历
        return {};
    }
};

/**
 * 题目2：BFS 广度优先遍历
 *
 * 给定一个图的邻接表表示，从指定节点开始进行广度优先遍历
 * 返回遍历顺序
 *
 * 示例：
 * 输入：adj = [[1,2], [0,3], [0,3], [1,2]], start = 0
 * 输出：[0, 1, 2, 3]
 */
class BFSTraversal {
public:
    vector<int> bfs(vector<vector<int>>& adj, int start) {
        // TODO: 实现 BFS 遍历
        return {};
    }
};

/**
 * 题目3：岛屿数量 (LeetCode 200)
 *
 * 给你一个由 '1'（陆地）和 '0'（水）组成的二维网格，
 * 请你计算网格中岛屿的数量。
 *
 * 示例：
 * 输入：grid = [
 *   ["1","1","0","0","0"],
 *   ["1","1","0","0","0"],
 *   ["0","0","1","0","0"],
 *   ["0","0","0","1","1"]
 * ]
 * 输出：3
 */
class NumIslands {
public:
    int numIslands(vector<vector<char>>& grid) {
        // TODO: 实现岛屿数量统计
        return 0;
    }
};

/**
 * 题目4：克隆图 (LeetCode 133)
 *
 * 给你无向连通图中一个节点的引用，请你返回该图的深拷贝（克隆）。
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
        // TODO: 实现图的克隆
        return nullptr;
    }
};

/**
 * 题目5：所有可能的路径 (LeetCode 797)
 *
 * 给你一个有 n 个节点的有向无环图（DAG），请你找出所有从节点 0 到节点 n-1 的路径并输出。
 *
 * 示例：
 * 输入：graph = [[1,2],[3],[3],[]]
 * 输出：[[0,1,3],[0,2,3]]
 */
class AllPathsSourceTarget {
public:
    vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
        // TODO: 实现找出所有路径
        return {};
    }
};

/**
 * 题目6：判断二分图 (LeetCode 785)
 *
 * 如果图是二分图，返回 true；否则，返回 false。
 *
 * 示例：
 * 输入：graph = [[1,3],[0,2],[1,3],[0,2]]
 * 输出：true
 */
class IsBipartite {
public:
    bool isBipartite(vector<vector<int>>& graph) {
        // TODO: 实现二分图判断
        return false;
    }
};

int main() {
    return 0;
}
