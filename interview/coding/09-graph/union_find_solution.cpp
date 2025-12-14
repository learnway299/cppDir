/**
 * @file union_find_solution.cpp
 * @brief 并查集 - 参考答案
 */

#include "union_find.h"
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>

namespace UnionFindProblems {

// ==================== 并查集模板实现 ====================

// 在头文件中声明，这里在 solution 中提供辅助实现
class UnionFindHelper {
public:
    std::vector<int> parent;
    std::vector<int> rank_;
    int count;

    UnionFindHelper(int n) : parent(n), rank_(n, 0), count(n) {
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

// ==================== 参考答案实现 ====================

int findCircleNumSolution(std::vector<std::vector<int>>& isConnected) {
    int n = static_cast<int>(isConnected.size());
    UnionFindHelper uf(n);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (isConnected[i][j]) {
                uf.unite(i, j);
            }
        }
    }

    return uf.getCount();
}

std::vector<int> findRedundantConnectionSolution(std::vector<std::vector<int>>& edges) {
    int n = static_cast<int>(edges.size());
    UnionFindHelper uf(n + 1);  // 节点从 1 开始

    for (auto& edge : edges) {
        if (uf.connected(edge[0], edge[1])) {
            return edge;  // 已连通，再加边会成环
        }
        uf.unite(edge[0], edge[1]);
    }
    return {};
}

std::vector<std::vector<std::string>> accountsMergeSolution(std::vector<std::vector<std::string>>& accounts) {
    std::unordered_map<std::string, int> emailToIndex;
    std::unordered_map<std::string, std::string> emailToName;
    int emailCount = 0;

    // 为每个邮箱分配索引
    for (auto& account : accounts) {
        std::string& name = account[0];
        for (size_t i = 1; i < account.size(); ++i) {
            std::string& email = account[i];
            if (!emailToIndex.count(email)) {
                emailToIndex[email] = emailCount++;
                emailToName[email] = name;
            }
        }
    }

    // 并查集
    UnionFindHelper uf(emailCount);
    for (auto& account : accounts) {
        int firstIndex = emailToIndex[account[1]];
        for (size_t i = 2; i < account.size(); ++i) {
            uf.unite(firstIndex, emailToIndex[account[i]]);
        }
    }

    // 按根节点分组
    std::unordered_map<int, std::set<std::string>> groups;
    for (auto& [email, index] : emailToIndex) {
        int root = uf.find(index);
        groups[root].insert(email);
    }

    // 构建结果
    std::vector<std::vector<std::string>> result;
    for (auto& [root, emails] : groups) {
        std::vector<std::string> account;
        account.push_back(emailToName[*emails.begin()]);
        for (auto& email : emails) {
            account.push_back(email);
        }
        result.push_back(account);
    }

    return result;
}

bool equationsPossibleSolution(std::vector<std::string>& equations) {
    UnionFindHelper uf(26);

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

int longestConsecutiveSolution(std::vector<int>& nums) {
    // 使用哈希集合解法（更简洁）
    std::unordered_set<int> numSet(nums.begin(), nums.end());
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

            maxLen = std::max(maxLen, length);
        }
    }

    return maxLen;
}

// ==================== 测试函数 ====================

void testUnionFindSolution() {
    std::cout << "=== Union Find Tests (Solution) ===\n";

    // 测试并查集基本操作
    UnionFindHelper uf(5);
    uf.unite(0, 1);
    uf.unite(2, 3);
    assert(uf.connected(0, 1) == true);
    assert(uf.connected(0, 2) == false);
    assert(uf.getCount() == 3);
    std::cout << "  UnionFindHelper basic: PASSED\n";

    // 测试省份数量
    std::vector<std::vector<int>> isConnected = {{1,1,0},{1,1,0},{0,0,1}};
    assert(findCircleNumSolution(isConnected) == 2);
    std::cout << "  findCircleNumSolution: PASSED\n";

    // 测试冗余连接
    std::vector<std::vector<int>> edges = {{1,2},{1,3},{2,3}};
    auto redundant = findRedundantConnectionSolution(edges);
    assert(redundant[0] == 2 && redundant[1] == 3);
    std::cout << "  findRedundantConnectionSolution: PASSED\n";

    // 测试等式方程（false case）
    std::vector<std::string> eq1 = {"a==b", "b!=a"};
    assert(equationsPossibleSolution(eq1) == false);
    std::cout << "  equationsPossibleSolution (false case): PASSED\n";

    // 测试等式方程（true case）
    std::vector<std::string> eq2 = {"a==b", "b==c", "a==c"};
    assert(equationsPossibleSolution(eq2) == true);
    std::cout << "  equationsPossibleSolution (true case): PASSED\n";

    // 测试最长连续序列
    std::vector<int> nums = {100, 4, 200, 1, 3, 2};
    assert(longestConsecutiveSolution(nums) == 4);
    std::cout << "  longestConsecutiveSolution: PASSED\n";
}

} // namespace UnionFindProblems
