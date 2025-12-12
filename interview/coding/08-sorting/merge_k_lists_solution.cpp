/**
 * @file merge_k_lists_solution.cpp
 * @brief K 路归并问题集合 - 解答文件
 *
 * K 路归并复杂度分析：
 * - 堆方法：O(N log K)，N 是总元素数，K 是序列数
 * - 分治方法：O(N log K)
 * - 逐一合并：O(NK)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

/**
 * 题目1: 合并K个升序链表
 *
 * 方法1：优先队列（最小堆）
 * 时间: O(N log K)
 * 空间: O(K)
 */
ListNode* mergeKListsHeap(std::vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) {
        return a->val > b->val;
    };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> minHeap(cmp);

    // 初始化：所有链表头入堆
    for (ListNode* list : lists) {
        if (list) {
            minHeap.push(list);
        }
    }

    ListNode dummy;
    ListNode* curr = &dummy;

    while (!minHeap.empty()) {
        ListNode* node = minHeap.top();
        minHeap.pop();
        curr->next = node;
        curr = curr->next;

        if (node->next) {
            minHeap.push(node->next);
        }
    }

    return dummy.next;
}

/**
 * 方法2：分治归并
 * 时间: O(N log K)
 * 空间: O(log K) 递归栈
 */
class Solution23Divide {
private:
    ListNode* merge2Lists(ListNode* l1, ListNode* l2) {
        ListNode dummy;
        ListNode* curr = &dummy;

        while (l1 && l2) {
            if (l1->val <= l2->val) {
                curr->next = l1;
                l1 = l1->next;
            } else {
                curr->next = l2;
                l2 = l2->next;
            }
            curr = curr->next;
        }
        curr->next = l1 ? l1 : l2;
        return dummy.next;
    }

    ListNode* mergeRange(std::vector<ListNode*>& lists, int left, int right) {
        if (left > right) return nullptr;
        if (left == right) return lists[left];

        int mid = left + (right - left) / 2;
        ListNode* leftList = mergeRange(lists, left, mid);
        ListNode* rightList = mergeRange(lists, mid + 1, right);
        return merge2Lists(leftList, rightList);
    }

public:
    ListNode* mergeKLists(std::vector<ListNode*>& lists) {
        if (lists.empty()) return nullptr;
        return mergeRange(lists, 0, lists.size() - 1);
    }
};

ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    return mergeKListsHeap(lists);
}

/**
 * 题目2: 合并两个有序数组
 *
 * 从后往前填充，避免覆盖
 */
void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n) {
    int i = m - 1, j = n - 1, k = m + n - 1;

    while (i >= 0 && j >= 0) {
        if (nums1[i] > nums2[j]) {
            nums1[k--] = nums1[i--];
        } else {
            nums1[k--] = nums2[j--];
        }
    }

    // 如果 nums2 还有剩余
    while (j >= 0) {
        nums1[k--] = nums2[j--];
    }
}

/**
 * 题目3: 合并K个有序数组
 */
std::vector<int> mergeKArrays(std::vector<std::vector<int>>& arrays) {
    // {值, 数组索引, 元素索引}
    auto cmp = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    // 初始化
    for (int i = 0; i < static_cast<int>(arrays.size()); ++i) {
        if (!arrays[i].empty()) {
            minHeap.push({arrays[i][0], i, 0});
        }
    }

    std::vector<int> result;
    while (!minHeap.empty()) {
        auto [val, arrIdx, elemIdx] = minHeap.top();
        minHeap.pop();
        result.push_back(val);

        if (elemIdx + 1 < static_cast<int>(arrays[arrIdx].size())) {
            minHeap.push({arrays[arrIdx][elemIdx + 1], arrIdx, elemIdx + 1});
        }
    }

    return result;
}

/**
 * 题目4: 合并区间
 */
std::vector<std::vector<int>> mergeIntervals(std::vector<std::vector<int>>& intervals) {
    if (intervals.empty()) return {};

    // 按起点排序
    std::sort(intervals.begin(), intervals.end());

    std::vector<std::vector<int>> result;
    result.push_back(intervals[0]);

    for (int i = 1; i < static_cast<int>(intervals.size()); ++i) {
        if (intervals[i][0] <= result.back()[1]) {
            // 重叠，合并
            result.back()[1] = std::max(result.back()[1], intervals[i][1]);
        } else {
            // 不重叠，添加新区间
            result.push_back(intervals[i]);
        }
    }

    return result;
}

/**
 * 题目5: 插入区间
 */
