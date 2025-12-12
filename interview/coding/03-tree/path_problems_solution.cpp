/**
 * @file path_problems_solution.cpp
 * @brief 二叉树路径问题 - 解答文件
 *
 * 包含 path_problems.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>
#include <unordered_map>
#include <algorithm>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

/**
 * 题目1: 二叉树的所有路径
 *
 * 解法: DFS 回溯
 * 时间复杂度: O(n^2) - 最坏情况下路径长度为 n
 * 空间复杂度: O(n)
 */
void binaryTreePathsHelper(TreeNode* root, std::string path, std::vector<std::string>& result) {
    if (!root) return;

    path += std::to_string(root->val);

    // 叶子节点
    if (!root->left && !root->right) {
        result.push_back(path);
        return;
    }

    // 非叶子节点，继续搜索
    path += "->";
    binaryTreePathsHelper(root->left, path, result);
    binaryTreePathsHelper(root->right, path, result);
}

std::vector<std::string> binaryTreePaths(TreeNode* root) {
    std::vector<std::string> result;
    binaryTreePathsHelper(root, "", result);
    return result;
}

/**
 * 解法2: BFS + 路径记录
 */
std::vector<std::string> binaryTreePathsBFS(TreeNode* root) {
    std::vector<std::string> result;
    if (!root) return result;

    std::queue<std::pair<TreeNode*, std::string>> q;
    q.push({root, std::to_string(root->val)});

    while (!q.empty()) {
        auto [node, path] = q.front();
        q.pop();

        if (!node->left && !node->right) {
            result.push_back(path);
            continue;
        }

        if (node->left) {
            q.push({node->left, path + "->" + std::to_string(node->left->val)});
        }
        if (node->right) {
            q.push({node->right, path + "->" + std::to_string(node->right->val)});
        }
    }

    return result;
}

/**
 * 题目2: 路径总和
 *
 * 解法1: DFS 递归
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
bool hasPathSum(TreeNode* root, int targetSum) {
    if (!root) return false;

    // 叶子节点
    if (!root->left && !root->right) {
        return root->val == targetSum;
    }

    // 递归检查左右子树
    int remaining = targetSum - root->val;
    return hasPathSum(root->left, remaining) || hasPathSum(root->right, remaining);
}

/**
 * 解法2: BFS 迭代
 */
bool hasPathSumBFS(TreeNode* root, int targetSum) {
    if (!root) return false;

    std::queue<std::pair<TreeNode*, int>> q;
    q.push({root, root->val});

    while (!q.empty()) {
        auto [node, sum] = q.front();
        q.pop();

        // 叶子节点
        if (!node->left && !node->right && sum == targetSum) {
            return true;
        }

        if (node->left) {
            q.push({node->left, sum + node->left->val});
        }
        if (node->right) {
            q.push({node->right, sum + node->right->val});
        }
    }

    return false;
}

/**
 * 题目3: 路径总和 II
 *
 * 解法: DFS + 回溯
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n)
 */
void pathSumHelper(TreeNode* root, int targetSum, std::vector<int>& path,
                   std::vector<std::vector<int>>& result) {
    if (!root) return;

    path.push_back(root->val);

    // 叶子节点且路径和等于目标
    if (!root->left && !root->right && root->val == targetSum) {
        result.push_back(path);
    } else {
        // 继续搜索
        pathSumHelper(root->left, targetSum - root->val, path, result);
        pathSumHelper(root->right, targetSum - root->val, path, result);
    }

    // 回溯
    path.pop_back();
}

std::vector<std::vector<int>> pathSum(TreeNode* root, int targetSum) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    pathSumHelper(root, targetSum, path, result);
    return result;
}

