/**
 * @file construct_tree_solution.cpp
 * @brief 构造二叉树问题 - 参考答案
 */

#include "construct_tree.h"
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <stack>

namespace ConstructTree {

// ==================== 辅助函数 ====================

static TreeNode* buildFromPreInHelper(std::vector<int>& preorder, int preStart, int preEnd,
                                      int inStart, std::unordered_map<int, int>& inorderMap) {
    if (preStart > preEnd) return nullptr;
    int rootVal = preorder[preStart];
    TreeNode* root = new TreeNode(rootVal);
    int rootIdx = inorderMap[rootVal];
    int leftSize = rootIdx - inStart;
    root->left = buildFromPreInHelper(preorder, preStart + 1, preStart + leftSize,
                                      inStart, inorderMap);
    root->right = buildFromPreInHelper(preorder, preStart + leftSize + 1, preEnd,
                                       rootIdx + 1, inorderMap);
    return root;
}

static TreeNode* buildFromInPostHelper(std::vector<int>& postorder, int postStart, int postEnd,
                                       int inStart, std::unordered_map<int, int>& inorderMap) {
    if (postStart > postEnd) return nullptr;
    int rootVal = postorder[postEnd];
    TreeNode* root = new TreeNode(rootVal);
    int rootIdx = inorderMap[rootVal];
    int leftSize = rootIdx - inStart;
    root->left = buildFromInPostHelper(postorder, postStart, postStart + leftSize - 1,
                                       inStart, inorderMap);
    root->right = buildFromInPostHelper(postorder, postStart + leftSize, postEnd - 1,
                                        rootIdx + 1, inorderMap);
    return root;
}

static TreeNode* buildFromPrePostHelper(std::vector<int>& preorder, int preStart, int preEnd,
                                        std::vector<int>& postorder, int postStart, int postEnd,
                                        std::unordered_map<int, int>& postMap) {
    if (preStart > preEnd) return nullptr;
    TreeNode* root = new TreeNode(preorder[preStart]);
    if (preStart == preEnd) return root;
    int leftRootVal = preorder[preStart + 1];
    int leftRootIdx = postMap[leftRootVal];
    int leftSize = leftRootIdx - postStart + 1;
    root->left = buildFromPrePostHelper(preorder, preStart + 1, preStart + leftSize,
                                        postorder, postStart, leftRootIdx, postMap);
    root->right = buildFromPrePostHelper(preorder, preStart + leftSize + 1, preEnd,
                                         postorder, leftRootIdx + 1, postEnd - 1, postMap);
    return root;
}

static TreeNode* constructMaxHelper(std::vector<int>& nums, int left, int right) {
    if (left > right) return nullptr;
    int maxIdx = left;
    for (int i = left + 1; i <= right; ++i) {
        if (nums[i] > nums[maxIdx]) maxIdx = i;
    }
    TreeNode* root = new TreeNode(nums[maxIdx]);
    root->left = constructMaxHelper(nums, left, maxIdx - 1);
    root->right = constructMaxHelper(nums, maxIdx + 1, right);
    return root;
}

static TreeNode* buildBSTFromArray(std::vector<int>& nums, int left, int right) {
    if (left > right) return nullptr;
    int mid = left + (right - left) / 2;
    TreeNode* root = new TreeNode(nums[mid]);
    root->left = buildBSTFromArray(nums, left, mid - 1);
    root->right = buildBSTFromArray(nums, mid + 1, right);
    return root;
}

static TreeNode* head_ = nullptr;
static TreeNode* tail_ = nullptr;

static void treeToDoublyListHelper(TreeNode* root) {
    if (!root) return;
    treeToDoublyListHelper(root->left);
    if (tail_) {
        tail_->right = root;
        root->left = tail_;
    } else {
        head_ = root;
    }
    tail_ = root;
    treeToDoublyListHelper(root->right);
}

static void preorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    result.push_back(root->val);
    preorder(root->left, result);
    preorder(root->right, result);
}

// ==================== 参考答案实现 ====================

// 题目1: 从前序与中序遍历序列构造二叉树
TreeNode* buildTreeFromPreInSolution(std::vector<int>& preorder, std::vector<int>& inorder) {
    if (preorder.empty()) return nullptr;
    std::unordered_map<int, int> inorderMap;
    for (int i = 0; i < static_cast<int>(inorder.size()); ++i) {
        inorderMap[inorder[i]] = i;
    }
    return buildFromPreInHelper(preorder, 0, static_cast<int>(preorder.size()) - 1, 0, inorderMap);
}

// 题目2: 从中序与后序遍历序列构造二叉树
TreeNode* buildTreeFromInPostSolution(std::vector<int>& inorder, std::vector<int>& postorder) {
    if (inorder.empty()) return nullptr;
    std::unordered_map<int, int> inorderMap;
    for (int i = 0; i < static_cast<int>(inorder.size()); ++i) {
        inorderMap[inorder[i]] = i;
    }
    return buildFromInPostHelper(postorder, 0, static_cast<int>(postorder.size()) - 1, 0, inorderMap);
}

