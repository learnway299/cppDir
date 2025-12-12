/**
 * @file comparison_sorts_solution.cpp
 * @brief 比较排序算法实现 - 解答文件
 *
 * 排序算法对比：
 * | 算法     | 平均     | 最坏     | 空间   | 稳定 |
 * |----------|----------|----------|--------|------|
 * | 冒泡     | O(n²)    | O(n²)    | O(1)   | ✓    |
 * | 选择     | O(n²)    | O(n²)    | O(1)   | ✗    |
 * | 插入     | O(n²)    | O(n²)    | O(1)   | ✓    |
 * | 希尔     | O(n^1.3) | O(n²)    | O(1)   | ✗    |
 * | 归并     | O(nlogn) | O(nlogn) | O(n)   | ✓    |
 * | 快速     | O(nlogn) | O(n²)    | O(logn)| ✗    |
 * | 堆       | O(nlogn) | O(nlogn) | O(1)   | ✗    |
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

/**
 * 题目1: 冒泡排序
 *
 * 优化版本：添加提前退出机制
 */
void bubbleSort(std::vector<int>& nums) {
    int n = nums.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (nums[j] > nums[j + 1]) {
                std::swap(nums[j], nums[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;  // 没有交换，已排序
    }
}

/**
 * 题目2: 选择排序
 */
void selectionSort(std::vector<int>& nums) {
    int n = nums.size();
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

/**
 * 题目3: 插入排序
 */
void insertionSort(std::vector<int>& nums) {
    int n = nums.size();
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

/**
 * 二分插入排序（减少比较次数）
 */
void binaryInsertionSort(std::vector<int>& nums) {
    int n = nums.size();
    for (int i = 1; i < n; ++i) {
        int key = nums[i];
        // 二分查找插入位置
        int left = 0, right = i;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] > key) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        // 移动元素
        for (int j = i; j > left; --j) {
            nums[j] = nums[j - 1];
        }
        nums[left] = key;
    }
}

/**
 * 题目4: 希尔排序
 *
 * 使用 Hibbard 序列：1, 3, 7, 15, ... (2^k - 1)
 */
void shellSort(std::vector<int>& nums) {
    int n = nums.size();
    // 生成 gap 序列
    int gap = 1;
    while (gap < n / 3) {
        gap = gap * 3 + 1;  // 1, 4, 13, 40, ...
    }

    while (gap >= 1) {
        // 对每个 gap 组进行插入排序
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

/**
 * 题目5: 归并排序
 */
class MergeSorter {
private:
    std::vector<int> temp;

    void merge(std::vector<int>& nums, int left, int mid, int right) {
        // 复制到临时数组
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
    }

    void sort(std::vector<int>& nums, int left, int right) {
        if (left >= right) return;

        int mid = left + (right - left) / 2;
        sort(nums, left, mid);
        sort(nums, mid + 1, right);

        // 优化：如果已经有序，跳过合并
        if (nums[mid] <= nums[mid + 1]) return;

        merge(nums, left, mid, right);
    }

public:
    void operator()(std::vector<int>& nums) {
        if (nums.empty()) return;
        temp.resize(nums.size());
        sort(nums, 0, nums.size() - 1);
    }
};

void mergeSort(std::vector<int>& nums) {
    MergeSorter sorter;
    sorter(nums);
}

/**
 * 非递归归并排序（自底向上）
 */
void mergeSortIterative(std::vector<int>& nums) {
    int n = nums.size();
    if (n <= 1) return;

    std::vector<int> temp(n);

    auto merge = [&](int left, int mid, int right) {
        for (int i = left; i <= right; ++i) {
            temp[i] = nums[i];
        }
        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (temp[i] <= temp[j]) nums[k++] = temp[i++];
            else nums[k++] = temp[j++];
        }
        while (i <= mid) nums[k++] = temp[i++];
        while (j <= right) nums[k++] = temp[j++];
    };

    // 从 size=1 开始，每次翻倍
    for (int size = 1; size < n; size *= 2) {
        for (int left = 0; left < n - size; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            merge(left, mid, right);
        }
    }
}

/**
 * 题目6: 快速排序
 */
class QuickSorter {
private:
    std::mt19937 rng{std::random_device{}()};

    int partition(std::vector<int>& nums, int left, int right) {
        // 随机选择 pivot
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
    }

    void sort(std::vector<int>& nums, int left, int right) {
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

        int pivotIdx = partition(nums, left, right);
        sort(nums, left, pivotIdx - 1);
        sort(nums, pivotIdx + 1, right);
    }

public:
    void operator()(std::vector<int>& nums) {
        if (nums.size() <= 1) return;
        sort(nums, 0, nums.size() - 1);
    }
};

void quickSort(std::vector<int>& nums) {
    QuickSorter sorter;
    sorter(nums);
}

/**
 * 三路快排（适合有大量重复元素的情况）
 */
void quickSort3Way(std::vector<int>& nums, int left, int right) {
    if (left >= right) return;

    // 三路划分：< pivot | == pivot | > pivot
    int lt = left, gt = right, i = left + 1;
    int pivot = nums[left];

    while (i <= gt) {
        if (nums[i] < pivot) {
            std::swap(nums[lt++], nums[i++]);
        } else if (nums[i] > pivot) {
            std::swap(nums[i], nums[gt--]);
        } else {
            ++i;
        }
    }

    quickSort3Way(nums, left, lt - 1);
    quickSort3Way(nums, gt + 1, right);
}

/**
 * 题目7: 堆排序
 */
void heapSort(std::vector<int>& nums) {
    int n = nums.size();
    if (n <= 1) return;

    // 下沉操作
    auto siftDown = [&](int i, int heapSize) {
        while (2 * i + 1 < heapSize) {
            int j = 2 * i + 1;  // 左孩子
            if (j + 1 < heapSize && nums[j + 1] > nums[j]) {
                ++j;  // 右孩子更大
            }
            if (nums[i] >= nums[j]) break;
            std::swap(nums[i], nums[j]);
            i = j;
        }
    };

    // 建堆（从最后一个非叶节点开始）
    for (int i = n / 2 - 1; i >= 0; --i) {
        siftDown(i, n);
    }

    // 排序
    for (int i = n - 1; i > 0; --i) {
        std::swap(nums[0], nums[i]);
        siftDown(0, i);
    }
}

/**
 * 题目8: 排序数组 - 使用随机快排
 */
std::vector<int> sortArray(std::vector<int>& nums) {
    quickSort(nums);
    return nums;
}

/**
 * 题目9: 排序链表
 */
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 方法1：自顶向下归并（O(log n) 空间）
class Solution148TopDown {
private:
    ListNode* getMid(ListNode* head) {
        ListNode* slow = head;
        ListNode* fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }

    ListNode* merge(ListNode* l1, ListNode* l2) {
        ListNode dummy(0);
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

public:
    ListNode* sortList(ListNode* head) {
        if (!head || !head->next) return head;

        ListNode* mid = getMid(head);
        ListNode* rightHead = mid->next;
        mid->next = nullptr;

        ListNode* left = sortList(head);
        ListNode* right = sortList(rightHead);

        return merge(left, right);
    }
};

// 方法2：自底向上归并（O(1) 空间）
class Solution148BottomUp {
private:
    ListNode* split(ListNode* head, int n) {
        for (int i = 1; head && i < n; ++i) {
            head = head->next;
        }
        if (!head) return nullptr;
        ListNode* second = head->next;
        head->next = nullptr;
        return second;
    }

    ListNode* merge(ListNode* l1, ListNode* l2, ListNode* tail) {
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
    }

public:
    ListNode* sortList(ListNode* head) {
        if (!head || !head->next) return head;

        // 计算链表长度
        int length = 0;
        for (ListNode* curr = head; curr; curr = curr->next) {
            ++length;
        }

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
};

ListNode* sortList(ListNode* head) {
    Solution148BottomUp solution;
    return solution.sortList(head);
}

/**
 * 题目10: 链表插入排序
 */
ListNode* insertionSortList(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode dummy(0);
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;

        // 找到插入位置
        ListNode* prev = &dummy;
        while (prev->next && prev->next->val < curr->val) {
            prev = prev->next;
        }

        // 插入
        curr->next = prev->next;
        prev->next = curr;

        curr = next;
    }

    return dummy.next;
}

/**
 * 题目11: 数组中的逆序对
 *
 * 归并排序过程中统计：当右半部分元素先归并时，
 * 说明它小于左半部分所有剩余元素，产生逆序对
 */
class Solution51 {
private:
    std::vector<int> temp;
    int count;

    void mergeCount(std::vector<int>& nums, int left, int mid, int right) {
        for (int i = left; i <= right; ++i) {
            temp[i] = nums[i];
        }

        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (temp[i] <= temp[j]) {
                nums[k++] = temp[i++];
            } else {
                nums[k++] = temp[j++];
                count += mid - i + 1;  // 左边剩余元素都比 temp[j] 大
            }
        }
        while (i <= mid) nums[k++] = temp[i++];
        while (j <= right) nums[k++] = temp[j++];
    }

    void sort(std::vector<int>& nums, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        sort(nums, left, mid);
        sort(nums, mid + 1, right);
        if (nums[mid] > nums[mid + 1]) {
            mergeCount(nums, left, mid, right);
        }
    }

public:
    int reversePairs(std::vector<int>& nums) {
        if (nums.empty()) return 0;
        temp.resize(nums.size());
        count = 0;
        sort(nums, 0, nums.size() - 1);
        return count;
    }
};

int reversePairs(std::vector<int>& nums) {
    Solution51 solution;
    return solution.reversePairs(nums);
}

/**
 * 题目12: 计算右侧小于当前元素的个数
 *
 * 归并排序 + 索引数组
 */
class Solution315 {
private:
    std::vector<int> counts;
    std::vector<std::pair<int, int>> temp;  // {value, originalIndex}

    void mergeCount(std::vector<std::pair<int, int>>& arr, int left, int mid, int right) {
        for (int i = left; i <= right; ++i) {
            temp[i] = arr[i];
        }

        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (temp[i].first <= temp[j].first) {
                // 右边已归并的元素都小于 temp[i]
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
    }

    void sort(std::vector<std::pair<int, int>>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        sort(arr, left, mid);
        sort(arr, mid + 1, right);
        mergeCount(arr, left, mid, right);
    }

public:
    std::vector<int> countSmaller(std::vector<int>& nums) {
        int n = nums.size();
        counts.assign(n, 0);
        temp.resize(n);

        std::vector<std::pair<int, int>> arr(n);
        for (int i = 0; i < n; ++i) {
            arr[i] = {nums[i], i};
        }

        sort(arr, 0, n - 1);
        return counts;
    }
};

std::vector<int> countSmaller(std::vector<int>& nums) {
    Solution315 solution;
    return solution.countSmaller(nums);
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

    std::vector<int> nums2 = {5, 2, 8, 1, 9, 3};
    mergeSortIterative(nums2);
    assert((nums2 == std::vector<int>{1, 2, 3, 5, 8, 9}));

    std::cout << "mergeSort: PASSED\n";
}

void testQuickSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    quickSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));

    std::vector<int> nums2 = {5, 2, 8, 1, 9, 3};
    quickSort3Way(nums2, 0, nums2.size() - 1);
    assert((nums2 == std::vector<int>{1, 2, 3, 5, 8, 9}));

    std::cout << "quickSort: PASSED\n";
}

void testHeapSort() {
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    heapSort(nums);
    assert((nums == std::vector<int>{1, 2, 3, 5, 8, 9}));
    std::cout << "heapSort: PASSED\n";
}

void testSortList() {
    // 创建链表 4->2->1->3
    ListNode* head = new ListNode(4);
    head->next = new ListNode(2);
    head->next->next = new ListNode(1);
    head->next->next->next = new ListNode(3);

    head = sortList(head);

    // 验证结果 1->2->3->4
    assert(head->val == 1);
    assert(head->next->val == 2);
    assert(head->next->next->val == 3);
    assert(head->next->next->next->val == 4);

    // 清理
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }

    std::cout << "sortList: PASSED\n";
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
    std::cout << "=== Comparison Sorts Solution Tests ===\n";

    testBubbleSort();
    testSelectionSort();
    testInsertionSort();
    testShellSort();
    testMergeSort();
    testQuickSort();
    testHeapSort();
    testSortList();
    testReversePairs();
    testCountSmaller();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
