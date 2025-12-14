/**
 * @file topk.h
 * @brief Top K 问题集合 - 题目说明与声明
 *
 * Top K 问题是面试高频题，常见解法：
 * 1. 排序：O(n log n)
 * 2. 堆：O(n log k)
 * 3. 快速选择：O(n) 平均
 * 4. 计数/桶排序：O(n)（特定场景）
 */

#ifndef TOPK_H
#define TOPK_H

#include <vector>
#include <string>
#include <queue>

namespace TopK {

// ==================== 题目声明 ====================

/**
 * 题目1: 数组中的第K个最大元素
 * LeetCode 215
 *
 * 给定整数数组 nums 和整数 k，请返回数组中第 k 个最大的元素。
 * 请注意，你需要找的是数组排序后的第 k 个最大的元素，而不是第 k 个不同的元素。
 *
 * 示例:
 *   输入: nums = [3,2,1,5,6,4], k = 2
 *   输出: 5
 *
 * 思路:
 *   - 方法1：最小堆，维护 k 个最大元素
 *   - 方法2：快速选择
 */
int findKthLargest(std::vector<int>& nums, int k);
int findKthLargestSolution(std::vector<int>& nums, int k);

/**
 * 题目2: 前 K 个高频元素
 * LeetCode 347
 *
 * 给你一个整数数组 nums 和一个整数 k，请你返回其中出现频率前 k 高的元素。
 * 你可以按任意顺序返回答案。
 *
 * 示例:
 *   输入: nums = [1,1,1,2,2,3], k = 2
 *   输出: [1,2]
 *
 * 思路:
 *   - 方法1：哈希统计 + 最小堆
 *   - 方法2：哈希统计 + 桶排序
 */
std::vector<int> topKFrequent(std::vector<int>& nums, int k);
std::vector<int> topKFrequentSolution(std::vector<int>& nums, int k);

/**
 * 题目3: 前 K 个高频单词
 * LeetCode 692
 *
 * 给定一个单词列表 words 和一个整数 k，返回前 k 个出现次数最多的单词。
 * 返回的答案应该按单词出现频率由高到低排序。
 * 如果不同的单词有相同出现频率，按字典序排序。
 *
 * 示例:
 *   输入: words = ["i","love","leetcode","i","love","coding"], k = 2
 *   输出: ["i","love"]
 *
 * 思路: 哈希统计 + 自定义比较器的堆
 */
std::vector<std::string> topKFrequentWords(std::vector<std::string>& words, int k);
std::vector<std::string> topKFrequentWordsSolution(std::vector<std::string>& words, int k);

/**
 * 题目4: 最接近原点的 K 个点
 * LeetCode 973
 *
 * 给定一个数组 points，其中 points[i] = [xi, yi] 表示平面上的一个点，
 * 以及一个整数 k，返回离原点最近的 k 个点。
 *
 * 示例:
 *   输入: points = [[1,3],[-2,2]], k = 1
 *   输出: [[-2,2]]
 *
 * 思路: 最大堆维护 k 个最近的点
 */
std::vector<std::vector<int>> kClosest(std::vector<std::vector<int>>& points, int k);
std::vector<std::vector<int>> kClosestSolution(std::vector<std::vector<int>>& points, int k);

/**
 * 题目5: 第K个最小的素数分数
 * LeetCode 786
 *
 * 给你一个按递增顺序排序的数组 arr 和一个整数 k。
 * 数组 arr 由 1 和若干素数组成，且其中所有整数互不相同。
 *
 * 对于每对满足 0 <= i < j < arr.length 的 i 和 j，可以得到分数 arr[i] / arr[j]。
 * 返回第 k 个最小的分数。
 *
 * 示例:
 *   输入: arr = [1,2,3,5], k = 3
 *   输出: [2,5]
 *   解释: 分数为 1/5, 1/3, 2/5, 1/2, 3/5, 2/3，第 3 小的是 2/5
 *
 * 思路:
 *   - 方法1：最小堆
 *   - 方法2：二分查找
 */
std::vector<int> kthSmallestPrimeFraction(std::vector<int>& arr, int k);
std::vector<int> kthSmallestPrimeFractionSolution(std::vector<int>& arr, int k);

/**
 * 题目6: 数据流中的第 K 大元素
 * LeetCode 703
 *
 * 设计一个找到数据流中第 k 大元素的类。
 * 注意是排序后的第 k 大元素，不是第 k 个不同的元素。
 *
 * 实现 KthLargestStream 类：
 *   - KthLargestStream(int k, int[] nums) 使用整数 k 和整数流 nums 初始化对象
 *   - int add(int val) 将 val 插入数据流 nums 后，返回当前数据流中第 k 大的元素
 *
 * 示例:
 *   KthLargestStream kthLargest = new KthLargestStream(3, [4, 5, 8, 2]);
 *   kthLargest.add(3);   // 返回 4
 *   kthLargest.add(5);   // 返回 5
 *
 * 思路: 维护大小为 k 的最小堆
 */
class KthLargestStream {
public:
    KthLargestStream(int k, std::vector<int>& nums);
    int add(int val);

private:
    int k_;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
};

class KthLargestStreamSolution {
public:
    KthLargestStreamSolution(int k, std::vector<int>& nums);
    int add(int val);

private:
    int k_;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
};

/**
 * 题目7: 查找和最小的 K 对数字
 * LeetCode 373
 *
 * 给定两个以非递减顺序排列的整数数组 nums1 和 nums2，以及一个整数 k。
 * 定义一对值 (u,v)，其中第一个元素来自 nums1，第二个元素来自 nums2。
 * 请找到和最小的 k 对数字 (u1,v1), (u2,v2) ... (uk,vk)。
 *
 * 示例:
 *   输入: nums1 = [1,7,11], nums2 = [2,4,6], k = 3
 *   输出: [[1,2],[1,4],[1,6]]
 *
 * 思路: 最小堆 + 多路归并
 */
std::vector<std::vector<int>> kSmallestPairs(std::vector<int>& nums1,
                                              std::vector<int>& nums2, int k);
std::vector<std::vector<int>> kSmallestPairsSolution(std::vector<int>& nums1,
                                                      std::vector<int>& nums2, int k);

/**
 * 题目8: 有序矩阵中第 K 小的元素
 * LeetCode 378
 *
 * 给你一个 n x n 矩阵 matrix，其中每行和每列元素均按升序排序，
 * 找到矩阵中第 k 小的元素。
 *
 * 示例:
 *   输入: matrix = [[1,5,9],[10,11,13],[12,13,15]], k = 8
 *   输出: 13
 *
 * 思路:
 *   - 方法1：最小堆，多路归并
 *   - 方法2：二分查找
 */
int kthSmallest(std::vector<std::vector<int>>& matrix, int k);
int kthSmallestSolution(std::vector<std::vector<int>>& matrix, int k);

/**
 * 题目9: 丑数 II
 * LeetCode 264
 *
 * 丑数是质因子只包含 2, 3, 5 的正整数。
 * 给你一个整数 n，请你找出第 n 个丑数。
 *
 * 示例:
 *   输入: n = 10
 *   输出: 12
 *   解释: [1,2,3,4,5,6,8,9,10,12] 是前 10 个丑数
 *
 * 思路:
 *   - 方法1：最小堆
 *   - 方法2：三指针动态规划
 */
int nthUglyNumber(int n);
int nthUglyNumberSolution(int n);

/**
 * 题目10: 超级丑数
 * LeetCode 313
 *
 * 超级丑数是一个正整数，其质因子都是给定质数数组 primes 中的数。
 * 给你一个整数 n 和一个整数数组 primes，返回第 n 个超级丑数。
 *
 * 示例:
 *   输入: n = 12, primes = [2,7,13,19]
 *   输出: 32
 *
 * 思路: 多指针动态规划
 */
int nthSuperUglyNumber(int n, std::vector<int>& primes);
int nthSuperUglyNumberSolution(int n, std::vector<int>& primes);

/**
 * 附加题: 数据流的中位数
 * LeetCode 295
 *
 * 设计一个支持以下两种操作的数据结构：
 *   - void addNum(int num) 从数据流中添加一个整数到数据结构中
 *   - double findMedian() 返回目前所有元素的中位数
 *
 * 思路: 大顶堆（左半部分）+ 小顶堆（右半部分）
 */
class MedianFinder {
public:
    void addNum(int num);
    double findMedian();
};

class MedianFinderSolution {
public:
    void addNum(int num);
    double findMedian();

private:
    std::priority_queue<int> maxHeap_;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
};

// ==================== 测试函数声明 ====================

void testTopK();          // 测试面试者实现
void testTopKSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTopK();
    testTopKSolution();
}

} // namespace TopK

#endif // TOPK_H
