/**
 * @file lca.h
 * @brief 最近公共祖先问题 - 题目说明与声明
 *
 * LCA (Lowest Common Ancestor) 是树问题中的经典问题，
 * 在普通二叉树和二叉搜索树中有不同的解法。
 */

#ifndef LCA_H
#define LCA_H

#include "../../util/Util.h"
#include <vector>

namespace LCA {

// ==================== 题目声明 ====================

/**
 * 题目1: 二叉树的最近公共祖先
 *
 * 给定一个二叉树，找到该树中两个指定节点的最近公共祖先。
 * 一个节点也可以是它自己的祖先。
 *
 * 示例:
 *   输入: root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 1
 *   输出: 3
 */
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q);
TreeNode* lowestCommonAncestorSolution(TreeNode* root, TreeNode* p, TreeNode* q);

/**
 * 题目2: 二叉搜索树的最近公共祖先
 *
 * 利用BST性质简化查找：
 *   - 如果 p, q 都小于 root，则 LCA 在左子树
 *   - 如果 p, q 都大于 root，则 LCA 在右子树
 *   - 否则 root 就是 LCA（分叉点）
 */
TreeNode* lowestCommonAncestorBST(TreeNode* root, TreeNode* p, TreeNode* q);
TreeNode* lowestCommonAncestorBSTSolution(TreeNode* root, TreeNode* p, TreeNode* q);

/**
 * 题目3: 有父指针的二叉树的最近公共祖先
 *
 * 每个节点都包含其父节点的引用。
 * 思路：类似于两个链表找交点问题。
 */
ParentNode* lowestCommonAncestorWithParent(ParentNode* p, ParentNode* q);
ParentNode* lowestCommonAncestorWithParentSolution(ParentNode* p, ParentNode* q);

/**
 * 题目4: 二叉树的最近公共祖先 II
 *
 * 与题目1的区别：p 或 q 可能不在树中，此时返回 nullptr。
 */
TreeNode* lowestCommonAncestorII(TreeNode* root, TreeNode* p, TreeNode* q);
TreeNode* lowestCommonAncestorIISolution(TreeNode* root, TreeNode* p, TreeNode* q);

/**
 * 题目5: 二叉树的最近公共祖先 III（多个节点）
 *
 * 给定一棵二叉树的根节点 root 和一个节点数组 nodes，
 * 返回 nodes 中所有节点的最近公共祖先。
 */
TreeNode* lowestCommonAncestorMultiple(TreeNode* root, std::vector<TreeNode*>& nodes);
TreeNode* lowestCommonAncestorMultipleSolution(TreeNode* root, std::vector<TreeNode*>& nodes);

/**
 * 题目6: 二叉树的最深叶节点的最近公共祖先
 *
 * 返回最深的叶节点的最近公共祖先。
 */
TreeNode* lcaDeepestLeaves(TreeNode* root);
TreeNode* lcaDeepestLeavesSolution(TreeNode* root);

// ==================== 测试函数声明 ====================

void testLCA();          // 测试面试者实现
void testLCASolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testLCA();
    testLCASolution();
}

} // namespace LCA

#endif // LCA_H
