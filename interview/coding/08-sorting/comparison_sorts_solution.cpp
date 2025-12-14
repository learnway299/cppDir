/**
 * @file comparison_sorts_solution.cpp
 * @brief 比较排序算法实现 - 参考答案
 */

#include "comparison_sorts.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <random>
#include <functional>

namespace ComparisonSorts {

// ==================== 参考答案实现 ====================

void bubbleSortSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (nums[j] > nums[j + 1]) {
                std::swap(nums[j], nums[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void selectionSortSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (nums[j] < nums[minIdx]) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            std::swap(nums[i], nums[minIdx]);
        }
    }
}

void insertionSortSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    for (int i = 1; i < n; ++i) {
        int key = nums[i];
        int j = i - 1;
        while (j >= 0 && nums[j] > key) {
            nums[j + 1] = nums[j];
            --j;
        }
        nums[j + 1] = key;
    }
}

void shellSortSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    int gap = 1;
    while (gap < n / 3) {
        gap = gap * 3 + 1;
    }

    while (gap >= 1) {
        for (int i = gap; i < n; ++i) {
            int key = nums[i];
            int j = i - gap;
            while (j >= 0 && nums[j] > key) {
                nums[j + gap] = nums[j];
                j -= gap;
            }
            nums[j + gap] = key;
        }
        gap /= 3;
    }
}

void mergeSortSolution(std::vector<int>& nums) {
    if (nums.empty()) return;

    std::vector<int> temp(nums.size());

    std::function<void(int, int, int)> merge = [&](int left, int mid, int right) {
        for (int i = left; i <= right; ++i) {
            temp[i] = nums[i];
        }
        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (temp[i] <= temp[j]) {
                nums[k++] = temp[i++];
            } else {
                nums[k++] = temp[j++];
            }
        }
        while (i <= mid) nums[k++] = temp[i++];
        while (j <= right) nums[k++] = temp[j++];
    };

    std::function<void(int, int)> sort = [&](int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        sort(left, mid);
        sort(mid + 1, right);
        if (nums[mid] <= nums[mid + 1]) return;
        merge(left, mid, right);
    };

    sort(0, static_cast<int>(nums.size()) - 1);
}

void quickSortSolution(std::vector<int>& nums) {
    if (nums.size() <= 1) return;

    static std::mt19937 rng{std::random_device{}()};

    std::function<int(int, int)> partition = [&](int left, int right) -> int {
        std::uniform_int_distribution<int> dist(left, right);
        int pivotIdx = dist(rng);
        std::swap(nums[pivotIdx], nums[right]);

        int pivot = nums[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (nums[j] < pivot) {
                std::swap(nums[i++], nums[j]);
            }
        }
        std::swap(nums[i], nums[right]);
        return i;
    };

    std::function<void(int, int)> sort = [&](int left, int right) {
        if (left >= right) return;

        // 小数组使用插入排序
        if (right - left < 16) {
            for (int i = left + 1; i <= right; ++i) {
                int key = nums[i];
                int j = i - 1;
                while (j >= left && nums[j] > key) {
                    nums[j + 1] = nums[j];
                    --j;
                }
                nums[j + 1] = key;
            }
            return;
        }

        int pivotIdx = partition(left, right);
        sort(left, pivotIdx - 1);
        sort(pivotIdx + 1, right);
    };

    sort(0, static_cast<int>(nums.size()) - 1);
}

void heapSortSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    if (n <= 1) return;

    auto siftDown = [&](int i, int heapSize) {
        while (2 * i + 1 < heapSize) {
            int j = 2 * i + 1;
            if (j + 1 < heapSize && nums[j + 1] > nums[j]) {
                ++j;
            }
            if (nums[i] >= nums[j]) break;
            std::swap(nums[i], nums[j]);
            i = j;
        }
    };

    // 建堆
    for (int i = n / 2 - 1; i >= 0; --i) {
        siftDown(i, n);
    }

    // 排序
    for (int i = n - 1; i > 0; --i) {
        std::swap(nums[0], nums[i]);
        siftDown(0, i);
    }
}

std::vector<int> sortArraySolution(std::vector<int>& nums) {
    quickSortSolution(nums);
    return nums;
}

ListNode* sortListSolution(ListNode* head) {
    if (!head || !head->next) return head;

    // 自底向上归并排序
    int length = 0;
    for (ListNode* curr = head; curr; curr = curr->next) {
        ++length;
    }

    auto split = [](ListNode* head, int n) -> ListNode* {
        for (int i = 1; head && i < n; ++i) {
            head = head->next;
        }
        if (!head) return nullptr;
        ListNode* second = head->next;
        head->next = nullptr;
        return second;
    };

    auto merge = [](ListNode* l1, ListNode* l2, ListNode* tail) -> ListNode* {
        ListNode* curr = tail;
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
        while (curr->next) curr = curr->next;
        return curr;
    };

    ListNode dummy(0);
    dummy.next = head;

    for (int size = 1; size < length; size *= 2) {
        ListNode* curr = dummy.next;
        ListNode* tail = &dummy;

        while (curr) {
            ListNode* left = curr;
            ListNode* right = split(left, size);
            curr = split(right, size);
            tail = merge(left, right, tail);
        }
    }

    return dummy.next;
}

