//
// Created by learyuan on 25-6-8.
//

#include "MergeSort.h"
#include "../util/Util.h"

namespace MergeSort {
    void solution() {
        std::cout << "MergeSort::solution::mergeSortedVectors" << std::endl;

        std::vector<int> vecN = {1, 3, 5, 7, 9};
        std::vector<int> vecM = {2, 3, 4, 6, 9};
        Util::prntVec(vecN);
        Util::prntVec(vecM);
        // 调整 vecN 的大小以容纳合并后的结果
        vecN.reserve(vecN.size() + vecM.size());

        mergeSortedVectors(vecM, vecN);
        Util::prntVec(vecN);

        std::cout << "MergeSort::solution::mergeSortList" << std::endl;
        ListNode* list = Util::creatList({1,3,2,4,6,5,7,8});
        Util::printList(list);
        list = mergeSortList(list);
        Util::printList(list);
        Util::deleteList(list);
    }

    void mergeSortedVectors(std::vector<int> &vecM, std::vector<int> &vecN) {
        vecN.resize(vecN.size() + vecM.size());
        int nIndex = vecN.size() - vecM.size() - 1;
        int mIndex = vecM.size() - 1;
        int curIndex = vecN.size() - 1;
        while (mIndex >= 0 && nIndex >= 0) {
            if (vecM[mIndex] > vecN[nIndex]) {
                vecN[curIndex--] = vecM[mIndex--];
            } else {
                vecN[curIndex--] = vecN[nIndex--];
            }
        }
        while (mIndex >= 0) {
            vecN[curIndex--] = vecM[mIndex--];
        }
    }

    ListNode *findMid(ListNode *head) {
        ListNode *slow = head, *fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }

    ListNode *mergeList(ListNode *listA, ListNode *listB) {
        if (!listA) {
            return listB;
        }
        if (!listB) {
            return listA;
        }
        ListNode dummy(0);
        ListNode *head = &dummy;
        while (listA && listB) {
            if (listA->val < listB->val) {
                head->next = listA;
                listA = listA->next;
            } else {
                head->next = listB;
                listB = listB->next;
            }
            head = head->next;
        }
        head->next = listA ? listA : listB;
        return dummy.next;
    }

    ListNode *mergeSortList(ListNode *head) {
        if (!head || !head->next) {
            return head;
        }
        ListNode *midNode = findMid(head);
        ListNode *nextNode = midNode->next;
        midNode->next = nullptr;

         ListNode *leftNode = mergeSortList(head);
        ListNode *rightNode = mergeSortList(nextNode);
        return mergeList(leftNode, rightNode);
    }
} // MergeSort
