/**
 * @file lca_solution.cpp
 * @brief 最近公共祖先问题 - 参考答案
 */

#include "lca.h"
#include <iostream>
#include <cassert>

namespace LCA {

// ==================== 辅助函数 ====================

static std::pair<int, TreeNode*> lcaDeepestLeavesHelper(TreeNode* root) {
    if (!root) return {0, nullptr};
    auto [leftDepth, leftLCA] = lcaDeepestLeavesHelper(root->left);
    auto [rightDepth, rightLCA] = lcaDeepestLeavesHelper(root->right);
    if (leftDepth > rightDepth) return {leftDepth + 1, leftLCA};
    if (rightDepth > leftDepth) return {rightDepth + 1, rightLCA};
    return {leftDepth + 1, root};
}

static bool foundP = false, foundQ = false;

static TreeNode* lowestCommonAncestorIIHelper(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return nullptr;
    TreeNode* left = lowestCommonAncestorIIHelper(root->left, p, q);
    TreeNode* right = lowestCommonAncestorIIHelper(root->right, p, q);
    if (root == p) { foundP = true; return root; }
    if (root == q) { foundQ = true; return root; }
    if (left && right) return root;
    return left ? left : right;
}

// ==================== 参考答案实现 ====================

// 题目1: 二叉树的最近公共祖先
TreeNode* lowestCommonAncestorSolution(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    TreeNode* left = lowestCommonAncestorSolution(root->left, p, q);
    TreeNode* right = lowestCommonAncestorSolution(root->right, p, q);
    if (left && right) return root;
    return left ? left : right;
}

// 题目2: 二叉搜索树的最近公共祖先
TreeNode* lowestCommonAncestorBSTSolution(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return nullptr;
    if (p->val < root->val && q->val < root->val)
        return lowestCommonAncestorBSTSolution(root->left, p, q);
    if (p->val > root->val && q->val > root->val)
        return lowestCommonAncestorBSTSolution(root->right, p, q);
    return root;
}

// 题目3: 有父指针的二叉树的最近公共祖先
ParentNode* lowestCommonAncestorWithParentSolution(ParentNode* p, ParentNode* q) {
    ParentNode* a = p;
    ParentNode* b = q;
    while (a != b) {
        a = a ? a->parent : q;
        b = b ? b->parent : p;
    }
    return a;
}

// 题目4: 二叉树的最近公共祖先 II（p或q可能不存在）
TreeNode* lowestCommonAncestorIISolution(TreeNode* root, TreeNode* p, TreeNode* q) {
    foundP = false;
    foundQ = false;
    TreeNode* result = lowestCommonAncestorIIHelper(root, p, q);
    if (foundP && foundQ) return result;
    return nullptr;
}

// 题目5: 二叉树的最近公共祖先 III（多个节点）
TreeNode* lowestCommonAncestorMultipleSolution(TreeNode* root, std::vector<TreeNode*>& nodes) {
    if (nodes.empty()) return nullptr;
    TreeNode* result = nodes[0];
    for (size_t i = 1; i < nodes.size(); ++i) {
        result = lowestCommonAncestorSolution(root, result, nodes[i]);
        if (result == root) break;
    }
    return result;
}

// 题目6: 二叉树的最深叶节点的最近公共祖先
TreeNode* lcaDeepestLeavesSolution(TreeNode* root) {
    return lcaDeepestLeavesHelper(root).second;
}

// ==================== 测试函数 ====================

void testLCASolution() {
    std::cout << "=== LCA Tests (Solution) ===\n";

    // lowestCommonAncestor
    TreeNode* root = Util::createTree({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    TreeNode* p = Util::findNode(root, 5);
    TreeNode* q = Util::findNode(root, 1);
    TreeNode* lca = lowestCommonAncestorSolution(root, p, q);
    assert(lca != nullptr && lca->val == 3);
    p = Util::findNode(root, 5);
    q = Util::findNode(root, 4);
    lca = lowestCommonAncestorSolution(root, p, q);
    assert(lca != nullptr && lca->val == 5);
    Util::deleteTree(root);
    std::cout << "  lowestCommonAncestorSolution: PASSED\n";

    // lowestCommonAncestorBST
    root = Util::createTree({6, 2, 8, 0, 4, 7, 9, -1, -1, 3, 5});
    p = Util::findNode(root, 2);
    q = Util::findNode(root, 8);
    lca = lowestCommonAncestorBSTSolution(root, p, q);
    assert(lca != nullptr && lca->val == 6);
    p = Util::findNode(root, 2);
    q = Util::findNode(root, 4);
    lca = lowestCommonAncestorBSTSolution(root, p, q);
    assert(lca != nullptr && lca->val == 2);
    Util::deleteTree(root);
    std::cout << "  lowestCommonAncestorBSTSolution: PASSED\n";

    // lowestCommonAncestorWithParent
    ParentNode* rootP = Util::createTreeWithParent({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    ParentNode* pN = Util::findNodeWithParent(rootP, 5);
    ParentNode* qN = Util::findNodeWithParent(rootP, 1);
    ParentNode* lcaN = lowestCommonAncestorWithParentSolution(pN, qN);
    assert(lcaN != nullptr && lcaN->val == 3);
    pN = Util::findNodeWithParent(rootP, 5);
    qN = Util::findNodeWithParent(rootP, 4);
    lcaN = lowestCommonAncestorWithParentSolution(pN, qN);
    assert(lcaN != nullptr && lcaN->val == 5);
    Util::deleteTreeWithParent(rootP);
    std::cout << "  lowestCommonAncestorWithParentSolution: PASSED\n";

    // lcaDeepestLeaves
    root = Util::createTree({3, 5, 1, 6, 2, -1, -1, -1, -1, 7, 4});
    TreeNode* result = lcaDeepestLeavesSolution(root);
    assert(result != nullptr && result->val == 2);
    Util::deleteTree(root);
    std::cout << "  lcaDeepestLeavesSolution: PASSED\n";

    // lowestCommonAncestorMultiple
    root = Util::createTree({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    std::vector<TreeNode*> nodes = {Util::findNode(root, 4), Util::findNode(root, 7)};
    lca = lowestCommonAncestorMultipleSolution(root, nodes);
    assert(lca != nullptr && lca->val == 2);
    Util::deleteTree(root);
    std::cout << "  lowestCommonAncestorMultipleSolution: PASSED\n";
}

} // namespace LCA