/**
 * 题目4: 路径总和 III
 *
 * 解法1: 前缀和 + 哈希表
 * 思路: 类似于数组中的"和为 K 的子数组"问题
 *       用前缀和记录从根到当前节点的路径和
 *       查找 prefixSum - targetSum 是否存在
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int pathSumIIIHelper(TreeNode* root, long long currSum, long long targetSum,
                     std::unordered_map<long long, int>& prefixSumCount) {
    if (!root) return 0;

    currSum += root->val;
    int count = 0;

    // 检查是否存在和为 currSum - targetSum 的前缀
    if (prefixSumCount.count(currSum - targetSum)) {
        count = prefixSumCount[currSum - targetSum];
    }

    // 将当前前缀和加入哈希表
    prefixSumCount[currSum]++;

    // 递归处理子树
    count += pathSumIIIHelper(root->left, currSum, targetSum, prefixSumCount);
    count += pathSumIIIHelper(root->right, currSum, targetSum, prefixSumCount);

    // 回溯：移除当前前缀和
    prefixSumCount[currSum]--;

    return count;
}

int pathSumIII(TreeNode* root, int targetSum) {
    std::unordered_map<long long, int> prefixSumCount;
    prefixSumCount[0] = 1;  // 空路径
    return pathSumIIIHelper(root, 0, targetSum, prefixSumCount);
}

/**
 * 解法2: 双递归（简单但效率较低）
 * 时间复杂度: O(n^2)
 */
int countPathsFromNode(TreeNode* root, long long targetSum) {
    if (!root) return 0;

    int count = (root->val == targetSum) ? 1 : 0;
    count += countPathsFromNode(root->left, targetSum - root->val);
    count += countPathsFromNode(root->right, targetSum - root->val);

    return count;
}

int pathSumIIIBruteForce(TreeNode* root, int targetSum) {
    if (!root) return 0;

    return countPathsFromNode(root, targetSum) +
           pathSumIIIBruteForce(root->left, targetSum) +
           pathSumIIIBruteForce(root->right, targetSum);
}

/**
 * 题目5: 二叉树的最大路径和
 *
 * 解法: DFS + 全局变量
 * 思路:
 *   - 对于每个节点，计算经过它的最大路径和
 *   - 经过节点的路径 = 左子树贡献 + 节点值 + 右子树贡献
 *   - 向父节点返回时，只能选择一条分支
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
int maxPathSumHelper(TreeNode* root, int& maxSum) {
    if (!root) return 0;

    // 计算左右子树的最大贡献（负数贡献为0，不选）
    int leftGain = std::max(0, maxPathSumHelper(root->left, maxSum));
    int rightGain = std::max(0, maxPathSumHelper(root->right, maxSum));

    // 经过当前节点的最大路径和
    int pathSum = root->val + leftGain + rightGain;

    // 更新全局最大值
    maxSum = std::max(maxSum, pathSum);

    // 返回给父节点的最大贡献（只能选一条分支）
    return root->val + std::max(leftGain, rightGain);
}

int maxPathSum(TreeNode* root) {
    int maxSum = INT_MIN;
    maxPathSumHelper(root, maxSum);
    return maxSum;
}

/**
 * 题目6: 二叉树中的最长交替路径
 *
 * 解法: DFS
 * 思路: 对于每个节点，记录从左边来和从右边来的最长路径
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
void longestZigZagHelper(TreeNode* root, bool fromLeft, int length, int& maxLength) {
    if (!root) return;

    maxLength = std::max(maxLength, length);

    if (fromLeft) {
        // 如果从左边来，下一步应该往右走继续交替
        longestZigZagHelper(root->right, false, length + 1, maxLength);
        // 或者重新开始往左走
        longestZigZagHelper(root->left, true, 1, maxLength);
    } else {
        // 如果从右边来，下一步应该往左走继续交替
        longestZigZagHelper(root->left, true, length + 1, maxLength);
        // 或者重新开始往右走
        longestZigZagHelper(root->right, false, 1, maxLength);
    }
}

int longestZigZag(TreeNode* root) {
    if (!root) return 0;

    int maxLength = 0;
    // 从根节点开始，分别尝试向左和向右
    longestZigZagHelper(root->left, true, 1, maxLength);
    longestZigZagHelper(root->right, false, 1, maxLength);

    return maxLength;
}

/**
 * 解法2: 返回值方式的 DFS
 */
