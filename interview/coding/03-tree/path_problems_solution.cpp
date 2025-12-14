/**
 * @file path_problems_solution.cpp
 * @brief 二叉树路径问题 - 参考答案
 */

#include "path_problems.h"
#include <iostream>
#include <cassert>
#include <climits>
#include <unordered_map>
#include <algorithm>

namespace PathProblems {

// ==================== 辅助函数 ====================

static void binaryTreePathsHelper(TreeNode* root, std::string path, std::vector<std::string>& result) {
    if (!root) return;
    path += std::to_string(root->val);
    if (!root->left && !root->right) {
        result.push_back(path);
        return;
    }
    path += "->";
    binaryTreePathsHelper(root->left, path, result);
    binaryTreePathsHelper(root->right, path, result);
}

static void pathSumHelper(TreeNode* root, int targetSum, std::vector<int>& path,
                          std::vector<std::vector<int>>& result) {
    if (!root) return;
    path.push_back(root->val);
    if (!root->left && !root->right && root->val == targetSum) {
        result.push_back(path);
    } else {
        pathSumHelper(root->left, targetSum - root->val, path, result);
        pathSumHelper(root->right, targetSum - root->val, path, result);
    }
    path.pop_back();
}

static int pathSumIIIHelper(TreeNode* root, long long currSum, long long targetSum,
                            std::unordered_map<long long, int>& prefixSumCount) {
    if (!root) return 0;
    currSum += root->val;
    int count = 0;
    if (prefixSumCount.count(currSum - targetSum)) {
        count = prefixSumCount[currSum - targetSum];
    }
    prefixSumCount[currSum]++;
    count += pathSumIIIHelper(root->left, currSum, targetSum, prefixSumCount);
    count += pathSumIIIHelper(root->right, currSum, targetSum, prefixSumCount);
    prefixSumCount[currSum]--;
    return count;
}

static int maxPathSumHelper(TreeNode* root, int& maxSum) {
    if (!root) return 0;
    int leftGain = std::max(0, maxPathSumHelper(root->left, maxSum));
    int rightGain = std::max(0, maxPathSumHelper(root->right, maxSum));
    int pathSum = root->val + leftGain + rightGain;
    maxSum = std::max(maxSum, pathSum);
    return root->val + std::max(leftGain, rightGain);
}

static void longestZigZagHelper(TreeNode* root, bool goLeft, int length, int& maxLen) {
    if (!root) return;
    maxLen = std::max(maxLen, length);
    if (goLeft) {
        longestZigZagHelper(root->left, false, length + 1, maxLen);
        longestZigZagHelper(root->right, true, 1, maxLen);
    } else {
        longestZigZagHelper(root->right, true, length + 1, maxLen);
        longestZigZagHelper(root->left, false, 1, maxLen);
    }
}

static int diameterHelper(TreeNode* root, int& maxDiameter) {
    if (!root) return 0;
    int leftDepth = diameterHelper(root->left, maxDiameter);
    int rightDepth = diameterHelper(root->right, maxDiameter);
    maxDiameter = std::max(maxDiameter, leftDepth + rightDepth);
    return 1 + std::max(leftDepth, rightDepth);
}

static int sumNumbersDFS(TreeNode* root, int currentNum) {
    if (!root) return 0;
    currentNum = currentNum * 10 + root->val;
    if (!root->left && !root->right) return currentNum;
    return sumNumbersDFS(root->left, currentNum) + sumNumbersDFS(root->right, currentNum);
}

// ==================== 参考答案实现 ====================

// 题目1: 二叉树的所有路径
std::vector<std::string> binaryTreePathsSolution(TreeNode* root) {
    std::vector<std::string> result;
    binaryTreePathsHelper(root, "", result);
    return result;
}

// 题目2: 路径总和
bool hasPathSumSolution(TreeNode* root, int targetSum) {
    if (!root) return false;
    if (!root->left && !root->right) return root->val == targetSum;
    int remaining = targetSum - root->val;
    return hasPathSumSolution(root->left, remaining) || hasPathSumSolution(root->right, remaining);
}

// 题目3: 路径总和 II
std::vector<std::vector<int>> pathSumSolution(TreeNode* root, int targetSum) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    pathSumHelper(root, targetSum, path, result);
    return result;
}