std::vector<std::vector<int>> insert(std::vector<std::vector<int>>& intervals,
                                      std::vector<int>& newInterval) {
    std::vector<std::vector<int>> result;
    int i = 0;
    int n = intervals.size();

    // 1. 添加所有在 newInterval 左边不重叠的区间
    while (i < n && intervals[i][1] < newInterval[0]) {
        result.push_back(intervals[i++]);
    }

    // 2. 合并所有重叠的区间
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = std::min(newInterval[0], intervals[i][0]);
        newInterval[1] = std::max(newInterval[1], intervals[i][1]);
        ++i;
    }
    result.push_back(newInterval);

    // 3. 添加所有在 newInterval 右边不重叠的区间
    while (i < n) {
        result.push_back(intervals[i++]);
    }

    return result;
}

/**
 * 题目6: 外部排序的合并阶段
 */
std::vector<int> externalSort(std::vector<std::vector<int>>& chunks) {
    return mergeKArrays(chunks);
}

/**
 * 题目7: 找出第 K 小的数对距离
 *
 * 二分答案 + 双指针统计
 */
int smallestDistancePair(std::vector<int>& nums, int k) {
    std::sort(nums.begin(), nums.end());
    int n = nums.size();

    int left = 0, right = nums[n - 1] - nums[0];

    while (left < right) {
        int mid = left + (right - left) / 2;

        // 统计距离 <= mid 的数对个数
        int count = 0;
        int j = 0;
        for (int i = 0; i < n; ++i) {
            while (j < n && nums[j] - nums[i] <= mid) ++j;
            count += j - i - 1;
        }

        if (count < k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

/**
 * 题目8: 第 K 个最小的乘法表数
 *
 * 二分答案
 */
int findKthNumber(int m, int n, int k) {
    int left = 1, right = m * n;

    while (left < right) {
        int mid = left + (right - left) / 2;

        // 统计 <= mid 的数的个数
        int count = 0;
        for (int i = 1; i <= m; ++i) {
            count += std::min(mid / i, n);
        }

        if (count < k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

/**
 * 题目9: 有序矩阵中的第 K 小的元素
 *
 * 方法1：K 路归并
 */
int kthSmallestInMatrixHeap(std::vector<std::vector<int>>& matrix, int k) {
    int n = matrix.size();
    auto cmp = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    // 初始化：每行第一个元素
    for (int i = 0; i < n; ++i) {
        minHeap.push({matrix[i][0], i, 0});
    }

    for (int i = 0; i < k - 1; ++i) {
        auto [val, row, col] = minHeap.top();
        minHeap.pop();
        if (col + 1 < n) {
            minHeap.push({matrix[row][col + 1], row, col + 1});
        }
    }

    return std::get<0>(minHeap.top());
}

/**
 * 方法2：二分查找
 */
int kthSmallestInMatrix(std::vector<std::vector<int>>& matrix, int k) {
    int n = matrix.size();
    int left = matrix[0][0], right = matrix[n - 1][n - 1];

    while (left < right) {
        int mid = left + (right - left) / 2;

        // 统计 <= mid 的元素个数
        int count = 0;
        int j = n - 1;
        for (int i = 0; i < n; ++i) {
            while (j >= 0 && matrix[i][j] > mid) --j;
            count += j + 1;
        }

        if (count < k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

/**
 * 题目10: 最小区间
 *
 * 使用最小堆，跟踪当前窗口
 */
std::vector<int> smallestRange(std::vector<std::vector<int>>& nums) {
    int k = nums.size();

    // {值, 列表索引, 元素索引}
    auto cmp = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    int curMax = INT_MIN;

    // 初始化：每个列表的第一个元素
    for (int i = 0; i < k; ++i) {
        minHeap.push({nums[i][0], i, 0});
        curMax = std::max(curMax, nums[i][0]);
    }

    int rangeStart = 0, rangeEnd = INT_MAX;

    while (true) {
        auto [curMin, listIdx, elemIdx] = minHeap.top();
        minHeap.pop();

        // 更新最小区间
        if (curMax - curMin < rangeEnd - rangeStart) {
            rangeStart = curMin;
            rangeEnd = curMax;
        }

        // 移动该列表的指针
        if (elemIdx + 1 >= static_cast<int>(nums[listIdx].size())) {
            break;  // 某个列表耗尽，无法继续
        }

        int nextVal = nums[listIdx][elemIdx + 1];
        minHeap.push({nextVal, listIdx, elemIdx + 1});
        curMax = std::max(curMax, nextVal);
    }

    return {rangeStart, rangeEnd};
}

/**
 * 附加: 丑数 III
 * LeetCode 1201
 *
 * 给你四个整数 n, a, b, c，请你设计一个算法来找出第 n 个丑数。
 * 丑数是可以被 a 或 b 或 c 整除的正整数。
 *
 * 思路: 二分答案 + 容斥原理
 */
int nthUglyNumber3(int n, int a, int b, int c) {
    auto gcd = [](long x, long y) -> long {
        while (y) {
            long t = x % y;
            x = y;
            y = t;
        }
        return x;
    };

    auto lcm = [&](long x, long y) -> long {
        return x / gcd(x, y) * y;
    };

    long A = a, B = b, C = c;
    long AB = lcm(A, B);
    long AC = lcm(A, C);
    long BC = lcm(B, C);
    long ABC = lcm(AB, C);

    long left = 1, right = 2e9;

    while (left < right) {
        long mid = left + (right - left) / 2;

        // 容斥原理：统计 <= mid 的丑数个数
        long count = mid / A + mid / B + mid / C
                   - mid / AB - mid / AC - mid / BC
                   + mid / ABC;

        if (count < n) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return static_cast<int>(left);
}


// ==================== 测试代码 ====================
#include <cassert>

ListNode* createList(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* curr = &dummy;
    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
    }
    return dummy.next;
}

std::vector<int> listToVector(ListNode* head) {
    std::vector<int> result;
    while (head) {
        result.push_back(head->val);
        head = head->next;
    }
    return result;
}

void freeList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

void testMergeKLists() {
    std::vector<ListNode*> lists = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* result = mergeKLists(lists);
    auto vec = listToVector(result);
    assert((vec == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    freeList(result);
    std::cout << "mergeKLists: PASSED\n";
}

void testMerge() {
    std::vector<int> nums1 = {1, 2, 3, 0, 0, 0};
    std::vector<int> nums2 = {2, 5, 6};
    merge(nums1, 3, nums2, 3);
    assert((nums1 == std::vector<int>{1, 2, 2, 3, 5, 6}));
    std::cout << "merge: PASSED\n";
}

void testMergeKArrays() {
    std::vector<std::vector<int>> arrays = {{1, 4, 5}, {1, 3, 4}, {2, 6}};
    auto result = mergeKArrays(arrays);
    assert((result == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    std::cout << "mergeKArrays: PASSED\n";
}

void testMergeIntervals() {
    std::vector<std::vector<int>> intervals = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    auto result = mergeIntervals(intervals);
    assert((result == std::vector<std::vector<int>>{{1, 6}, {8, 10}, {15, 18}}));
    std::cout << "mergeIntervals: PASSED\n";
}

void testInsert() {
    std::vector<std::vector<int>> intervals = {{1, 3}, {6, 9}};
    std::vector<int> newInterval = {2, 5};
    auto result = insert(intervals, newInterval);
    assert((result == std::vector<std::vector<int>>{{1, 5}, {6, 9}}));
    std::cout << "insert: PASSED\n";
}

void testSmallestDistancePair() {
    std::vector<int> nums = {1, 3, 1};
    assert(smallestDistancePair(nums, 1) == 0);

    std::vector<int> nums2 = {1, 6, 1};
    assert(smallestDistancePair(nums2, 3) == 5);

    std::cout << "smallestDistancePair: PASSED\n";
}

void testFindKthNumber() {
    assert(findKthNumber(3, 3, 5) == 3);
    assert(findKthNumber(2, 3, 6) == 6);
    std::cout << "findKthNumber: PASSED\n";
}

void testKthSmallestInMatrix() {
    std::vector<std::vector<int>> matrix = {{1, 5, 9}, {10, 11, 13}, {12, 13, 15}};
    assert(kthSmallestInMatrix(matrix, 8) == 13);
    std::cout << "kthSmallestInMatrix: PASSED\n";
}

void testSmallestRange() {
    std::vector<std::vector<int>> nums = {{4, 10, 15, 24, 26}, {0, 9, 12, 20}, {5, 18, 22, 30}};
    auto result = smallestRange(nums);
    assert(result[0] == 20 && result[1] == 24);
    std::cout << "smallestRange: PASSED\n";
}

void testNthUglyNumber3() {
    assert(nthUglyNumber3(3, 2, 3, 5) == 4);
    assert(nthUglyNumber3(4, 2, 3, 4) == 6);
    std::cout << "nthUglyNumber3: PASSED\n";
}

int main() {
    std::cout << "=== Merge K Lists Solution Tests ===\n";

    testMergeKLists();
    testMerge();
    testMergeKArrays();
    testMergeIntervals();
    testInsert();
    testSmallestDistancePair();
    testFindKthNumber();
    testKthSmallestInMatrix();
    testSmallestRange();
    testNthUglyNumber3();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
