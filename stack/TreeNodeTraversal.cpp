//
// Created by learyuan on 2025/6/22.
//

#include "TreeNodeTraversal.h"
#include <stack>
#include <memory>

void TreeNodeTraversal::solution() {
    std::cout << "TreeNodeTraversal::solution" << std::endl;
    TreeNode *root = new TreeNode(0);
    root->left = new TreeNode(1);
    root->right = new TreeNode(2);
    root->left->left = new TreeNode(3);
    root->left->right = new TreeNode(4);
    std::vector<int> preorderResult = preorderTraversal(root);
    std::cout << "preorderTraversal" << std::endl;
    Util::printVector(preorderResult);
    std::vector<int> inorderResult = inorderTraversal(root);
    std::cout << "inorderTraversal" << std::endl;
    Util::printVector(inorderResult);
    std::vector<int> postorderResult = postorderTraversal(root);
    std::cout << "postorderTraversal" << std::endl;
    Util::printVector(postorderResult);
    delete root->left->right;
    delete root->left->left;
    delete root->right;
    delete root->left;
    delete root;
}

std::vector<int> TreeNodeTraversal::preorderTraversal(TreeNode *head) {
    std::vector<int> result;
    if (!head) {
        return result;
    }
    std::stack<TreeNode *> cache;
    cache.push(head);
    while (!cache.empty()) {
        TreeNode *node = cache.top();
        cache.pop();
        result.push_back(node->val);
        if (node->right) {
            cache.push(node->right);
        }
        if (node->left) {
            cache.push(node->left);
        }
    }
    return result;
}

std::vector<int> TreeNodeTraversal::inorderTraversal(TreeNode *head) {
    std::vector<int> result;
    if (!head) {
        return result;
    }
    std::stack<TreeNode *> cache;
    TreeNode *node = head;
    while (!cache.empty() || node) {
        while (node) {
            cache.push(node);
            node = node->left;
        }

        node = cache.top();
        cache.pop();
        result.push_back(node->val);

        node = node->right;
    }
    return result;
}

std::vector<int> TreeNodeTraversal::postorderTraversal(TreeNode *head) {
    std::vector<int> result;
    if (!head) {
        return result;
    }
    std::stack<TreeNode *> temp, cache;
    temp.push(head);
    TreeNode *node{nullptr};
    while (!temp.empty()) {
        node = temp.top();
        cache.push(node);
        temp.pop();

        if (node->left) {
            temp.push(node->left);
        }
        if (node->right) {
            temp.push(node->right);
        }
    }

    while (!cache.empty()) {
        result.push_back(cache.top()->val);
        cache.pop();
    }
    return result;
}
