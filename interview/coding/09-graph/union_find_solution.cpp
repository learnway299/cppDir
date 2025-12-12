/**
 * @file union_find_solution.cpp
 * @brief 并查集 - 解答
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <set>

using namespace std;

/**
 * 题目1：并查集模板
 *
 * 优化技巧：
 * 1. 路径压缩：find 时将节点直接指向根
 * 2. 按秩合并：将小树合并到大树
 * 时间复杂度：均摊 O(α(n))，α 是阿克曼函数的反函数
 */
class UnionFind {
private:
    vector<int> parent;
    vector<int> rank_;
    int count;

public:
    UnionFind(int n) : parent(n), rank_(n, 0), count(n) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);  // 路径压缩
        }
        return parent[x];
    }

    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX == rootY) return;

        // 按秩合并
        if (rank_[rootX] < rank_[rootY]) {
            parent[rootX] = rootY;
        } else if (rank_[rootX] > rank_[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank_[rootX]++;
        }
        --count;
    }

    bool connected(int x, int y) {
        return find(x) == find(y);
    }

    int getCount() {
        return count;
    }
};

/**
 * 题目2：省份数量 (LeetCode 547)
 *
 * 思路：并查集合并相连的城市，返回连通分量数
 * 时间复杂度：O(n^2 * α(n))
 * 空间复杂度：O(n)
 */
class FindCircleNum {
public:
    int findCircleNum(vector<vector<int>>& isConnected) {
        int n = isConnected.size();
        UnionFind uf(n);

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (isConnected[i][j]) {
                    uf.unite(i, j);
                }
            }
        }

        return uf.getCount();
    }

    // DFS 解法
    int findCircleNumDFS(vector<vector<int>>& isConnected) {
        int n = isConnected.size();
        vector<bool> visited(n, false);
        int count = 0;

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                dfs(isConnected, visited, i);
                ++count;
            }
        }
        return count;
    }

private:
    void dfs(vector<vector<int>>& isConnected, vector<bool>& visited, int i) {
        visited[i] = true;
        for (int j = 0; j < isConnected.size(); ++j) {
            if (isConnected[i][j] && !visited[j]) {
                dfs(isConnected, visited, j);
            }
        }
    }
};

/**
 * 题目3：冗余连接 (LeetCode 684)
 *
 * 思路：按顺序加边，第一条使图成环的边就是答案
 * 时间复杂度：O(n * α(n))
 * 空间复杂度：O(n)
 */
class FindRedundantConnection {
public:
    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        int n = edges.size();
        UnionFind uf(n + 1);  // 节点从 1 开始

        for (auto& edge : edges) {
            if (uf.connected(edge[0], edge[1])) {
                return edge;  // 已连通，再加边会成环
            }
            uf.unite(edge[0], edge[1]);
        }
        return {};
    }
};

/**
 * 题目4：账户合并 (LeetCode 721)
 *
 * 思路：
 * 1. 用邮箱作为并查集的元素
 * 2. 同一账户的邮箱合并到一起
 * 3. 按根节点分组，排序输出
 * 时间复杂度：O(n * k * α(nk))，k 是平均邮箱数
 * 空间复杂度：O(nk)
 */
class AccountsMerge {
public:
    vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
        unordered_map<string, int> emailToIndex;
        unordered_map<string, string> emailToName;
        int emailCount = 0;

        // 为每个邮箱分配索引
        for (auto& account : accounts) {
            string& name = account[0];
            for (int i = 1; i < account.size(); ++i) {
                string& email = account[i];
                if (!emailToIndex.count(email)) {
                    emailToIndex[email] = emailCount++;
                    emailToName[email] = name;
                }
            }
        }

        // 并查集
        UnionFind uf(emailCount);
        for (auto& account : accounts) {
            int firstIndex = emailToIndex[account[1]];
            for (int i = 2; i < account.size(); ++i) {
                uf.unite(firstIndex, emailToIndex[account[i]]);
            }
        }

        // 按根节点分组
        unordered_map<int, set<string>> groups;
        for (auto& [email, index] : emailToIndex) {
            int root = uf.find(index);
            groups[root].insert(email);
        }

