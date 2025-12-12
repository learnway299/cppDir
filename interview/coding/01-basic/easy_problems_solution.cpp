/**
 * @file easy_problems_solution.cpp
 * @brief 基础编程题集合 - 解答文件
 *
 * 包含 easy_problems.cpp 中所有题目的解答和详细解析。
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <stack>
#include <algorithm>
#include <climits>

/**
 * 题目1: 两数之和
 *
 * 解法: 哈希表
 * 思路:
 *   - 遍历数组，对于每个元素 nums[i]，检查 target - nums[i] 是否在哈希表中
 *   - 如果存在，返回两个下标
 *   - 如果不存在，将当前元素加入哈希表
 *
 * 时间复杂度: O(n) - 一次遍历
 * 空间复杂度: O(n) - 哈希表存储
 */
std::vector<int> twoSum(std::vector<int>& nums, int target) {
    std::unordered_map<int, int> numToIndex;  // 值 -> 下标

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        int complement = target - nums[i];

        // 检查补数是否存在
        auto it = numToIndex.find(complement);
        if (it != numToIndex.end()) {
            return {it->second, i};
        }

        // 将当前数加入哈希表
        numToIndex[nums[i]] = i;
    }

    return {};  // 无解
}

/**
 * 题目2: 反转字符串
 *
 * 解法: 双指针
 * 思路:
 *   - 使用左右两个指针，从两端向中间移动
 *   - 交换左右指针指向的字符
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
void reverseString(std::vector<char>& s) {
    int left = 0;
    int right = static_cast<int>(s.size()) - 1;

    while (left < right) {
        std::swap(s[left], s[right]);
        ++left;
        --right;
    }
}

/**
 * 题目3: 回文数判断
 *
 * 解法: 反转一半数字
 * 思路:
 *   - 负数不是回文数
 *   - 以 0 结尾的数（除了 0 本身）不是回文数
 *   - 反转数字的后半部分，与前半部分比较
 *
 * 时间复杂度: O(log n) - 数字位数
 * 空间复杂度: O(1)
 */
bool isPalindrome(int x) {
    // 负数和以 0 结尾的数（除 0 外）不是回文
    if (x < 0 || (x % 10 == 0 && x != 0)) {
        return false;
    }

    int reversed = 0;
    while (x > reversed) {
        reversed = reversed * 10 + x % 10;
        x /= 10;
    }

    // 当位数为奇数时，reversed 会多一位，需要除以 10
    return x == reversed || x == reversed / 10;
}

/**
 * 题目4: 有效的括号
 *
 * 解法: 栈
 * 思路:
 *   - 遇到左括号，入栈
 *   - 遇到右括号，检查栈顶是否为对应的左括号
 *   - 最终栈为空则有效
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool isValid(std::string s) {
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

// 更简洁的写法：使用哈希表映射
bool isValidV2(std::string s) {
    std::stack<char> stk;
    std::unordered_map<char, char> pairs = {
        {')', '('},
        {']', '['},
        {'}', '{'}
    };

    for (char c : s) {
        if (pairs.count(c)) {
            // 右括号
            if (stk.empty() || stk.top() != pairs[c]) {
                return false;
            }
            stk.pop();
        } else {
            // 左括号
            stk.push(c);
        }
    }

    return stk.empty();
}

/**
 * 题目5: 合并两个有序数组
 *
 * 解法: 从后向前双指针
 * 思路:
 *   - 从 nums1 末尾开始填充（避免覆盖未处理的元素）
 *   - 比较 nums1 和 nums2 的元素，将较大的放到末尾
 *
 * 时间复杂度: O(m + n)
 * 空间复杂度: O(1)
 */
void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n) {
    int p1 = m - 1;      // nums1 有效元素的最后一个
    int p2 = n - 1;      // nums2 的最后一个
    int p = m + n - 1;   // 合并后的最后一个位置

    while (p2 >= 0) {
        if (p1 >= 0 && nums1[p1] > nums2[p2]) {
            nums1[p--] = nums1[p1--];
        } else {
            nums1[p--] = nums2[p2--];
        }
    }
    // 如果 nums2 已全部合并，nums1 剩余部分已经在正确位置
}

