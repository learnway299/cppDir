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

// ============================================================
// 题目1: 前序遍历 (根 -> 左 -> 右)
// ============================================================

/**
 * 前序遍历 - 递归实现
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(h) - h 为树高，递归栈深度
 */
namespace {
void preorderRecursive(TreeNode* node, std::vector<int>& result) {
    if (!node) return;
    result.push_back(node->val);      // 根
    preorderRecursive(node->left, result);   // 左
    preorderRecursive(node->right, result);  // 右
}
}

std::vector<int> preorderTraversalRecursive(TreeNode* root) {
    std::vector<int> result;
    preorderRecursive(root, result);
    return result;
}

/**
 * 前序遍历 - 迭代实现（栈模拟）
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(h) - 栈最多存储 h 个节点
 */
std::vector<int> preorderTraversalIterative(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;
    std::stack<TreeNode*> stk;
    stk.push(root);
    while (!stk.empty()) {
        TreeNode* node = stk.top();
        stk.pop();
        result.push_back(node->val);
        // 先压右再压左，出栈时就是先左后右
        if (node->right) stk.push(node->right);
        if (node->left) stk.push(node->left);
    }
    return result;
}

/**
 * 前序遍历 - Morris 遍历
 * 时间复杂度: O(n) - 每个节点最多访问两次
 * 空间复杂度: O(1) - 不使用栈，利用空闲指针
 */
std::vector<int> preorderTraversalMorris(TreeNode* root) {
    std::vector<int> result;
    TreeNode* curr = root;
    while (curr) {
        if (!curr->left) {
            // 无左子树，访问当前节点，走向右子树
            result.push_back(curr->val);
            curr = curr->right;
        } else {
            // 找左子树的最右节点（前驱）
            TreeNode* pred = curr->left;
            while (pred->right && pred->right != curr) {
                pred = pred->right;
            }
            if (!pred->right) {
                // 第一次访问，建立线索，访问当前节点
                result.push_back(curr->val);
                pred->right = curr;
                curr = curr->left;
            } else {
                // 第二次访问，断开线索
                pred->right = nullptr;
                curr = curr->right;
            }
        }
    }
    return result;
}

// Solution 默认使用迭代实现
std::vector<int> preorderTraversalSolution(TreeNode* root) {
    return preorderTraversalIterative(root);
}

// ============================================================
// 题目2: 中序遍历 (左 -> 根 -> 右)
// ============================================================

/**
 * 中序遍历 - 递归实现
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(h) - 递归栈深度
 */
namespace {
void inorderRecursive(TreeNode* node, std::vector<int>& result) {
    if (!node) return;
    inorderRecursive(node->left, result);    // 左
    result.push_back(node->val);             // 根
    inorderRecursive(node->right, result);   // 右
}
}

std::vector<int> inorderTraversalRecursive(TreeNode* root) {
    std::vector<int> result;
    inorderRecursive(root, result);
    return result;
}

/**
 * 中序遍历 - 迭代实现（栈模拟）
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(h) - 栈最多存储 h 个节点
 */
std::vector<int> inorderTraversalIterative(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> stk;
    TreeNode* curr = root;
    while (curr || !stk.empty()) {
        // 一路向左，全部入栈
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }
        // 弹出栈顶，访问
        curr = stk.top();
        stk.pop();
        result.push_back(curr->val);
        // 转向右子树
        curr = curr->right;
    }
    return result;
}

/**
 * 中序遍历 - Morris 遍历
 * 时间复杂度: O(n) - 每个节点最多访问两次
 * 空间复杂度: O(1) - 不使用栈
 */
std::vector<int> inorderTraversalMorris(TreeNode* root) {
    std::vector<int> result;
    TreeNode* curr = root;
    while (curr) {
        if (!curr->left) {
            result.push_back(curr->val);
            curr = curr->right;
        } else {
            TreeNode* pred = curr->left;
            while (pred->right && pred->right != curr) {
                pred = pred->right;
            }
            if (!pred->right) {
                pred->right = curr;
                curr = curr->left;
            } else {
                pred->right = nullptr;
                result.push_back(curr->val);  // 中序在第二次访问时输出
                curr = curr->right;
            }
        }
    }
    return result;
}

std::vector<int> inorderTraversalSolution(TreeNode* root) {
    return inorderTraversalIterative(root);
}

// ============================================================
// 题目3: 后序遍历 (左 -> 右 -> 根)
// ============================================================

/**
 * 后序遍历 - 递归实现
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(h) - 递归栈深度
 */
