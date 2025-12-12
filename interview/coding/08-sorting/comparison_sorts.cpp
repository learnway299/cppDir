/**
 * @file comparison_sorts.cpp
 * @brief 比较排序算法实现 - 题目文件
 *
 * 比较排序：通过比较元素大小来决定顺序
 * 时间复杂度下界：O(n log n)
 *
 * 主要算法：
 * - 冒泡排序 O(n²)
 * - 选择排序 O(n²)
 * - 插入排序 O(n²)
 * - 希尔排序 O(n^1.3)
 * - 归并排序 O(n log n)
 * - 快速排序 O(n log n) 平均
 * - 堆排序 O(n log n)
 */

#include <iostream>
#include <vector>
#include <algorithm>

/**
 * 题目1: 冒泡排序
 *
 * 原理：
 *   - 重复遍历数组，比较相邻元素
 *   - 如果顺序错误就交换
 *   - 每轮遍历将最大元素"冒泡"到末尾
 *
 * 时间复杂度: O(n²)
 * 空间复杂度: O(1)
 * 稳定性: 稳定
 *
 * 优化：
 *   - 如果某轮没有交换，说明已排序，可提前退出
 */
void bubbleSort(std::vector<int>& nums) {
    // TODO: 实现冒泡排序
}

/**
 * 题目2: 选择排序
 *
 * 原理：
 *   - 找到未排序部分的最小元素
 *   - 将其与未排序部分的第一个元素交换
 *   - 重复直到全部排序
 *
 * 时间复杂度: O(n²)
 * 空间复杂度: O(1)
 * 稳定性: 不稳定
 */
void selectionSort(std::vector<int>& nums) {
    // TODO: 实现选择排序
}

/**
 * 题目3: 插入排序
 *
 * 原理：
 *   - 将数组分为已排序和未排序两部分
 *   - 从未排序部分取一个元素，插入已排序部分的正确位置
 *
 * 时间复杂度: O(n²)，对于近乎有序的数组接近 O(n)
 * 空间复杂度: O(1)
 * 稳定性: 稳定
 *
 * 优点：对于小数组和近乎有序的数组效率高
 */
void insertionSort(std::vector<int>& nums) {
    // TODO: 实现插入排序
}

/**
 * 题目4: 希尔排序
 *
 * 原理：
 *   - 插入排序的改进版
 *   - 先按间隔 gap 分组进行插入排序
 *   - 逐渐减小 gap，最终 gap=1 时完成排序
 *
 * 时间复杂度: O(n^1.3) ~ O(n²)，取决于 gap 序列
 * 空间复杂度: O(1)
 * 稳定性: 不稳定
 */
void shellSort(std::vector<int>& nums) {
    // TODO: 实现希尔排序
}

/**
 * 题目5: 归并排序
 *
 * 原理：
 *   - 分治思想：将数组分成两半
 *   - 递归排序每一半
 *   - 合并两个有序数组
 *
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(n)
 * 稳定性: 稳定
 *
 * 应用：外部排序、链表排序
 */
void mergeSort(std::vector<int>& nums) {
    // TODO: 实现归并排序
}

/**
 * 题目6: 快速排序
 *
 * 原理：
 *   - 选择一个基准元素 pivot
 *   - 将小于 pivot 的放左边，大于 pivot 的放右边
 *   - 递归排序左右两部分
 *
 * 时间复杂度: O(n log n) 平均，O(n²) 最坏
 * 空间复杂度: O(log n) 递归栈
 * 稳定性: 不稳定
 *
 * 优化：
 *   - 三数取中选 pivot
 *   - 小数组使用插入排序
 *   - 随机化避免最坏情况
 */
void quickSort(std::vector<int>& nums) {
    // TODO: 实现快速排序
}

/**
 * 题目7: 堆排序
 *
 * 原理：
 *   - 将数组构建成最大堆
 *   - 反复取出堆顶（最大值）与末尾交换
 *   - 调整堆，重复直到排序完成
 *
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(1)
 * 稳定性: 不稳定
 *
 * 优点：原地排序，最坏情况仍是 O(n log n)
 */
void heapSort(std::vector<int>& nums) {
    // TODO: 实现堆排序
}