// 返回 [往左走的长度, 往右走的长度]
std::pair<int, int> zigzagDFS(TreeNode* root, int& maxLength) {
    if (!root) return {0, 0};

    auto [leftL, leftR] = zigzagDFS(root->left, maxLength);
    auto [rightL, rightR] = zigzagDFS(root->right, maxLength);

    // 当前节点往左走，接上左子树的往右走长度
    int goLeft = leftR + 1;
    // 当前节点往右走，接上右子树的往左走长度
    int goRight = rightL + 1;

    maxLength = std::max(maxLength, std::max(goLeft, goRight));

    return {goLeft, goRight};
}

int longestZigZagV2(TreeNode* root) {
    int maxLength = 0;
    zigzagDFS(root, maxLength);
    return maxLength - 1;  // 边数 = 节点数 - 1
}

/**
 * 题目7: 二叉树的直径
 *
 * 解法: DFS
 * 思路: 直径 = 左子树深度 + 右子树深度
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
int diameterHelper(TreeNode* root, int& maxDiameter) {
    if (!root) return 0;

    int leftDepth = diameterHelper(root->left, maxDiameter);
    int rightDepth = diameterHelper(root->right, maxDiameter);

    // 更新最大直径
    maxDiameter = std::max(maxDiameter, leftDepth + rightDepth);

    // 返回当前子树的深度
    return 1 + std::max(leftDepth, rightDepth);
}

int diameterOfBinaryTree(TreeNode* root) {
    int maxDiameter = 0;
    diameterHelper(root, maxDiameter);
    return maxDiameter;
}

/**
 * 题目8: 求根节点到叶节点数字之和
 *
 * 解法1: DFS 递归
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
void sumNumbersHelper(TreeNode* root, int currentNum, int& totalSum) {
    if (!root) return;

    currentNum = currentNum * 10 + root->val;

    // 叶子节点
    if (!root->left && !root->right) {
        totalSum += currentNum;
        return;
    }

    sumNumbersHelper(root->left, currentNum, totalSum);
    sumNumbersHelper(root->right, currentNum, totalSum);
}

int sumNumbers(TreeNode* root) {
    int totalSum = 0;
    sumNumbersHelper(root, 0, totalSum);
    return totalSum;
}

/**
 * 解法2: 更简洁的递归
 */
int sumNumbersDFS(TreeNode* root, int currentNum) {
    if (!root) return 0;

    currentNum = currentNum * 10 + root->val;

    // 叶子节点
    if (!root->left && !root->right) {
        return currentNum;
    }

    return sumNumbersDFS(root->left, currentNum) + sumNumbersDFS(root->right, currentNum);
}

int sumNumbersV2(TreeNode* root) {
    return sumNumbersDFS(root, 0);
}

/**
 * 解法3: BFS 迭代
 */
int sumNumbersBFS(TreeNode* root) {
    if (!root) return 0;

    int totalSum = 0;
    std::queue<std::pair<TreeNode*, int>> q;
    q.push({root, root->val});

    while (!q.empty()) {
        auto [node, num] = q.front();
        q.pop();

        // 叶子节点
        if (!node->left && !node->right) {
            totalSum += num;
            continue;
        }

        if (node->left) {
            q.push({node->left, num * 10 + node->left->val});
        }
        if (node->right) {
            q.push({node->right, num * 10 + node->right->val});
        }
    }

    return totalSum;
}


// ==================== 辅助函数 ====================

TreeNode* createTree(const std::vector<int>& vals) {
    if (vals.empty() || vals[0] == -1) return nullptr;

    TreeNode* root = new TreeNode(vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);

    size_t i = 1;
    while (!q.empty() && i < vals.size()) {
        TreeNode* node = q.front();
        q.pop();

        if (i < vals.size() && vals[i] != -1) {
            node->left = new TreeNode(vals[i]);
            q.push(node->left);
        }
        ++i;

        if (i < vals.size() && vals[i] != -1) {
            node->right = new TreeNode(vals[i]);
            q.push(node->right);
        }
        ++i;
    }

    return root;
}

void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}


// ==================== 测试代码 ====================
#include <cassert>