namespace {
void postorderRecursive(TreeNode* node, std::vector<int>& result) {
    if (!node) return;
    postorderRecursive(node->left, result);   // 左
    postorderRecursive(node->right, result);  // 右
    result.push_back(node->val);              // 根
}
}

std::vector<int> postorderTraversalRecursive(TreeNode* root) {
    std::vector<int> result;
    postorderRecursive(root, result);
    return result;
}

/**
 * 后序遍历 - 迭代实现（反转法）
 * 时间复杂度: O(n) - 访问 + 反转
 * 空间复杂度: O(n) - 栈 + 结果数组
 */
std::vector<int> postorderTraversalIterative(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;
    std::stack<TreeNode*> stk;
    stk.push(root);
    while (!stk.empty()) {
        TreeNode* node = stk.top();
        stk.pop();
        result.push_back(node->val);
        // 先压左再压右，出栈顺序为 根->右->左
        if (node->left) stk.push(node->left);
        if (node->right) stk.push(node->right);
    }
    // 反转得到 左->右->根
    std::reverse(result.begin(), result.end());
    return result;
}

/**
 * 后序遍历 - 迭代实现（标记法）
 * 时间复杂度: O(n) - 访问每个节点
 * 空间复杂度: O(h) - 栈深度
 */
std::vector<int> postorderTraversalIterative2(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;
    std::stack<TreeNode*> stk;
    TreeNode* prev = nullptr;  // 记录上一个访问的节点
    TreeNode* curr = root;

    while (curr || !stk.empty()) {
        // 一路向左
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }
        curr = stk.top();
        // 如果右子树为空或已访问过，则访问当前节点
        if (!curr->right || curr->right == prev) {
            result.push_back(curr->val);
            stk.pop();
            prev = curr;
            curr = nullptr;  // 重要：置空防止重复入栈
        } else {
            // 否则转向右子树
            curr = curr->right;
        }
    }
    return result;
}

/**
 * 后序遍历 - Morris 遍历
 * 时间复杂度: O(n) - 每个节点最多访问两次
 * 空间复杂度: O(1) - 不使用栈
 */
std::vector<int> postorderTraversalMorris(TreeNode* root) {
    std::vector<int> result;
    TreeNode dummy(0);
    dummy.left = root;
    TreeNode* curr = &dummy;

    auto reverseOutput = [&result](TreeNode* from, TreeNode* to) {
        std::vector<int> temp;
        while (from != to) {
            temp.push_back(from->val);
            from = from->right;
        }
        temp.push_back(to->val);
        for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
            result.push_back(*it);
        }
    };

    while (curr) {
        if (!curr->left) {
            curr = curr->right;
        } else {
            TreeNode* pred = curr->left;
            while (pred->right && pred->right != curr) {
                pred = pred->right;
            }
            if (!pred->right) {
                pred->right = curr;
                curr = curr->left;
            } else {
                reverseOutput(curr->left, pred);
                pred->right = nullptr;
                curr = curr->right;
            }
        }
    }
    return result;
}

std::vector<int> postorderTraversalSolution(TreeNode* root) {
    return postorderTraversalIterative(root);
}

// ============================================================
// 题目4: 层序遍历
// ============================================================

/**
 * 层序遍历 - BFS 迭代（队列）
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(n) - 最坏情况最后一层有 n/2 个节点
 */
std::vector<std::vector<int>> levelOrderIterative(TreeNode* root) {
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

/**
 * 层序遍历 - DFS 递归
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(h) - 递归栈深度
 */
namespace {
void levelOrderDFS(TreeNode* node, int depth, std::vector<std::vector<int>>& result) {
    if (!node) return;
    // 如果当前层还没有创建，先创建
    if (depth >= static_cast<int>(result.size())) {
        result.push_back({});
    }
    result[depth].push_back(node->val);
    levelOrderDFS(node->left, depth + 1, result);
    levelOrderDFS(node->right, depth + 1, result);
}
}

std::vector<std::vector<int>> levelOrderRecursive(TreeNode* root) {
    std::vector<std::vector<int>> result;
    levelOrderDFS(root, 0, result);
    return result;
}

std::vector<std::vector<int>> levelOrderSolution(TreeNode* root) {
    return levelOrderIterative(root);
}

/**
 * 题目5: 锯齿形层序遍历 - BFS + 方向交替
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(n) - 队列最多存储一层节点
 */
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

/**
 * 题目6: 二叉树的右视图 - BFS 取每层最后一个
 * 时间复杂度: O(n) - 访问每个节点一次
 * 空间复杂度: O(n) - 队列最多存储一层节点
 */
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
