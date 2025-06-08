//
// Created by learyuan on 25-6-8.
//

#include "Util.h"
#include <memory>

namespace Util {
    void prntVec(const std::vector<int> &input) {
        for (const auto& item : input) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }

    void printList(ListNode *head) {
        while (head) {
            std::cout << head->val << " ";
            head = head->next;
        }
        std::cout << std::endl;
    }

    ListNode* creatList(const std::vector<int> &input) {
        if (input.empty()) {
            return nullptr;
        }

        ListNode dummy(0);
        ListNode* head = &dummy;

        for (int num : input) {
            head->next = new ListNode(num);
            head = head->next;
        }

        return dummy.next;
    }

    void deleteList(ListNode *head) {
        while (head) {
            ListNode* delNode = head;
            head = head->next;
            delete delNode;
        }
    }

    void swap(int &a, int &b) noexcept {
        if (&a != &b) {
            a = a ^ b;
            b = a ^ b;
            a = a ^ b;
        }
    }
} // Util