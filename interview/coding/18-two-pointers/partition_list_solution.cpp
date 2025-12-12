/**
 * @file partition_list_solution.cpp
 * @brief 分隔链表 - 解答
 */
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* partition(ListNode* head, int x) {
    ListNode smallDummy(0), largeDummy(0);
    ListNode* small = &smallDummy;
    ListNode* large = &largeDummy;

    while (head) {
        if (head->val < x) {
            small->next = head;
            small = small->next;
        } else {
            large->next = head;
            large = large->next;
        }
        head = head->next;
    }
    large->next = nullptr;
    small->next = largeDummy.next;
    return smallDummy.next;
}

int main() {
    // 创建链表 1->4->3->2->5->2
    ListNode* head = new ListNode(1);
    head->next = new ListNode(4);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(2);
    head->next->next->next->next = new ListNode(5);
    head->next->next->next->next->next = new ListNode(2);

    cout << "Before: ";
    for (ListNode* p = head; p; p = p->next) cout << p->val << " ";

    head = partition(head, 3);

    cout << "\nAfter (x=3): ";
    for (ListNode* p = head; p; p = p->next) cout << p->val << " ";
    cout << "\n";
    return 0;
}
