//
// Created by learyuan on 25-6-8.
//
#ifndef UTIL_H
#define UTIL_H
#include <vector>
#include <iostream>
using namespace std;

struct ListNode {
    ListNode(int data): val(data), next(nullptr) {
    }

    int val{0};
    ListNode *next{nullptr};
};

struct TreeNode {
    TreeNode(int data): val(data), left(nullptr), right(nullptr) {
    }
    int val{0};
    TreeNode * left{nullptr};
    TreeNode * right{nullptr};
};

namespace Util {
    void prntVec(const std::vector<int> &input);
    void printList(ListNode *head);
    ListNode*  creatList(const std::vector<int> &input);
    void deleteList(ListNode* head);
    void swap(int &a, int &b) noexcept;
} // Util

#endif //UTIL_H
