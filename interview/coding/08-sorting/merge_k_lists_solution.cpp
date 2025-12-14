/**
 * @file merge_k_lists_solution.cpp
 * @brief K 路归并问题集合 - 参考答案
 */

#include "merge_k_lists.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>
#include <functional>
#include <climits>
#include <tuple>

namespace MergeKLists {

// ==================== 参考答案实现 ====================

ListNode* mergeKListsSolution(std::vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) {
        return a->val > b->val;
    };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> minHeap(cmp);

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

void mergeSolution(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n) {
    int i = m - 1, j = n - 1, k = m + n - 1;

    while (i >= 0 && j >= 0) {
        if (nums1[i] > nums2[j]) {
            nums1[k--] = nums1[i--];
        } else {
            nums1[k--] = nums2[j--];
        }
    }

    while (j >= 0) {
        nums1[k--] = nums2[j--];
    }
}

std::vector<int> mergeKArraysSolution(std::vector<std::vector<int>>& arrays) {
    auto cmp = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

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

std::vector<std::vector<int>> mergeIntervalsSolution(std::vector<std::vector<int>>& intervals) {
    if (intervals.empty()) return {};

    std::sort(intervals.begin(), intervals.end());

    std::vector<std::vector<int>> result;
    result.push_back(intervals[0]);

    for (int i = 1; i < static_cast<int>(intervals.size()); ++i) {
        if (intervals[i][0] <= result.back()[1]) {
            result.back()[1] = std::max(result.back()[1], intervals[i][1]);
        } else {
            result.push_back(intervals[i]);
        }
    }

    return result;
}

std::vector<std::vector<int>> insertSolution(std::vector<std::vector<int>>& intervals,
                                              std::vector<int>& newInterval) {
    std::vector<std::vector<int>> result;
    int i = 0;
    int n = static_cast<int>(intervals.size());

    // 左边不重叠部分
    while (i < n && intervals[i][1] < newInterval[0]) {
        result.push_back(intervals[i++]);
    }

    // 重叠部分合并
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = std::min(newInterval[0], intervals[i][0]);
        newInterval[1] = std::max(newInterval[1], intervals[i][1]);
        ++i;
    }
    result.push_back(newInterval);

    // 右边不重叠部分
    while (i < n) {
        result.push_back(intervals[i++]);
    }

    return result;
}

std::vector<int> externalSortSolution(std::vector<std::vector<int>>& chunks) {
    return mergeKArraysSolution(chunks);
}

int smallestDistancePairSolution(std::vector<int>& nums, int k) {
    std::sort(nums.begin(), nums.end());
    int n = static_cast<int>(nums.size());

    int left = 0, right = nums[n - 1] - nums[0];

    while (left < right) {
        int mid = left + (right - left) / 2;

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

int findKthNumberSolution(int m, int n, int k) {
    int left = 1, right = m * n;

    while (left < right) {
        int mid = left + (right - left) / 2;

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

int kthSmallestInMatrixSolution(std::vector<std::vector<int>>& matrix, int k) {
    int n = static_cast<int>(matrix.size());
    int left = matrix[0][0], right = matrix[n - 1][n - 1];

    while (left < right) {
        int mid = left + (right - left) / 2;

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

std::vector<int> smallestRangeSolution(std::vector<std::vector<int>>& nums) {
    int k = static_cast<int>(nums.size());

    auto cmp = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    int curMax = INT_MIN;

    for (int i = 0; i < k; ++i) {
        minHeap.push({nums[i][0], i, 0});
        curMax = std::max(curMax, nums[i][0]);
    }

    int rangeStart = 0, rangeEnd = INT_MAX;

    while (true) {
        auto [curMin, listIdx, elemIdx] = minHeap.top();
        minHeap.pop();

        if (curMax - curMin < rangeEnd - rangeStart) {
            rangeStart = curMin;
            rangeEnd = curMax;
        }

        if (elemIdx + 1 >= static_cast<int>(nums[listIdx].size())) {
            break;
        }

        int nextVal = nums[listIdx][elemIdx + 1];
        minHeap.push({nextVal, listIdx, elemIdx + 1});
        curMax = std::max(curMax, nextVal);
    }

    return {rangeStart, rangeEnd};
}

int nthUglyNumber3Solution(int n, int a, int b, int c) {
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

    long left = 1, right = 2000000000L;

    while (left < right) {
        long mid = left + (right - left) / 2;

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

// ==================== 测试函数 ====================

void testMergeKListsSolution() {
    std::cout << "=== Merge K Lists Tests (Solution) ===\n";

    // mergeKListsSolution
    std::vector<ListNode*> lists = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* result = mergeKListsSolution(lists);
    auto vec = listToVector(result);
    assert((vec == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    freeList(result);
    std::cout << "  mergeKListsSolution: PASSED\n";

    // mergeSolution
    std::vector<int> nums1 = {1, 2, 3, 0, 0, 0};
    std::vector<int> nums2 = {2, 5, 6};
    mergeSolution(nums1, 3, nums2, 3);
    assert((nums1 == std::vector<int>{1, 2, 2, 3, 5, 6}));
    std::cout << "  mergeSolution: PASSED\n";

    // mergeKArraysSolution
    std::vector<std::vector<int>> arrays = {{1, 4, 5}, {1, 3, 4}, {2, 6}};
    auto arrResult = mergeKArraysSolution(arrays);
    assert((arrResult == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    std::cout << "  mergeKArraysSolution: PASSED\n";

    // mergeIntervalsSolution
    std::vector<std::vector<int>> intervals = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    auto mergeResult = mergeIntervalsSolution(intervals);
    assert((mergeResult == std::vector<std::vector<int>>{{1, 6}, {8, 10}, {15, 18}}));
    std::cout << "  mergeIntervalsSolution: PASSED\n";

    // insertSolution
    std::vector<std::vector<int>> intervals2 = {{1, 3}, {6, 9}};
    std::vector<int> newInterval = {2, 5};
    auto insertResult = insertSolution(intervals2, newInterval);
    assert((insertResult == std::vector<std::vector<int>>{{1, 5}, {6, 9}}));
    std::cout << "  insertSolution: PASSED\n";

    // smallestDistancePairSolution
    std::vector<int> nums3 = {1, 3, 1};
    assert(smallestDistancePairSolution(nums3, 1) == 0);
    std::cout << "  smallestDistancePairSolution: PASSED\n";

    // findKthNumberSolution
    assert(findKthNumberSolution(3, 3, 5) == 3);
    std::cout << "  findKthNumberSolution: PASSED\n";

    // kthSmallestInMatrixSolution
    std::vector<std::vector<int>> matrix = {{1, 5, 9}, {10, 11, 13}, {12, 13, 15}};
    assert(kthSmallestInMatrixSolution(matrix, 8) == 13);
    std::cout << "  kthSmallestInMatrixSolution: PASSED\n";

    // smallestRangeSolution
    std::vector<std::vector<int>> rangeNums = {{4, 10, 15, 24, 26}, {0, 9, 12, 20}, {5, 18, 22, 30}};
    auto rangeResult = smallestRangeSolution(rangeNums);
    assert(rangeResult[0] == 20 && rangeResult[1] == 24);
    std::cout << "  smallestRangeSolution: PASSED\n";

    // nthUglyNumber3Solution
    assert(nthUglyNumber3Solution(3, 2, 3, 5) == 4);
    std::cout << "  nthUglyNumber3Solution: PASSED\n";
}

} // namespace MergeKLists
