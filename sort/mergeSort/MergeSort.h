//
// Created by learyuan on 25-6-8.
//

#ifndef MERGESORT_H
#define MERGESORT_H
#include <vector>

class ListNode;
namespace MergeSort {
    void solution();
    /**
     * 合并两个有序数组，其中一个已预分配内存
     * 要求时间复杂度O(n + m),空间复杂度O(1)
     * @param vecM 有序数组
     * @param vecN 有序数组，预分配内存
     */
    void mergeSortedVectors(std::vector<int>& vecM, std::vector<int>& vecN);


    ListNode* findMid(ListNode* head);
    ListNode* mergeList(ListNode* listA, ListNode* listB);
    /**
     *https://leetcode.cn/problems/7WHec2/description/
     * 归并排序链表 时间复杂度O(nlogn) 空间复杂度O(logn)
     * @param head
     * @return
     */
    ListNode* mergeSortList(ListNode* head);
} // MergeSort

#endif //MERGESORT_H
