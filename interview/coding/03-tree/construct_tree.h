/**
 * @file construct_tree.h
 * @brief 构造二叉树问题 - 题目说明与声明
 *
 * 构造二叉树是面试高频题，需要理解遍历序列的特性来还原树的结构。
 */

#ifndef CONSTRUCT_TREE_H
#define CONSTRUCT_TREE_H

#include "../../util/Util.h"
#include <vector>
#include <string>

namespace ConstructTree {

// ==================== 题目声明 ====================

/**
 * 题目1: 从前序与中序遍历序列构造二叉树
 *
 * 前序遍历的第一个元素是根节点，在中序遍历中找到根节点，
 * 左边是左子树，右边是右子树，递归构造。
 */
TreeNode* buildTreeFromPreIn(std::vector<int>& preorder, std::vector<int>& inorder);
TreeNode* buildTreeFromPreInSolution(std::vector<int>& preorder, std::vector<int>& inorder);

/**
 * 题目2: 从中序与后序遍历序列构造二叉树
 *
 * 后序遍历的最后一个元素是根节点，其余类似题目1。
 */
TreeNode* buildTreeFromInPost(std::vector<int>& inorder, std::vector<int>& postorder);
TreeNode* buildTreeFromInPostSolution(std::vector<int>& inorder, std::vector<int>& postorder);

/**
 * 题目3: 根据前序和后序遍历构造二叉树
 *
 * 没有中序遍历，无法唯一确定树的结构，返回任意合法树即可。
 */
TreeNode* buildTreeFromPrePost(std::vector<int>& preorder, std::vector<int>& postorder);
TreeNode* buildTreeFromPrePostSolution(std::vector<int>& preorder, std::vector<int>& postorder);

/**
 * 题目4: 最大二叉树
 *
 * 找到数组最大值作为根，递归在左右子数组上构建左右子树。
 */
TreeNode* constructMaximumBinaryTree(std::vector<int>& nums);
TreeNode* constructMaximumBinaryTreeSolution(std::vector<int>& nums);

/**
 * 题目5: 有序链表转换二叉搜索树
 *
 * 将升序链表转换为高度平衡的BST。
 */
TreeNode* sortedListToBST(ListNode* head);
TreeNode* sortedListToBSTSolution(ListNode* head);

/**
 * 题目6: 有序数组转换为二叉搜索树
 *
 * 将升序数组转换为高度平衡的BST。
 */
TreeNode* sortedArrayToBST(std::vector<int>& nums);
TreeNode* sortedArrayToBSTSolution(std::vector<int>& nums);

/**
 * 题目7: 将二叉搜索树转化为排序的双向链表
 *
 * 就地转换BST为双向循环链表。
 */
TreeNode* treeToDoublyList(TreeNode* root);
TreeNode* treeToDoublyListSolution(TreeNode* root);

/**
 * 题目8: 从先序遍历还原二叉树
 *
 * 格式："1-2--3--4-5--6--7"，短划线数量表示深度。
 */
TreeNode* recoverFromPreorder(std::string traversal);
TreeNode* recoverFromPreorderSolution(std::string traversal);

// ==================== 测试函数声明 ====================

void testConstructTree();          // 测试面试者实现
void testConstructTreeSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testConstructTree();
    testConstructTreeSolution();
}

} // namespace ConstructTree

#endif // CONSTRUCT_TREE_H