/**
 * 题目8: 排序数组
 * LeetCode 912
 *
 * 给你一个整数数组 nums，请将该数组升序排列。
 * 要求时间复杂度 O(n log n)
 *
 * 示例:
 *   输入: nums = [5,2,3,1]
 *   输出: [1,2,3,5]
 *
 * 思路: 使用快速排序或归并排序
 */
std::vector<int> sortArray(std::vector<int>& nums) {
    // TODO: 实现高效排序
    return nums;
}

/**
 * 题目9: 排序链表
 * LeetCode 148
 *
 * 给你链表的头结点 head，请将其按升序排列并返回排序后的链表。
 * 要求时间复杂度 O(n log n)，空间复杂度 O(1)
 *
 * 示例:
 *   输入: head = [4,2,1,3]
 *   输出: [1,2,3,4]
 *
 * 思路:
 *   - 自顶向下归并：找中点 + 递归（O(log n) 空间）
 *   - 自底向上归并：迭代合并（O(1) 空间）
 */
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* sortList(ListNode* head) {
    // TODO: 实现链表排序
    return nullptr;
}

/**
 * 题目10: 对链表进行插入排序
 * LeetCode 147
 *
 * 给定单链表的头节点，使用插入排序对链表进行排序，并返回排序后链表的头。
 *
 * 示例:
 *   输入: head = [4,2,1,3]
 *   输出: [1,2,3,4]
 */
ListNode* insertionSortList(ListNode* head) {
    // TODO: 实现链表插入排序
    return nullptr;
}

/**
 * 题目11: 数组中的逆序对
 * 剑指 Offer 51
 *
 * 在数组中的两个数字，如果前面一个数字大于后面的数字，
 * 则这两个数字组成一个逆序对。输入一个数组，求出这个数组中的逆序对的总数。
 *
 * 示例:
 *   输入: [7,5,6,4]
 *   输出: 5
 *   解释: 逆序对为 (7,5), (7,6), (7,4), (5,4), (6,4)
 *
 * 思路: 归并排序过程中统计逆序对
 */
int reversePairs(std::vector<int>& nums) {
    // TODO: 实现逆序对计数
    return 0;
}

/**
 * 题目12: 计算右侧小于当前元素的个数
 * LeetCode 315
 *
 * 给你一个整数数组 nums，按要求返回一个新数组 counts。
 * 数组 counts[i] 的值是 nums[i] 右侧小于 nums[i] 的元素的数量。
 *
 * 示例:
 *   输入: nums = [5,2,6,1]
 *   输出: [2,1,1,0]
 *
 * 思路: 归并排序变体，需要记录原始索引
 */
std::vector<int> countSmaller(std::vector<int>& nums) {
    // TODO: 实现计算右侧较小元素个数
    return {};
}


// ==================== 测试代码 ====================
#include <cassert>

void testBubbleSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    bubbleSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "bubbleSort: PASSED\n";
}

void testSelectionSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    selectionSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "selectionSort: PASSED\n";
}

void testInsertionSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    insertionSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "insertionSort: PASSED\n";
}

void testShellSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    shellSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "shellSort: PASSED\n";
}

void testMergeSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    mergeSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "mergeSort: PASSED\n";
}

void testQuickSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    quickSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "quickSort: PASSED\n";
}

void testHeapSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    heapSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "heapSort: PASSED\n";
}

void testReversePairs() {
    std::vector<int> nums = {7, 5, 6, 4};
    assert(reversePairs(nums) == 5);
    std::cout << "reversePairs: PASSED\n";
}

void testCountSmaller() {
    std::vector<int> nums = {5, 2, 6, 1};
    auto result = countSmaller(nums);
    assert((result == std::vector<int>{2, 1, 1, 0}));
    std::cout << "countSmaller: PASSED\n";
}

int main() {
    std::cout << "=== Comparison Sorts Tests ===\n";

    // 取消注释以运行测试
    // testBubbleSort();
    // testSelectionSort();
    // testInsertionSort();
    // testShellSort();
    // testMergeSort();
    // testQuickSort();
    // testHeapSort();
    // testReversePairs();
    // testCountSmaller();

    std::cout << "All tests passed!\n";
    return 0;
}