ListNode* insertionSortListSolution(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode dummy(0);
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;

        ListNode* prev = &dummy;
        while (prev->next && prev->next->val < curr->val) {
            prev = prev->next;
        }

        curr->next = prev->next;
        prev->next = curr;

        curr = next;
    }

    return dummy.next;
}

int reversePairsSolution(std::vector<int>& nums) {
    if (nums.empty()) return 0;

    std::vector<int> temp(nums.size());
    int count = 0;

    std::function<void(int, int, int)> mergeCount = [&](int left, int mid, int right) {
        for (int i = left; i <= right; ++i) {
            temp[i] = nums[i];
        }

        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (temp[i] <= temp[j]) {
                nums[k++] = temp[i++];
            } else {
                nums[k++] = temp[j++];
                count += mid - i + 1;
            }
        }
        while (i <= mid) nums[k++] = temp[i++];
        while (j <= right) nums[k++] = temp[j++];
    };

    std::function<void(int, int)> sort = [&](int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        sort(left, mid);
        sort(mid + 1, right);
        if (nums[mid] > nums[mid + 1]) {
            mergeCount(left, mid, right);
        }
    };

    sort(0, static_cast<int>(nums.size()) - 1);
    return count;
}

std::vector<int> countSmallerSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    std::vector<int> counts(n, 0);
    std::vector<std::pair<int, int>> temp(n);
    std::vector<std::pair<int, int>> arr(n);

    for (int i = 0; i < n; ++i) {
        arr[i] = {nums[i], i};
    }

    std::function<void(int, int, int)> mergeCount = [&](int left, int mid, int right) {
        for (int i = left; i <= right; ++i) {
            temp[i] = arr[i];
        }

        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (temp[i].first <= temp[j].first) {
                counts[temp[i].second] += j - (mid + 1);
                arr[k++] = temp[i++];
            } else {
                arr[k++] = temp[j++];
            }
        }
        while (i <= mid) {
            counts[temp[i].second] += j - (mid + 1);
            arr[k++] = temp[i++];
        }
        while (j <= right) {
            arr[k++] = temp[j++];
        }
    };

    std::function<void(int, int)> sort = [&](int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        sort(left, mid);
        sort(mid + 1, right);
        mergeCount(left, mid, right);
    };

    sort(0, n - 1);
    return counts;
}

// ==================== 测试函数 ====================

void testComparisonSortsSolution() {
    std::cout << "=== Comparison Sorts Tests (Solution) ===\n";

    // bubbleSortSolution
    std::vector<int> nums1 = {5, 2, 8, 1, 9, 3};
    bubbleSortSolution(nums1);
    assert((nums1 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  bubbleSortSolution: PASSED\n";

    // selectionSortSolution
    std::vector<int> nums2 = {5, 2, 8, 1, 9, 3};
    selectionSortSolution(nums2);
    assert((nums2 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  selectionSortSolution: PASSED\n";

    // insertionSortSolution
    std::vector<int> nums3 = {5, 2, 8, 1, 9, 3};
    insertionSortSolution(nums3);
    assert((nums3 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  insertionSortSolution: PASSED\n";

    // shellSortSolution
    std::vector<int> nums4 = {5, 2, 8, 1, 9, 3};
    shellSortSolution(nums4);
    assert((nums4 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  shellSortSolution: PASSED\n";

    // mergeSortSolution
    std::vector<int> nums5 = {5, 2, 8, 1, 9, 3};
    mergeSortSolution(nums5);
    assert((nums5 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  mergeSortSolution: PASSED\n";

    // quickSortSolution
    std::vector<int> nums6 = {5, 2, 8, 1, 9, 3};
    quickSortSolution(nums6);
    assert((nums6 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  quickSortSolution: PASSED\n";

    // heapSortSolution
    std::vector<int> nums7 = {5, 2, 8, 1, 9, 3};
    heapSortSolution(nums7);
    assert((nums7 == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "  heapSortSolution: PASSED\n";

    // sortListSolution
    ListNode* head = new ListNode(4);
    head->next = new ListNode(2);
    head->next->next = new ListNode(1);
    head->next->next->next = new ListNode(3);
    head = sortListSolution(head);
    assert(head->val == 1);
    assert(head->next->val == 2);
    assert(head->next->next->val == 3);
    assert(head->next->next->next->val == 4);
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
    std::cout << "  sortListSolution: PASSED\n";

    // reversePairsSolution
    std::vector<int> nums8 = {7, 5, 6, 4};
    assert(reversePairsSolution(nums8) == 5);
    std::cout << "  reversePairsSolution: PASSED\n";

    // countSmallerSolution
    std::vector<int> nums9 = {5, 2, 6, 1};
    auto result = countSmallerSolution(nums9);
    assert((result == std::vector<int>{2, 1, 1, 0}));
    std::cout << "  countSmallerSolution: PASSED\n";
}

} // namespace ComparisonSorts
