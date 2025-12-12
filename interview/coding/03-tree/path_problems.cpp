/**
 * @file path_problems.cpp
 * @brief 二叉树路径问题 - 题目文件
 *
 * 路径问题是树问题中的高频考点，涉及路径查找、路径求和等。
 * 注意区分"根到叶子路径"和"任意节点间路径"的不同。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>

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
 * 给你一个二叉树的根节点 root，按任意顺序返回所有从根节点到叶子节点的路径。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *      \
 *       5
 *   输入: root = [1,2,3,null,5]
 *   输出: ["1->2->5","1->3"]
 *
 * @param root 根节点
 * @return 所有根到叶子的路径
 */
std::vector<std::string> binaryTreePaths(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目2: 路径总和
 *
 * 给你二叉树的根节点 root 和一个表示目标和的整数 targetSum。
 * 判断该树中是否存在根节点到叶子节点的路径，这条路径上所有节点值相加等于目标和 targetSum。
 *
 * 示例:
 *           5
 *          / \
 *         4   8
 *        /   / \
 *       11  13  4
 *      /  \      \
 *     7    2      1
 *   输入: root, targetSum = 22
 *   输出: true
 *   解释: 5->4->11->2 路径和为 22
 *
 * @param root 根节点
 * @param targetSum 目标和
 * @return 是否存在满足条件的路径
 */
bool hasPathSum(TreeNode* root, int targetSum) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目3: 路径总和 II
 *
 * 给你二叉树的根节点 root 和一个整数目标和 targetSum，
 * 找出所有从根节点到叶子节点路径总和等于给定目标和的路径。
 *
 * 示例:
 *           5
 *          / \
 *         4   8
 *        /   / \
 *       11  13  4
 *      /  \    / \
 *     7    2  5   1
 *   输入: root, targetSum = 22
 *   输出: [[5,4,11,2],[5,8,4,5]]
 *
 * @param root 根节点
 * @param targetSum 目标和
 * @return 所有满足条件的路径
 */
std::vector<std::vector<int>> pathSum(TreeNode* root, int targetSum) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目4: 路径总和 III
 *
 * 给定一个二叉树的根节点 root，和一个整数 targetSum，
 * 求该二叉树里节点值之和等于 targetSum 的路径的数目。
 *
 * 路径不需要从根节点开始，也不需要在叶子节点结束，
 * 但是路径方向必须是向下的（只能从父节点到子节点）。
 *
 * 示例:
 *           10
 *          /  \
 *         5   -3
 *        / \    \
 *       3   2   11
 *      / \   \
 *     3  -2   1
 *   输入: root, targetSum = 8
 *   输出: 3
 *   解释: 满足条件的路径有:
 *         5 -> 3
 *         5 -> 2 -> 1
 *         -3 -> 11
 *
 * @param root 根节点
 * @param targetSum 目标和
 * @return 满足条件的路径数目
 */
int pathSumIII(TreeNode* root, int targetSum) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 二叉树的最大路径和
 *
 * 路径被定义为一条从树中任意节点出发，沿父节点-子节点连接，达到任意节点的序列。
 * 同一个节点在一条路径序列中至多出现一次。该路径至少包含一个节点，且不一定经过根节点。
 *
 * 路径和是路径中各节点值的总和。
 *
 * 给你一个二叉树的根节点 root，返回其最大路径和。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *   输入: root = [1,2,3]
 *   输出: 6
 *   解释: 最优路径是 2 -> 1 -> 3，路径和为 2 + 1 + 3 = 6
 *
 *      -10
 *      /  \
 *     9   20
 *        /  \
 *       15   7
 *   输入: root = [-10,9,20,null,null,15,7]
 *   输出: 42
 *   解释: 最优路径是 15 -> 20 -> 7，路径和为 15 + 20 + 7 = 42
 *
 * @param root 根节点
 * @return 最大路径和
 */
int maxPathSum(TreeNode* root) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 二叉树中的最长交替路径
 *
 * 给你一棵以 root 为根的二叉树，返回树中最长交替路径的长度。
 *
 * 交替路径：你从 root 出发，然后轮流选择左孩子和右孩子（或者先选择右孩子再选择左孩子），一直走下去。
 *
 * 示例:
 *           1
 *            \
 *             1
 *            / \
 *           1   1
 *              / \
 *             1   1
 *                  \
 *                   1
 *   输入: root
 *   输出: 4
 *   解释: 路径是 1(根) -> 1(右) -> 1(左) -> 1(右) -> 1(右)，长度为 4
 *
 * @param root 根节点
 * @return 最长交替路径长度
 */
int longestZigZag(TreeNode* root) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 二叉树的直径
 *
 * 给定一棵二叉树，你需要计算它的直径长度。
 * 一棵二叉树的直径长度是任意两个结点路径长度中的最大值。
 * 这条路径可能穿过也可能不穿过根结点。
 *
 * 注意：两结点之间的路径长度是以它们之间边的数目表示。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *    / \
 *   4   5
 *   输入: root
 *   输出: 3
 *   解释: 路径 [4,2,1,3] 或者 [5,2,1,3] 的长度为 3
 *
 * @param root 根节点
 * @return 直径长度（边数）
 */
int diameterOfBinaryTree(TreeNode* root) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目8: 求根节点到叶节点数字之和
 *
 * 给你一个二叉树的根节点 root，树中每个节点都存放有一个 0 到 9 之间的数字。
 * 每条从根节点到叶节点的路径都代表一个数字。
 *
 * 例如，从根节点到叶节点的路径 1 -> 2 -> 3 表示数字 123。
 *
 * 计算从根节点到叶节点生成的所有数字之和。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *   输入: root = [1,2,3]
 *   输出: 25
 *   解释: 从根到叶子节点路径 1->2 代表数字 12
 *         从根到叶子节点路径 1->3 代表数字 13
 *         因此，数字总和 = 12 + 13 = 25
 *
 *         4
 *        / \
 *       9   0
 *      / \
 *     5   1
 *   输入: root = [4,9,0,5,1]
 *   输出: 1026
 *   解释: 495 + 491 + 40 = 1026
 *
 * @param root 根节点
 * @return 所有数字之和
 */
int sumNumbers(TreeNode* root) {
    // TODO: 实现你的解法
    return 0;
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
#include <algorithm>

void testBinaryTreePaths() {
    TreeNode* root = createTree({1, 2, 3, -1, 5});
    auto result = binaryTreePaths(root);
    std::sort(result.begin(), result.end());
    std::vector<std::string> expected = {"1->2->5", "1->3"};
    std::sort(expected.begin(), expected.end());
    assert(result == expected);
    deleteTree(root);

    std::cout << "binaryTreePaths: PASSED\n";
}

void testHasPathSum() {
    TreeNode* root = createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, -1, 1});
    assert(hasPathSum(root, 22) == true);
    assert(hasPathSum(root, 100) == false);
    deleteTree(root);

    assert(hasPathSum(nullptr, 0) == false);

    std::cout << "hasPathSum: PASSED\n";
}

