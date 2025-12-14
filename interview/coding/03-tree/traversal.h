/**
 * @file traversal.h
 * @brief 二叉树遍历问题 - 题目说明与声明
 *
 * 树的遍历是最基础的树问题，需要掌握递归和迭代两种实现方式。
 */

#ifndef TREE_TRAVERSAL_H
#define TREE_TRAVERSAL_H

#include "../../util/Util.h"
#include <vector>

namespace TreeTraversal {

// ==================== 题目声明 ====================

/**
 * 题目1: 二叉树的前序遍历
 * 顺序：根 -> 左 -> 右
 */
std::vector<int> preorderTraversal(TreeNode* root);
std::vector<int> preorderTraversalSolution(TreeNode* root);

/**
 * 题目2: 二叉树的中序遍历
 * 顺序：左 -> 根 -> 右
 */
std::vector<int> inorderTraversal(TreeNode* root);
std::vector<int> inorderTraversalSolution(TreeNode* root);

/**
 * 题目3: 二叉树的后序遍历
 * 顺序：左 -> 右 -> 根
 */
std::vector<int> postorderTraversal(TreeNode* root);
std::vector<int> postorderTraversalSolution(TreeNode* root);

/**
 * 题目4: 二叉树的层序遍历
 * 返回每层一个数组
 */
std::vector<std::vector<int>> levelOrder(TreeNode* root);
std::vector<std::vector<int>> levelOrderSolution(TreeNode* root);

/**
 * 题目5: 二叉树的锯齿形层序遍历
 * 奇数层从左到右，偶数层从右到左
 */
std::vector<std::vector<int>> zigzagLevelOrder(TreeNode* root);
std::vector<std::vector<int>> zigzagLevelOrderSolution(TreeNode* root);

/**
 * 题目6: 二叉树的右视图
 * 从右侧看到的节点
 */
std::vector<int> rightSideView(TreeNode* root);
std::vector<int> rightSideViewSolution(TreeNode* root);

// ==================== 测试函数声明 ====================

void testTreeTraversal();          // 测试面试者实现
void testTreeTraversalSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTreeTraversal();
    testTreeTraversalSolution();
}

} // namespace TreeTraversal

#endif // TREE_TRAVERSAL_H