/**
 * 题目6: 移除元素
 *
 * 解法: 双指针（快慢指针）
 * 思路:
 *   - 慢指针指向下一个要填充的位置
 *   - 快指针遍历数组，遇到非 val 的元素就复制到慢指针位置
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int removeElement(std::vector<int>& nums, int val) {
    int slow = 0;
    for (int fast = 0; fast < static_cast<int>(nums.size()); ++fast) {
        if (nums[fast] != val) {
            nums[slow++] = nums[fast];
        }
    }
    return slow;
}

// 优化版：当要删除的元素很少时
int removeElementV2(std::vector<int>& nums, int val) {
    int left = 0;
    int right = static_cast<int>(nums.size());

    while (left < right) {
        if (nums[left] == val) {
            nums[left] = nums[right - 1];
            --right;
        } else {
            ++left;
        }
    }
    return left;
}

/**
 * 题目7: 加一
 *
 * 解法: 从后向前模拟加法
 * 思路:
 *   - 从最低位开始加 1
 *   - 如果产生进位，继续向前处理
 *   - 如果最高位也产生进位，需要在开头插入 1
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1) (不考虑结果数组)
 */
std::vector<int> plusOne(std::vector<int>& digits) {
    int n = static_cast<int>(digits.size());

    // 从最低位开始处理
    for (int i = n - 1; i >= 0; --i) {
        if (digits[i] < 9) {
            digits[i]++;
            return digits;  // 无进位，直接返回
        }
        digits[i] = 0;  // 产生进位，当前位变 0
    }

    // 如果执行到这里，说明所有位都是 9，需要扩展数组
    // 例如 999 -> 1000
    digits.insert(digits.begin(), 1);
    return digits;
}

// 更优雅的方式（不修改原数组）
std::vector<int> plusOneV2(std::vector<int>& digits) {
    std::vector<int> result = digits;
    int carry = 1;

    for (int i = static_cast<int>(result.size()) - 1; i >= 0 && carry; --i) {
        int sum = result[i] + carry;
        result[i] = sum % 10;
        carry = sum / 10;
    }

    if (carry) {
        result.insert(result.begin(), carry);
    }

    return result;
}