// 题目3: 根据前序和后序遍历构造二叉树
TreeNode* buildTreeFromPrePostSolution(std::vector<int>& preorder, std::vector<int>& postorder) {
    if (preorder.empty()) return nullptr;
    std::unordered_map<int, int> postMap;
    for (int i = 0; i < static_cast<int>(postorder.size()); ++i) {
        postMap[postorder[i]] = i;
    }
    return buildFromPrePostHelper(preorder, 0, static_cast<int>(preorder.size()) - 1,
                                  postorder, 0, static_cast<int>(postorder.size()) - 1, postMap);
}

// 题目4: 最大二叉树
TreeNode* constructMaximumBinaryTreeSolution(std::vector<int>& nums) {
    return constructMaxHelper(nums, 0, static_cast<int>(nums.size()) - 1);
}

// 题目5: 有序链表转换二叉搜索树
TreeNode* sortedListToBSTSolution(ListNode* head) {
    if (!head) return nullptr;
    if (!head->next) return new TreeNode(head->val);
    ListNode* slow = head;
    ListNode* fast = head;
    ListNode* prev = nullptr;
    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }
    if (prev) prev->next = nullptr;
    TreeNode* root = new TreeNode(slow->val);
    root->left = sortedListToBSTSolution(head == slow ? nullptr : head);
    root->right = sortedListToBSTSolution(slow->next);
    return root;
}

// 题目6: 有序数组转换为二叉搜索树
TreeNode* sortedArrayToBSTSolution(std::vector<int>& nums) {
    return buildBSTFromArray(nums, 0, static_cast<int>(nums.size()) - 1);
}

// 题目7: 将二叉搜索树转化为排序的双向链表
TreeNode* treeToDoublyListSolution(TreeNode* root) {
    if (!root) return nullptr;
    head_ = nullptr;
    tail_ = nullptr;
    treeToDoublyListHelper(root);
    head_->left = tail_;
    tail_->right = head_;
    return head_;
}

// 题目8: 从先序遍历还原二叉树
TreeNode* recoverFromPreorderSolution(std::string traversal) {
    if (traversal.empty()) return nullptr;
    std::stack<TreeNode*> stk;
    int i = 0;
    int n = static_cast<int>(traversal.size());
    while (i < n) {
        int depth = 0;
        while (i < n && traversal[i] == '-') { ++depth; ++i; }
        int val = 0;
        while (i < n && traversal[i] != '-') {
            val = val * 10 + (traversal[i] - '0');
            ++i;
        }
        TreeNode* node = new TreeNode(val);
        while (static_cast<int>(stk.size()) > depth) stk.pop();
        if (!stk.empty()) {
            if (!stk.top()->left) stk.top()->left = node;
            else stk.top()->right = node;
        }
        stk.push(node);
    }
    while (stk.size() > 1) stk.pop();
    return stk.top();
}

// ==================== 测试函数 ====================

void testConstructTreeSolution() {
    std::cout << "=== Construct Tree Tests (Solution) ===\n";

    // buildTreeFromPreIn
    std::vector<int> pre = {3, 9, 20, 15, 7};
    std::vector<int> in = {9, 3, 15, 20, 7};
    TreeNode* root = buildTreeFromPreInSolution(pre, in);
    std::vector<int> preResult, inResult;
    preorder(root, preResult);
    Util::inorder(root, inResult);
    assert(preResult == pre);
    assert(inResult == in);
    Util::deleteTree(root);
    std::cout << "  buildTreeFromPreInSolution: PASSED\n";

    // buildTreeFromInPost
    std::vector<int> post = {9, 15, 7, 20, 3};
    root = buildTreeFromInPostSolution(in, post);
    inResult.clear();
    Util::inorder(root, inResult);
    assert(inResult == in);
    Util::deleteTree(root);
    std::cout << "  buildTreeFromInPostSolution: PASSED\n";

    // constructMaximumBinaryTree
    std::vector<int> nums = {3, 2, 1, 6, 0, 5};
    root = constructMaximumBinaryTreeSolution(nums);
    assert(root != nullptr && root->val == 6);
    assert(root->left->val == 3);
    assert(root->right->val == 5);
    Util::deleteTree(root);
    std::cout << "  constructMaximumBinaryTreeSolution: PASSED\n";

    // sortedArrayToBST
    std::vector<int> sortedNums = {-10, -3, 0, 5, 9};
    root = sortedArrayToBSTSolution(sortedNums);
    inResult.clear();
    Util::inorder(root, inResult);
    assert(inResult == sortedNums);
    Util::deleteTree(root);
    std::cout << "  sortedArrayToBSTSolution: PASSED\n";

    // sortedListToBST
    ListNode* head = Util::createList({-10, -3, 0, 5, 9});
    root = sortedListToBSTSolution(head);
    inResult.clear();
    Util::inorder(root, inResult);
    assert((inResult == std::vector<int>{-10, -3, 0, 5, 9}));
    Util::deleteTree(root);
    std::cout << "  sortedListToBSTSolution: PASSED\n";

    // recoverFromPreorder
    std::string traversal = "1-2--3--4-5--6--7";
    root = recoverFromPreorderSolution(traversal);
    assert(root->val == 1);
    assert(root->left->val == 2);
    assert(root->right->val == 5);
    Util::deleteTree(root);
    std::cout << "  recoverFromPreorderSolution: PASSED\n";
}

} // namespace ConstructTree
