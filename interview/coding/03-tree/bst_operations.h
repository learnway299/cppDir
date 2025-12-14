/**
 * @file bst_operations.h
 * @brief 二叉搜索树操作 - 题目说明与声明
 *
 * BST 的核心性质：左子树所有节点 < 根 < 右子树所有节点
 */

#ifndef BST_OPERATIONS_H
#define BST_OPERATIONS_H

#include "../../util/Util.h"
#include <vector>

namespace BSTOperations {

// ==================== 题目声明 ====================

/**
 * 题目1: 验证二叉搜索树
 */
bool isValidBST(TreeNode* root);
bool isValidBSTSolution(TreeNode* root);

/**
 * 题目2: 二叉搜索树中的搜索
 */
TreeNode* searchBST(TreeNode* root, int val);
TreeNode* searchBSTSolution(TreeNode* root, int val);

/**
 * 题目3: 二叉搜索树中的插入操作
 */
TreeNode* insertIntoBST(TreeNode* root, int val);
TreeNode* insertIntoBSTSolution(TreeNode* root, int val);

/**
 * 题目4: 删除二叉搜索树中的节点
 */
TreeNode* deleteNode(TreeNode* root, int key);
TreeNode* deleteNodeSolution(TreeNode* root, int key);

/**
 * 题目5: 二叉搜索树的最近公共祖先
 */
TreeNode* lowestCommonAncestorBST(TreeNode* root, TreeNode* p, TreeNode* q);
TreeNode* lowestCommonAncestorBSTSolution(TreeNode* root, TreeNode* p, TreeNode* q);

/**
 * 题目6: 二叉搜索树中第K小的元素
 */
int kthSmallest(TreeNode* root, int k);
int kthSmallestSolution(TreeNode* root, int k);

/**
 * 题目7: 将有序数组转换为二叉搜索树
 */
TreeNode* sortedArrayToBST(std::vector<int>& nums);
TreeNode* sortedArrayToBSTSolution(std::vector<int>& nums);

/**
 * 题目8: 二叉搜索树的范围和
 */
int rangeSumBST(TreeNode* root, int low, int high);
int rangeSumBSTSolution(TreeNode* root, int low, int high);

// ==================== 测试函数声明 ====================

void testBSTOperations();          // 测试面试者实现
void testBSTOperationsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testBSTOperations();
    testBSTOperationsSolution();
}

} // namespace BSTOperations

#endif // BST_OPERATIONS_H
