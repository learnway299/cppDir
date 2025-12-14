/**
 * @file bst_operations_solution.cpp
 * @brief 二叉搜索树操作 - 参考答案
 */

#include "bst_operations.h"
#include <iostream>
#include <cassert>
#include <climits>
#include <stack>

namespace BSTOperations {

// ==================== 辅助函数 ====================

static bool isValidBSTHelper(TreeNode* root, long long minVal, long long maxVal) {
    if (!root) return true;
    if (root->val <= minVal || root->val >= maxVal) return false;
    return isValidBSTHelper(root->left, minVal, root->val) &&
           isValidBSTHelper(root->right, root->val, maxVal);
}

static TreeNode* findMin(TreeNode* root) {
    while (root->left) root = root->left;
    return root;
}

static void kthSmallestHelper(TreeNode* root, int& k, int& result) {
    if (!root || k <= 0) return;
    kthSmallestHelper(root->left, k, result);
    --k;
    if (k == 0) { result = root->val; return; }
    kthSmallestHelper(root->right, k, result);
}

static TreeNode* sortedArrayToBSTHelper(std::vector<int>& nums, int left, int right) {
    if (left > right) return nullptr;
    int mid = left + (right - left) / 2;
    TreeNode* root = new TreeNode(nums[mid]);
    root->left = sortedArrayToBSTHelper(nums, left, mid - 1);
    root->right = sortedArrayToBSTHelper(nums, mid + 1, right);
    return root;
}

// ==================== 参考答案实现 ====================

// 题目1: 验证二叉搜索树
bool isValidBSTSolution(TreeNode* root) {
    return isValidBSTHelper(root, LLONG_MIN, LLONG_MAX);
}

// 题目2: 二叉搜索树中的搜索
TreeNode* searchBSTSolution(TreeNode* root, int val) {
    if (!root) return nullptr;
    if (val == root->val) return root;
    if (val < root->val) return searchBSTSolution(root->left, val);
    return searchBSTSolution(root->right, val);
}

// 题目3: 二叉搜索树中的插入操作
TreeNode* insertIntoBSTSolution(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);
    if (val < root->val) root->left = insertIntoBSTSolution(root->left, val);
    else root->right = insertIntoBSTSolution(root->right, val);
    return root;
}

// 题目4: 删除二叉搜索树中的节点
TreeNode* deleteNodeSolution(TreeNode* root, int key) {
    if (!root) return nullptr;
    if (key < root->val) {
        root->left = deleteNodeSolution(root->left, key);
    } else if (key > root->val) {
        root->right = deleteNodeSolution(root->right, key);
    } else {
        if (!root->left && !root->right) {
            delete root;
            return nullptr;
        }
        if (!root->left) {
            TreeNode* rightChild = root->right;
            delete root;
            return rightChild;
        }
        if (!root->right) {
            TreeNode* leftChild = root->left;
            delete root;
            return leftChild;
        }
        TreeNode* successor = findMin(root->right);
        root->val = successor->val;
        root->right = deleteNodeSolution(root->right, successor->val);
    }
    return root;
}

// 题目5: 二叉搜索树的最近公共祖先
TreeNode* lowestCommonAncestorBSTSolution(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return nullptr;
    if (p->val < root->val && q->val < root->val)
        return lowestCommonAncestorBSTSolution(root->left, p, q);
    if (p->val > root->val && q->val > root->val)
        return lowestCommonAncestorBSTSolution(root->right, p, q);
    return root;
}

// 题目6: 二叉搜索树中第K小的元素
int kthSmallestSolution(TreeNode* root, int k) {
    int result = 0;
    kthSmallestHelper(root, k, result);
    return result;
}

// 题目7: 将有序数组转换为二叉搜索树
TreeNode* sortedArrayToBSTSolution(std::vector<int>& nums) {
    return sortedArrayToBSTHelper(nums, 0, static_cast<int>(nums.size()) - 1);
}

// 题目8: 二叉搜索树的范围和
int rangeSumBSTSolution(TreeNode* root, int low, int high) {
    if (!root) return 0;
    if (root->val < low) return rangeSumBSTSolution(root->right, low, high);
    if (root->val > high) return rangeSumBSTSolution(root->left, low, high);
    return root->val + rangeSumBSTSolution(root->left, low, high) + rangeSumBSTSolution(root->right, low, high);
}

// ==================== 测试函数 ====================

void testBSTOperationsSolution() {
    std::cout << "=== BST Operations Tests (Solution) ===\n";

    // isValidBST
    TreeNode* root = Util::createTree({2, 1, 3});
    assert(isValidBSTSolution(root) == true);
    Util::deleteTree(root);
    std::cout << "  isValidBSTSolution: PASSED\n";

    // searchBST
    root = Util::createTree({4, 2, 7, 1, 3});
    TreeNode* result = searchBSTSolution(root, 2);
    assert(result != nullptr && result->val == 2);
    Util::deleteTree(root);
    std::cout << "  searchBSTSolution: PASSED\n";

    // insertIntoBST
    root = Util::createTree({4, 2, 7, 1, 3});
    root = insertIntoBSTSolution(root, 5);
    std::vector<int> inorderResult;
    Util::inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({1, 2, 3, 4, 5, 7}));
    Util::deleteTree(root);
    std::cout << "  insertIntoBSTSolution: PASSED\n";

    // kthSmallest
    root = Util::createTree({3, 1, 4, -1, 2});
    assert(kthSmallestSolution(root, 1) == 1);
    Util::deleteTree(root);
    std::cout << "  kthSmallestSolution: PASSED\n";

    // sortedArrayToBST
    std::vector<int> nums = {-10, -3, 0, 5, 9};
    root = sortedArrayToBSTSolution(nums);
    assert(isValidBSTSolution(root) == true);
    Util::deleteTree(root);
    std::cout << "  sortedArrayToBSTSolution: PASSED\n";

    // rangeSumBST
    root = Util::createTree({10, 5, 15, 3, 7, -1, 18});
    assert(rangeSumBSTSolution(root, 7, 15) == 32);
    Util::deleteTree(root);
    std::cout << "  rangeSumBSTSolution: PASSED\n";
}

} // namespace BSTOperations
