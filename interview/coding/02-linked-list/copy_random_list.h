/**
 * @file copy_random_list.h
 * @brief 复制带随机指针的链表 - 题目说明与声明
 *
 * 这是一道经典的链表深拷贝问题，涉及复杂链表结构的处理。
 */

#ifndef COPY_RANDOM_LIST_H
#define COPY_RANDOM_LIST_H

#include "../../util/Util.h"
#include <unordered_map>

namespace CopyRandomList {

// ==================== 题目声明 ====================

/**
 * 题目: 复制带随机指针的链表
 *
 * 给你一个长度为 n 的链表，每个节点包含一个额外增加的随机指针 random，
 * 该指针可以指向链表中的任何节点或空节点。
 *
 * 构造这个链表的深拷贝。
 *
 * 提示:
 *   - 解法1: 哈希表建立原节点到新节点的映射 O(n) 时间 O(n) 空间
 *   - 解法2: 原地复制方法 O(n) 时间 O(1) 空间
 */
RandomListNode* copyRandomList(RandomListNode* head);
RandomListNode* copyRandomListSolution(RandomListNode* head);

// 哈希表解法
RandomListNode* copyRandomListHashSolution(RandomListNode* head);

// ==================== 测试函数声明 ====================

void testCopyRandomList();          // 测试面试者实现
void testCopyRandomListSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testCopyRandomList();
    testCopyRandomListSolution();
}

} // namespace CopyRandomList

#endif // COPY_RANDOM_LIST_H