void testBinaryTreePaths() {
    TreeNode* root = createTree({1, 2, 3, -1, 5});
    auto result = binaryTreePaths(root);
    std::sort(result.begin(), result.end());
    std::vector<std::string> expected = {"1->2->5", "1->3"};
    std::sort(expected.begin(), expected.end());
    assert(result == expected);

    // 测试 BFS 解法
    result = binaryTreePathsBFS(root);
    std::sort(result.begin(), result.end());
    assert(result == expected);

    deleteTree(root);

    // 单节点
    root = new TreeNode(1);
    result = binaryTreePaths(root);
    assert(result.size() == 1 && result[0] == "1");
    delete root;

    std::cout << "binaryTreePaths: PASSED\n";
}

void testHasPathSum() {
    TreeNode* root = createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, -1, 1});
    assert(hasPathSum(root, 22) == true);
    assert(hasPathSumBFS(root, 22) == true);
    assert(hasPathSum(root, 100) == false);
    deleteTree(root);

    assert(hasPathSum(nullptr, 0) == false);

    // 单节点
    root = new TreeNode(5);
    assert(hasPathSum(root, 5) == true);
    assert(hasPathSum(root, 1) == false);
    delete root;

    std::cout << "hasPathSum: PASSED\n";
}

void testPathSum() {
    TreeNode* root = createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, 5, 1});
    auto result = pathSum(root, 22);
    assert(result.size() == 2);
    deleteTree(root);

    // 无匹配路径
    root = createTree({1, 2, 3});
    result = pathSum(root, 100);
    assert(result.empty());
    deleteTree(root);

    std::cout << "pathSum: PASSED\n";
}

void testPathSumIII() {
    TreeNode* root = createTree({10, 5, -3, 3, 2, -1, 11, 3, -2, -1, 1});
    assert(pathSumIII(root, 8) == 3);
    assert(pathSumIIIBruteForce(root, 8) == 3);
    deleteTree(root);

    root = createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, 5, 1});
    assert(pathSumIII(root, 22) == 3);
    deleteTree(root);

    std::cout << "pathSumIII: PASSED\n";
}

void testMaxPathSum() {
    TreeNode* root = createTree({1, 2, 3});
    assert(maxPathSum(root) == 6);
    deleteTree(root);

    root = createTree({-10, 9, 20, -1, -1, 15, 7});
    assert(maxPathSum(root) == 42);
    deleteTree(root);

    // 全负数
    root = createTree({-3});
    assert(maxPathSum(root) == -3);
    delete root;

    root = createTree({-1, -2, -3});
    assert(maxPathSum(root) == -1);
    deleteTree(root);

    std::cout << "maxPathSum: PASSED\n";
}

void testLongestZigZag() {
    // 简单测试
    TreeNode* root = createTree({1, -1, 1, 1, 1, -1, -1, 1, 1, -1, 1});
    // 手动构建复杂树测试交替路径
    deleteTree(root);

    // 单节点
    root = new TreeNode(1);
    assert(longestZigZag(root) == 0);
    delete root;

    std::cout << "longestZigZag: PASSED\n";
}

void testDiameterOfBinaryTree() {
    TreeNode* root = createTree({1, 2, 3, 4, 5});
    assert(diameterOfBinaryTree(root) == 3);
    deleteTree(root);

    // 单边树
    root = createTree({1, 2, -1, 3, -1, 4});
    assert(diameterOfBinaryTree(root) == 3);
    deleteTree(root);

    // 单节点
    root = new TreeNode(1);
    assert(diameterOfBinaryTree(root) == 0);
    delete root;

    std::cout << "diameterOfBinaryTree: PASSED\n";
}

void testSumNumbers() {
    TreeNode* root = createTree({1, 2, 3});
    assert(sumNumbers(root) == 25);
    assert(sumNumbersV2(root) == 25);
    assert(sumNumbersBFS(root) == 25);
    deleteTree(root);

    root = createTree({4, 9, 0, 5, 1});
    assert(sumNumbers(root) == 1026);
    deleteTree(root);

    // 单节点
    root = new TreeNode(5);
    assert(sumNumbers(root) == 5);
    delete root;

    std::cout << "sumNumbers: PASSED\n";
}

int main() {
    std::cout << "=== Path Problems Solution Tests ===\n";

    testBinaryTreePaths();
    testHasPathSum();
    testPathSum();
    testPathSumIII();
    testMaxPathSum();
    testLongestZigZag();
    testDiameterOfBinaryTree();
    testSumNumbers();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
