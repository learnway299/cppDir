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
 *
 * 实现方式：
 * - 递归：最直观，空间复杂度 O(h)
 * - 迭代（栈）：模拟递归过程
 * - Morris：O(1) 空间，通过线索化临时修改树结构
 */
std::vector<int> preorderTraversal(TreeNode* root);           // 面试者实现
std::vector<int> preorderTraversalSolution(TreeNode* root);   // 参考答案（默认迭代）
std::vector<int> preorderTraversalRecursive(TreeNode* root);  // 递归实现
std::vector<int> preorderTraversalIterative(TreeNode* root);  // 迭代实现
std::vector<int> preorderTraversalMorris(TreeNode* root);     // Morris 遍历

/**
 * 题目2: 二叉树的中序遍历
 * 顺序：左 -> 根 -> 右
 *
 * 中序遍历 BST 可得到有序序列
 */
std::vector<int> inorderTraversal(TreeNode* root);
std::vector<int> inorderTraversalSolution(TreeNode* root);
std::vector<int> inorderTraversalRecursive(TreeNode* root);
std::vector<int> inorderTraversalIterative(TreeNode* root);
std::vector<int> inorderTraversalMorris(TreeNode* root);

/**
 * 题目3: 二叉树的后序遍历
 * 顺序：左 -> 右 -> 根
 *
 * 实现方式：
 * - 递归：最直观
 * - 迭代（反转法）：前序变体 根->右->左 然后反转
 * - 迭代（标记法）：记录上一个访问节点，不需要反转
 * - Morris：较复杂，需要逆序输出路径
 */
std::vector<int> postorderTraversal(TreeNode* root);
std::vector<int> postorderTraversalSolution(TreeNode* root);
std::vector<int> postorderTraversalRecursive(TreeNode* root);
std::vector<int> postorderTraversalIterative(TreeNode* root);   // 反转法
std::vector<int> postorderTraversalIterative2(TreeNode* root);  // 标记法
std::vector<int> postorderTraversalMorris(TreeNode* root);

/**
 * 题目4: 二叉树的层序遍历
 * 返回每层一个数组
 *
 * 实现方式：
 * - BFS（队列）：最常用
 * - DFS（递归）：利用深度参数
 */
std::vector<std::vector<int>> levelOrder(TreeNode* root);
std::vector<std::vector<int>> levelOrderSolution(TreeNode* root);
std::vector<std::vector<int>> levelOrderIterative(TreeNode* root);  // BFS
std::vector<std::vector<int>> levelOrderRecursive(TreeNode* root);  // DFS

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
