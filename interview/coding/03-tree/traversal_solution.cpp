/**
 * @file traversal_solution.cpp
 * @brief 二叉树遍历问题 - 参考答案
 */

#include "traversal.h"
#include <iostream>
#include <cassert>
#include <stack>
#include <queue>
#include <algorithm>

namespace TreeTraversal {

// ==================== 参考答案实现 ====================

// 题目1: 前序遍历 - 迭代
std::vector<int> preorderTraversalSolution(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;
    std::stack<TreeNode*> stk;
    stk.push(root);
    while (!stk.empty()) {
        TreeNode* node = stk.top();
        stk.pop();
        result.push_back(node->val);
        if (node->right) stk.push(node->right);
        if (node->left) stk.push(node->left);
    }
    return result;
}

// 题目2: 中序遍历 - 迭代
std::vector<int> inorderTraversalSolution(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> stk;
    TreeNode* curr = root;
    while (curr || !stk.empty()) {
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }
        curr = stk.top();
        stk.pop();
        result.push_back(curr->val);
        curr = curr->right;
    }
    return result;
}

// 题目3: 后序遍历 - 迭代（反转法）
std::vector<int> postorderTraversalSolution(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;
    std::stack<TreeNode*> stk;
    stk.push(root);
    while (!stk.empty()) {
        TreeNode* node = stk.top();
        stk.pop();
        result.push_back(node->val);
        if (node->left) stk.push(node->left);
        if (node->right) stk.push(node->right);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// 题目4: 层序遍历 - BFS
std::vector<std::vector<int>> levelOrderSolution(TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (!root) return result;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int levelSize = static_cast<int>(q.size());
        std::vector<int> level;
        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();
            level.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
    }
    return result;
}

// 题目5: 锯齿形层序遍历
std::vector<std::vector<int>> zigzagLevelOrderSolution(TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (!root) return result;
    std::queue<TreeNode*> q;
    q.push(root);
    bool leftToRight = true;
    while (!q.empty()) {
        int levelSize = static_cast<int>(q.size());
        std::vector<int> level(levelSize);
        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();
            int idx = leftToRight ? i : levelSize - 1 - i;
            level[idx] = node->val;
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
        leftToRight = !leftToRight;
    }
    return result;
}

// 题目6: 二叉树的右视图
std::vector<int> rightSideViewSolution(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int levelSize = static_cast<int>(q.size());
        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();
            if (i == levelSize - 1) result.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    return result;
}

// ==================== 测试函数 ====================

void testTreeTraversalSolution() {
    std::cout << "=== Tree Traversal Tests (Solution) ===\n";

    // preorderTraversal
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);
    assert(preorderTraversalSolution(root) == std::vector<int>({1, 2, 3}));
    Util::deleteTree(root);
    std::cout << "  preorderTraversalSolution: PASSED\n";

    // inorderTraversal
    root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);
    assert(inorderTraversalSolution(root) == std::vector<int>({1, 3, 2}));
    Util::deleteTree(root);
    std::cout << "  inorderTraversalSolution: PASSED\n";

    // postorderTraversal
    root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);
    assert(postorderTraversalSolution(root) == std::vector<int>({3, 2, 1}));
    Util::deleteTree(root);
    std::cout << "  postorderTraversalSolution: PASSED\n";

    // levelOrder
    root = Util::createTree({3, 9, 20, -1, -1, 15, 7});
    std::vector<std::vector<int>> expected = {{3}, {9, 20}, {15, 7}};
    assert(levelOrderSolution(root) == expected);
    Util::deleteTree(root);
    std::cout << "  levelOrderSolution: PASSED\n";

    // zigzagLevelOrder
    root = Util::createTree({3, 9, 20, -1, -1, 15, 7});
    expected = {{3}, {20, 9}, {15, 7}};
    assert(zigzagLevelOrderSolution(root) == expected);
    Util::deleteTree(root);
    std::cout << "  zigzagLevelOrderSolution: PASSED\n";

    // rightSideView
    root = Util::createTree({1, 2, 3, -1, 5, -1, 4});
    assert(rightSideViewSolution(root) == std::vector<int>({1, 3, 4}));
    Util::deleteTree(root);
    std::cout << "  rightSideViewSolution: PASSED\n";
}

} // namespace TreeTraversal