/**
 * 题目8: 最大子数组和
 *
 * 解法1: Kadane 算法（动态规划优化）
 * 思路:
 *   - 维护当前子数组和 currentSum
 *   - 如果 currentSum < 0，则抛弃之前的子数组，从当前元素重新开始
 *   - 记录过程中的最大值
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int maxSubArray(std::vector<int>& nums) {
    int maxSum = nums[0];
    int currentSum = nums[0];

    for (size_t i = 1; i < nums.size(); ++i) {
        // 要么从当前元素重新开始，要么加入之前的子数组
        currentSum = std::max(nums[i], currentSum + nums[i]);
        maxSum = std::max(maxSum, currentSum);
    }

    return maxSum;
}

// 解法2: 标准动态规划
// dp[i] = 以 nums[i] 结尾的最大子数组和
int maxSubArrayDP(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    std::vector<int> dp(n);
    dp[0] = nums[0];
    int maxSum = dp[0];

    for (int i = 1; i < n; ++i) {
        dp[i] = std::max(nums[i], dp[i - 1] + nums[i]);
        maxSum = std::max(maxSum, dp[i]);
    }

    return maxSum;
}

// 解法3: 分治法（面试时可能会问）
// 时间复杂度 O(n log n)
int maxCrossingSum(std::vector<int>& nums, int left, int mid, int right) {
    // 从 mid 向左扩展的最大和
    int leftSum = INT_MIN;
    int sum = 0;
    for (int i = mid; i >= left; --i) {
        sum += nums[i];
        leftSum = std::max(leftSum, sum);
    }

    // 从 mid+1 向右扩展的最大和
    int rightSum = INT_MIN;
    sum = 0;
    for (int i = mid + 1; i <= right; ++i) {
        sum += nums[i];
        rightSum = std::max(rightSum, sum);
    }

    return leftSum + rightSum;
}

int maxSubArrayDivide(std::vector<int>& nums, int left, int right) {
    if (left == right) return nums[left];

    int mid = left + (right - left) / 2;
    int leftMax = maxSubArrayDivide(nums, left, mid);
    int rightMax = maxSubArrayDivide(nums, mid + 1, right);
    int crossMax = maxCrossingSum(nums, left, mid, right);

    return std::max({leftMax, rightMax, crossMax});
}


// ==================== 测试代码 ====================
#include <iostream>
#include <cassert>

void testTwoSum() {
    std::vector<int> nums = {2, 7, 11, 15};
    auto result = twoSum(nums, 9);
    assert(result.size() == 2);
    assert(nums[result[0]] + nums[result[1]] == 9);
    std::cout << "twoSum: PASSED\n";
}

void testReverseString() {
    std::vector<char> s = {'h', 'e', 'l', 'l', 'o'};
    reverseString(s);
    assert(s == std::vector<char>({'o', 'l', 'l', 'e', 'h'}));
    std::cout << "reverseString: PASSED\n";
}

void testIsPalindrome() {
    assert(isPalindrome(121) == true);
    assert(isPalindrome(-121) == false);
    assert(isPalindrome(10) == false);
    assert(isPalindrome(0) == true);
    assert(isPalindrome(12321) == true);
    std::cout << "isPalindrome: PASSED\n";
}

void testIsValid() {
    assert(isValid("()") == true);
    assert(isValid("()[]{}") == true);
    assert(isValid("(]") == false);
    assert(isValid("([)]") == false);
    assert(isValid("{[]}") == true);
    assert(isValid("") == true);
    std::cout << "isValid: PASSED\n";
}

void testMerge() {
    std::vector<int> nums1 = {1, 2, 3, 0, 0, 0};
    std::vector<int> nums2 = {2, 5, 6};
    merge(nums1, 3, nums2, 3);
    assert(nums1 == std::vector<int>({1, 2, 2, 3, 5, 6}));

    std::vector<int> nums3 = {0};
    std::vector<int> nums4 = {1};
    merge(nums3, 0, nums4, 1);
    assert(nums3 == std::vector<int>({1}));
    std::cout << "merge: PASSED\n";
}

void testRemoveElement() {
    std::vector<int> nums = {3, 2, 2, 3};
    int len = removeElement(nums, 3);
    assert(len == 2);

    std::vector<int> nums2 = {0, 1, 2, 2, 3, 0, 4, 2};
    len = removeElement(nums2, 2);
    assert(len == 5);
    std::cout << "removeElement: PASSED\n";
}

void testPlusOne() {
    std::vector<int> digits1 = {1, 2, 3};
    auto result1 = plusOne(digits1);
    assert(result1 == std::vector<int>({1, 2, 4}));

    std::vector<int> digits2 = {9, 9, 9};
    auto result2 = plusOne(digits2);
    assert(result2 == std::vector<int>({1, 0, 0, 0}));

    std::vector<int> digits3 = {0};
    auto result3 = plusOne(digits3);
    assert(result3 == std::vector<int>({1}));
    std::cout << "plusOne: PASSED\n";
}

void testMaxSubArray() {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArray(nums) == 6);

    std::vector<int> nums2 = {1};
    assert(maxSubArray(nums2) == 1);

    std::vector<int> nums3 = {-1};
    assert(maxSubArray(nums3) == -1);

    std::vector<int> nums4 = {5, 4, -1, 7, 8};
    assert(maxSubArray(nums4) == 23);
    std::cout << "maxSubArray: PASSED\n";
}

int main() {
    std::cout << "=== Easy Problems Solution Tests ===\n";

    testTwoSum();
    testReverseString();
    testIsPalindrome();
    testIsValid();
    testMerge();
    testRemoveElement();
    testPlusOne();
    testMaxSubArray();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
