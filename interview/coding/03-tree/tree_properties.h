/**
 * @file tree_properties.h
 * @brief 二叉树属性问题 - 题目说明与声明
 *
 * 树的属性问题包括高度、深度、节点数、对称性等基本性质的求解。
 */

#ifndef TREE_PROPERTIES_H
#define TREE_PROPERTIES_H

#include "../../util/Util.h"

namespace TreeProperties {

// ==================== 题目声明 ====================

/**
 * 题目1: 二叉树的最大深度
 */
int maxDepth(TreeNode* root);
int maxDepthSolution(TreeNode* root);

/**
 * 题目2: 二叉树的最小深度
 */
int minDepth(TreeNode* root);
int minDepthSolution(TreeNode* root);

/**
 * 题目3: 平衡二叉树
 */
bool isBalanced(TreeNode* root);
bool isBalancedSolution(TreeNode* root);

/**
 * 题目4: 对称二叉树
 */
bool isSymmetric(TreeNode* root);
bool isSymmetricSolution(TreeNode* root);

/**
 * 题目5: 相同的树
 */
bool isSameTree(TreeNode* p, TreeNode* q);
bool isSameTreeSolution(TreeNode* p, TreeNode* q);

/**
 * 题目6: 另一棵树的子树
 */
bool isSubtree(TreeNode* root, TreeNode* subRoot);
bool isSubtreeSolution(TreeNode* root, TreeNode* subRoot);

/**
 * 题目7: 翻转二叉树
 */
TreeNode* invertTree(TreeNode* root);
TreeNode* invertTreeSolution(TreeNode* root);

/**
 * 题目8: 合并二叉树
 */
TreeNode* mergeTrees(TreeNode* root1, TreeNode* root2);
TreeNode* mergeTreesSolution(TreeNode* root1, TreeNode* root2);

// ==================== 测试函数声明 ====================

void testTreeProperties();          // 测试面试者实现
void testTreePropertiesSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTreeProperties();
    testTreePropertiesSolution();
}

} // namespace TreeProperties

#endif // TREE_PROPERTIES_H