        // 构建结果
        vector<vector<string>> result;
        for (auto& [root, emails] : groups) {
            vector<string> account;
            account.push_back(emailToName[*emails.begin()]);
            for (auto& email : emails) {
                account.push_back(email);
            }
            result.push_back(account);
        }

        return result;
    }
};

/**
 * 题目5：等式方程的可满足性 (LeetCode 990)
 *
 * 思路：
 * 1. 先处理所有 == 方程，将相等的变量合并
 * 2. 再处理所有 != 方程，检查是否矛盾
 * 时间复杂度：O(n * α(26))
 * 空间复杂度：O(26) = O(1)
 */
class EquationsPossible {
public:
    bool equationsPossible(vector<string>& equations) {
        UnionFind uf(26);

        // 处理 == 方程
        for (auto& eq : equations) {
            if (eq[1] == '=') {
                uf.unite(eq[0] - 'a', eq[3] - 'a');
            }
        }

        // 检查 != 方程
        for (auto& eq : equations) {
            if (eq[1] == '!') {
                if (uf.connected(eq[0] - 'a', eq[3] - 'a')) {
                    return false;
                }
            }
        }

        return true;
    }
};

/**
 * 题目6：最长连续序列 (LeetCode 128) - 并查集解法
 *
 * 思路：
 * 1. 将相邻的数合并（如果存在）
 * 2. 记录每个集合的大小
 * 时间复杂度：O(n * α(n))
 * 空间复杂度：O(n)
 */
class LongestConsecutive {
public:
    int longestConsecutive(vector<int>& nums) {
        if (nums.empty()) return 0;

        unordered_map<int, int> numToIndex;
        int n = nums.size();

        // 去重并建立映射
        for (int i = 0; i < n; ++i) {
            numToIndex[nums[i]] = i;
        }

        UnionFind uf(n);
        unordered_map<int, int> size;  // 根节点 -> 集合大小

        for (auto& [num, idx] : numToIndex) {
            size[uf.find(idx)] = 1;
        }

        for (auto& [num, idx] : numToIndex) {
            // 与 num+1 合并
            if (numToIndex.count(num + 1)) {
                int root1 = uf.find(idx);
                int root2 = uf.find(numToIndex[num + 1]);
                if (root1 != root2) {
                    int newSize = size[root1] + size[root2];
                    uf.unite(idx, numToIndex[num + 1]);
                    size[uf.find(idx)] = newSize;
                }
            }
        }

        int maxLen = 1;
        for (auto& [root, sz] : size) {
            maxLen = max(maxLen, sz);
        }
        return maxLen;
    }

    // 更简洁的哈希集合解法（推荐）
    int longestConsecutiveHash(vector<int>& nums) {
        unordered_set<int> numSet(nums.begin(), nums.end());
        int maxLen = 0;

        for (int num : numSet) {
            // 只从序列的起点开始计数
            if (!numSet.count(num - 1)) {
                int currentNum = num;
                int length = 1;

                while (numSet.count(currentNum + 1)) {
                    ++currentNum;
                    ++length;
                }

                maxLen = max(maxLen, length);
            }
        }

        return maxLen;
    }
};

int main() {
    // 测试省份数量
    FindCircleNum fc;
    vector<vector<int>> isConnected = {{1,1,0},{1,1,0},{0,0,1}};
    // fc.findCircleNum(isConnected) -> 2

    // 测试冗余连接
    FindRedundantConnection frc;
    vector<vector<int>> edges = {{1,2},{1,3},{2,3}};
    // frc.findRedundantConnection(edges) -> [2,3]

    // 测试等式方程
    EquationsPossible ep;
    vector<string> eq1 = {"a==b", "b!=a"};
    // ep.equationsPossible(eq1) -> false

    vector<string> eq2 = {"a==b", "b==c", "a==c"};
    // ep.equationsPossible(eq2) -> true

    // 测试最长连续序列
    LongestConsecutive lc;
    vector<int> nums = {100, 4, 200, 1, 3, 2};
    // lc.longestConsecutive(nums) -> 4

    return 0;
}
