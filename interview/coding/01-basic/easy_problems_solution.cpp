/**
 * @file easy_problems_solution.cpp
 * @brief 基础编程题集合 - 参考答案
 */

#include "easy_problems.h"
#include <unordered_map>
#include <stack>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace EasyProblems {

// ==================== 参考答案实现 ====================

// 题目1: 两数之和 - 哈希表 O(n)
std::vector<int> twoSumSolution(std::vector<int>& nums, int target) {
    std::unordered_map<int, int> numToIndex;
    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        int complement = target - nums[i];
        auto it = numToIndex.find(complement);
        if (it != numToIndex.end()) {
            return {it->second, i};
        }
        numToIndex[nums[i]] = i;
    }
    return {};
}

// 题目2: 反转字符串 - 双指针 O(n)
void reverseStringSolution(std::vector<char>& s) {
    int left = 0, right = static_cast<int>(s.size()) - 1;
    while (left < right) {
        std::swap(s[left++], s[right--]);
    }
}

// 题目3: 回文数判断 - 反转一半 O(log n)
bool isPalindromeSolution(int x) {
    if (x < 0 || (x % 10 == 0 && x != 0)) return false;
    int reversed = 0;
    while (x > reversed) {
        reversed = reversed * 10 + x % 10;
        x /= 10;
    }
    return x == reversed || x == reversed / 10;
}

// 题目4: 有效的括号 - 栈 O(n)
bool isValidSolution(const std::string& s) {
    std::stack<char> stk;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            stk.push(c);
        } else {
            if (stk.empty()) return false;
            char top = stk.top();
            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) {
                return false;
            }
            stk.pop();
        }
    }
    return stk.empty();
}

// 题目5: 合并两个有序数组 - 从后向前双指针 O(m+n)
void mergeSolution(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n) {
    int p1 = m - 1, p2 = n - 1, p = m + n - 1;
    while (p2 >= 0) {
        if (p1 >= 0 && nums1[p1] > nums2[p2]) {
            nums1[p--] = nums1[p1--];
        } else {
            nums1[p--] = nums2[p2--];
        }
    }
}

// 题目6: 移除元素 - 快慢指针 O(n)
int removeElementSolution(std::vector<int>& nums, int val) {
    int slow = 0;
    for (int fast = 0; fast < static_cast<int>(nums.size()); ++fast) {
        if (nums[fast] != val) {
            nums[slow++] = nums[fast];
        }
    }
    return slow;
}

// 题目7: 加一 O(n)
std::vector<int> plusOneSolution(std::vector<int>& digits) {
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        if (digits[i] < 9) {
            digits[i]++;
            return digits;
        }
        digits[i] = 0;
    }
    digits.insert(digits.begin(), 1);
    return digits;
}

// 题目8: 最大子数组和 - Kadane算法 O(n)
int maxSubArraySolution(std::vector<int>& nums) {
    int maxSum = nums[0], currentSum = nums[0];
    for (size_t i = 1; i < nums.size(); ++i) {
        currentSum = std::max(nums[i], currentSum + nums[i]);
        maxSum = std::max(maxSum, currentSum);
    }
    return maxSum;
}

// ==================== 测试函数 ====================

void testEasyProblemsSolution() {
    std::cout << "=== Easy Problems Tests (Solution) ===\n";

    // twoSum
    std::vector<int> nums1 = {2, 7, 11, 15};
    auto result = twoSumSolution(nums1, 9);
    assert(result.size() == 2 && nums1[result[0]] + nums1[result[1]] == 9);
    std::cout << "  twoSumSolution: PASSED\n";

    // reverseString
    std::vector<char> s = {'h', 'e', 'l', 'l', 'o'};
    reverseStringSolution(s);
    assert(s == std::vector<char>({'o', 'l', 'l', 'e', 'h'}));
    std::cout << "  reverseStringSolution: PASSED\n";

    // isPalindrome
    assert(isPalindromeSolution(121) && !isPalindromeSolution(-121) && !isPalindromeSolution(10));
    std::cout << "  isPalindromeSolution: PASSED\n";

    // isValid
    assert(isValidSolution("()[]{}") && !isValidSolution("(]") && isValidSolution("{[]}"));
    std::cout << "  isValidSolution: PASSED\n";

    // merge
    std::vector<int> nums2 = {1, 2, 3, 0, 0, 0};
    std::vector<int> nums3 = {2, 5, 6};
    mergeSolution(nums2, 3, nums3, 3);
    assert(nums2 == std::vector<int>({1, 2, 2, 3, 5, 6}));
    std::cout << "  mergeSolution: PASSED\n";

    // removeElement
    std::vector<int> nums4 = {3, 2, 2, 3};
    assert(removeElementSolution(nums4, 3) == 2);
    std::cout << "  removeElementSolution: PASSED\n";

    // plusOne
    std::vector<int> digits = {9, 9, 9};
    auto plusResult = plusOneSolution(digits);
    assert(plusResult == std::vector<int>({1, 0, 0, 0}));
    std::cout << "  plusOneSolution: PASSED\n";

    // maxSubArray
    std::vector<int> nums5 = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArraySolution(nums5) == 6);
    std::cout << "  maxSubArraySolution: PASSED\n";
}

} // namespace EasyProblems