// 题目4: 路径总和 III
int pathSumIIISolution(TreeNode* root, int targetSum) {
    std::unordered_map<long long, int> prefixSumCount;
    prefixSumCount[0] = 1;
    return pathSumIIIHelper(root, 0, targetSum, prefixSumCount);
}

// 题目5: 二叉树的最大路径和
int maxPathSumSolution(TreeNode* root) {
    int maxSum = INT_MIN;
    maxPathSumHelper(root, maxSum);
    return maxSum;
}

// 题目6: 二叉树中的最长交替路径
int longestZigZagSolution(TreeNode* root) {
    if (!root) return 0;
    int maxLen = 0;
    longestZigZagHelper(root->left, false, 1, maxLen);
    longestZigZagHelper(root->right, true, 1, maxLen);
    return maxLen;
}

// 题目7: 二叉树的直径
int diameterOfBinaryTreeSolution(TreeNode* root) {
    int maxDiameter = 0;
    diameterHelper(root, maxDiameter);
    return maxDiameter;
}

// 题目8: 求根节点到叶节点数字之和
int sumNumbersSolution(TreeNode* root) {
    return sumNumbersDFS(root, 0);
}

// ==================== 测试函数 ====================

void testPathProblemsSolution() {
    std::cout << "=== Path Problems Tests (Solution) ===\n";

    // binaryTreePaths
    TreeNode* root = Util::createTree({1, 2, 3, -1, 5});
    auto paths = binaryTreePathsSolution(root);
    std::sort(paths.begin(), paths.end());
    std::vector<std::string> expected = {"1->2->5", "1->3"};
    std::sort(expected.begin(), expected.end());
    assert(paths == expected);
    Util::deleteTree(root);
    std::cout << "  binaryTreePathsSolution: PASSED\n";

    // hasPathSum
    root = Util::createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, -1, 1});
    assert(hasPathSumSolution(root, 22) == true);
    assert(hasPathSumSolution(root, 100) == false);
    Util::deleteTree(root);
    std::cout << "  hasPathSumSolution: PASSED\n";

    // pathSum
    root = Util::createTree({5, 4, 8, 11, -1, 13, 4, 7, 2, -1, -1, 5, 1});
    auto result = pathSumSolution(root, 22);
    assert(result.size() == 2);
    Util::deleteTree(root);
    std::cout << "  pathSumSolution: PASSED\n";

    // pathSumIII
    root = Util::createTree({10, 5, -3, 3, 2, -1, 11, 3, -2, -1, 1});
    assert(pathSumIIISolution(root, 8) == 3);
    Util::deleteTree(root);
    std::cout << "  pathSumIIISolution: PASSED\n";

    // maxPathSum
    root = Util::createTree({1, 2, 3});
    assert(maxPathSumSolution(root) == 6);
    Util::deleteTree(root);
    root = Util::createTree({-10, 9, 20, -1, -1, 15, 7});
    assert(maxPathSumSolution(root) == 42);
    Util::deleteTree(root);
    std::cout << "  maxPathSumSolution: PASSED\n";

    // diameterOfBinaryTree
    root = Util::createTree({1, 2, 3, 4, 5});
    assert(diameterOfBinaryTreeSolution(root) == 3);
    Util::deleteTree(root);
    std::cout << "  diameterOfBinaryTreeSolution: PASSED\n";

    // sumNumbers
    root = Util::createTree({1, 2, 3});
    assert(sumNumbersSolution(root) == 25);
    Util::deleteTree(root);
    root = Util::createTree({4, 9, 0, 5, 1});
    assert(sumNumbersSolution(root) == 1026);
    Util::deleteTree(root);
    std::cout << "  sumNumbersSolution: PASSED\n";
}

} // namespace PathProblems