void testPathSum() {
    TreeNode* root = createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, 5, 1});
    auto result = pathSum(root, 22);
    assert(result.size() == 2);
    deleteTree(root);

    std::cout << "pathSum: PASSED\n";
}

void testPathSumIII() {
    TreeNode* root = createTree({10, 5, -3, 3, 2, -1, 11, 3, -2, -1, 1});
    assert(pathSumIII(root, 8) == 3);
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

    std::cout << "maxPathSum: PASSED\n";
}

void testDiameterOfBinaryTree() {
    TreeNode* root = createTree({1, 2, 3, 4, 5});
    assert(diameterOfBinaryTree(root) == 3);
    deleteTree(root);

    std::cout << "diameterOfBinaryTree: PASSED\n";
}

void testSumNumbers() {
    TreeNode* root = createTree({1, 2, 3});
    assert(sumNumbers(root) == 25);
    deleteTree(root);

    root = createTree({4, 9, 0, 5, 1});
    assert(sumNumbers(root) == 1026);
    deleteTree(root);

    std::cout << "sumNumbers: PASSED\n";
}

int main() {
    std::cout << "=== Path Problems Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testBinaryTreePaths();
    // testHasPathSum();
    // testPathSum();
    // testPathSumIII();
    // testMaxPathSum();
    // testDiameterOfBinaryTree();
    // testSumNumbers();

    std::cout << "All tests passed!\n";
    return 0;
}
