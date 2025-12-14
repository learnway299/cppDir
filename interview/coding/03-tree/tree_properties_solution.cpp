/**
 * @file tree_properties_solution.cpp
 * @brief 二叉树属性问题 - 参考答案
 */

#include "tree_properties.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>

namespace TreeProperties {

// ==================== 辅助函数 ====================

static int checkHeight(TreeNode* root) {
    if (!root) return 0;
    int leftH = checkHeight(root->left);
    if (leftH == -1) return -1;
    int rightH = checkHeight(root->right);
    if (rightH == -1) return -1;
    if (std::abs(leftH - rightH) > 1) return -1;
    return 1 + std::max(leftH, rightH);
}

static bool isMirror(TreeNode* left, TreeNode* right) {
    if (!left && !right) return true;
    if (!left || !right) return false;
    return left->val == right->val &&
           isMirror(left->left, right->right) &&
           isMirror(left->right, right->left);
}

// ==================== 参考答案实现 ====================

// 题目1: 二叉树的最大深度 - DFS
int maxDepthSolution(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(maxDepthSolution(root->left), maxDepthSolution(root->right));
}

// 题目2: 二叉树的最小深度 - DFS
int minDepthSolution(TreeNode* root) {
    if (!root) return 0;
    if (!root->left) return 1 + minDepthSolution(root->right);
    if (!root->right) return 1 + minDepthSolution(root->left);
    return 1 + std::min(minDepthSolution(root->left), minDepthSolution(root->right));
}

// 题目3: 平衡二叉树
bool isBalancedSolution(TreeNode* root) {
    return checkHeight(root) != -1;
}

// 题目4: 对称二叉树
bool isSymmetricSolution(TreeNode* root) {
    if (!root) return true;
    return isMirror(root->left, root->right);
}

// 题目5: 相同的树
bool isSameTreeSolution(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val &&
           isSameTreeSolution(p->left, q->left) &&
           isSameTreeSolution(p->right, q->right);
}

// 题目6: 另一棵树的子树
bool isSubtreeSolution(TreeNode* root, TreeNode* subRoot) {
    if (!root) return false;
    if (isSameTreeSolution(root, subRoot)) return true;
    return isSubtreeSolution(root->left, subRoot) || isSubtreeSolution(root->right, subRoot);
}

// 题目7: 翻转二叉树
TreeNode* invertTreeSolution(TreeNode* root) {
    if (!root) return nullptr;
    std::swap(root->left, root->right);
    invertTreeSolution(root->left);
    invertTreeSolution(root->right);
    return root;
}

// 题目8: 合并二叉树
TreeNode* mergeTreesSolution(TreeNode* root1, TreeNode* root2) {
    if (!root1) return root2;
    if (!root2) return root1;
    root1->val += root2->val;
    root1->left = mergeTreesSolution(root1->left, root2->left);
    root1->right = mergeTreesSolution(root1->right, root2->right);
    return root1;
}

// ==================== 测试函数 ====================

void testTreePropertiesSolution() {
    std::cout << "=== Tree Properties Tests (Solution) ===\n";

    // maxDepth
    TreeNode* root = Util::createTree({3, 9, 20, -1, -1, 15, 7});
    assert(maxDepthSolution(root) == 3);
    Util::deleteTree(root);
    std::cout << "  maxDepthSolution: PASSED\n";

    // minDepth
    root = Util::createTree({3, 9, 20, -1, -1, 15, 7});
    assert(minDepthSolution(root) == 2);
    Util::deleteTree(root);
    std::cout << "  minDepthSolution: PASSED\n";

    // isBalanced
    root = Util::createTree({3, 9, 20, -1, -1, 15, 7});
    assert(isBalancedSolution(root) == true);
    Util::deleteTree(root);
    std::cout << "  isBalancedSolution: PASSED\n";

    // isSymmetric
    root = Util::createTree({1, 2, 2, 3, 4, 4, 3});
    assert(isSymmetricSolution(root) == true);
    Util::deleteTree(root);
    std::cout << "  isSymmetricSolution: PASSED\n";

    // isSameTree
    TreeNode* p = Util::createTree({1, 2, 3});
    TreeNode* q = Util::createTree({1, 2, 3});
    assert(isSameTreeSolution(p, q) == true);
    Util::deleteTree(p);
    Util::deleteTree(q);
    std::cout << "  isSameTreeSolution: PASSED\n";

    // isSubtree
    root = Util::createTree({3, 4, 5, 1, 2});
    TreeNode* subRoot = Util::createTree({4, 1, 2});
    assert(isSubtreeSolution(root, subRoot) == true);
    Util::deleteTree(root);
    Util::deleteTree(subRoot);
    std::cout << "  isSubtreeSolution: PASSED\n";

    // invertTree
    root = Util::createTree({4, 2, 7, 1, 3, 6, 9});
    TreeNode* inverted = invertTreeSolution(root);
    assert(inverted->left->val == 7);
    Util::deleteTree(inverted);
    std::cout << "  invertTreeSolution: PASSED\n";

    // mergeTrees
    TreeNode* t1 = Util::createTree({1, 3, 2, 5});
    TreeNode* t2 = Util::createTree({2, 1, 3, -1, 4, -1, 7});
    TreeNode* merged = mergeTreesSolution(t1, t2);
    assert(merged->val == 3);
    Util::deleteTree(merged);
    std::cout << "  mergeTreesSolution: PASSED\n";
}

} // namespace TreeProperties
