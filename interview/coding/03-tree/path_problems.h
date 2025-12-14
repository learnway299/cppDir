/**
 * @file path_problems.h
 * @brief 二叉树路径问题 - 题目说明与声明
 *
 * 路径问题是树问题中的高频考点，涉及路径查找、路径求和等。
 * 注意区分"根到叶子路径"和"任意节点间路径"的不同。
 */

#ifndef PATH_PROBLEMS_H
#define PATH_PROBLEMS_H

#include "../../util/Util.h"
#include <vector>
#include <string>

namespace PathProblems {

// ==================== 题目声明 ====================

/**
 * 题目1: 二叉树的所有路径
 *
 * 返回所有从根节点到叶子节点的路径。
 * 示例: root = [1,2,3,null,5] -> ["1->2->5","1->3"]
 */
std::vector<std::string> binaryTreePaths(TreeNode* root);
std::vector<std::string> binaryTreePathsSolution(TreeNode* root);

/**
 * 题目2: 路径总和
 *
 * 判断是否存在根到叶子的路径，使路径和等于 targetSum。
 */
bool hasPathSum(TreeNode* root, int targetSum);
bool hasPathSumSolution(TreeNode* root, int targetSum);

/**
 * 题目3: 路径总和 II
 *
 * 找出所有从根到叶子、路径和等于 targetSum 的路径。
 */
std::vector<std::vector<int>> pathSum(TreeNode* root, int targetSum);
std::vector<std::vector<int>> pathSumSolution(TreeNode* root, int targetSum);

/**
 * 题目4: 路径总和 III
 *
 * 路径不需要从根开始，也不需要在叶子结束，但方向必须向下。
 * 求节点值之和等于 targetSum 的路径数目。
 */
int pathSumIII(TreeNode* root, int targetSum);
int pathSumIIISolution(TreeNode* root, int targetSum);

/**
 * 题目5: 二叉树的最大路径和
 *
 * 路径可以从任意节点出发到任意节点，求最大路径和。
 */
int maxPathSum(TreeNode* root);
int maxPathSumSolution(TreeNode* root);

/**
 * 题目6: 二叉树中的最长交替路径
 *
 * 轮流选择左孩子和右孩子，返回最长交替路径长度。
 */
int longestZigZag(TreeNode* root);
int longestZigZagSolution(TreeNode* root);

/**
 * 题目7: 二叉树的直径
 *
 * 任意两结点路径长度的最大值（边数）。
 */
int diameterOfBinaryTree(TreeNode* root);
int diameterOfBinaryTreeSolution(TreeNode* root);

/**
 * 题目8: 求根节点到叶节点数字之和
 *
 * 每条路径代表一个数字，计算所有数字之和。
 * 示例: root = [1,2,3] -> 12 + 13 = 25
 */
int sumNumbers(TreeNode* root);
int sumNumbersSolution(TreeNode* root);

// ==================== 测试函数声明 ====================

void testPathProblems();          // 测试面试者实现
void testPathProblemsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testPathProblems();
    testPathProblemsSolution();
}

} // namespace PathProblems

#endif // PATH_PROBLEMS_H
