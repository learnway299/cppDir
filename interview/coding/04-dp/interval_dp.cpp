/**
 * @file interval_dp.cpp
 * @brief 区间动态规划问题 - 题目文件
 *
 * 区间 DP 的特点是在区间上进行状态转移，
 * 通常 dp[i][j] 表示区间 [i,j] 上的最优解。
 * 遍历顺序：从小区间到大区间。
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

/**
 * 题目1: 最长回文子串
 *
 * 给你一个字符串 s，找到 s 中最长的回文子串。
 *
 * 示例:
 *   输入: s = "babad"
 *   输出: "bab" 或 "aba"
 *
 *   输入: s = "cbbd"
 *   输出: "bb"
 *
 * 状态定义: dp[i][j] = s[i..j] 是否为回文串
 * 状态转移: dp[i][j] = (s[i] == s[j]) && dp[i+1][j-1]
 *
 * @param s 输入字符串
 * @return 最长回文子串
 */
std::string longestPalindrome(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目2: 回文子串数目
 *
 * 给你一个字符串 s，请你统计并返回这个字符串中回文子串的数目。
 *
 * 回文字符串是正着读和倒过来读一样的字符串。
 * 子字符串是字符串中的由连续字符组成的一个序列。
 *
 * 示例:
 *   输入: s = "abc"
 *   输出: 3
 *   解释: 三个回文子串: "a", "b", "c"
 *
 *   输入: s = "aaa"
 *   输出: 6
 *   解释: 6 个回文子串: "a", "a", "a", "aa", "aa", "aaa"
 *
 * @param s 输入字符串
 * @return 回文子串数目
 */
int countSubstrings(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目3: 戳气球
 *
 * 有 n 个气球，编号为 0 到 n - 1，每个气球上都标有一个数字，这些数字存在数组 nums 中。
 *
 * 现在要求你戳破所有的气球。戳破第 i 个气球，你可以获得 nums[i-1] * nums[i] * nums[i+1] 枚硬币。
 * 这里的 i-1 和 i+1 代表和 i 相邻的两个气球的序号。如果 i-1 或 i+1 超出了数组的边界，
 * 那么就当它是一个数字为 1 的气球。
 *
 * 求所能获得硬币的最大数量。
 *
 * 示例:
 *   输入: nums = [3,1,5,8]
 *   输出: 167
 *   解释:
 *     nums = [3,1,5,8] --> [3,5,8] --> [3,8] --> [8] --> []
 *     coins =  3*1*5    +   3*5*8  +  1*3*8 + 1*8*1 = 167
 *
 * 思路: 反向思考，考虑最后一个被戳破的气球
 * dp[i][j] = 戳破开区间 (i,j) 内所有气球能获得的最大硬币数
 *
 * @param nums 气球数字数组
 * @return 最大硬币数
 */
int maxCoins(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目4: 最低票价
 *
 * 在一个火车旅行很受欢迎的国度，你提前一年计划了一些火车旅行。
 * 在接下来的一年里，你要旅行的日子将以一个名为 days 的数组给出。
 * 每一项是一个从 1 到 365 的整数。
 *
 * 火车票有三种不同的销售方式：
 *   - 一张为期一天的通行证售价为 costs[0] 美元
 *   - 一张为期七天的通行证售价为 costs[1] 美元
 *   - 一张为期三十天的通行证售价为 costs[2] 美元
 *
 * 通行证允许数天无限制的旅行。
 *
 * 返回你想要完成在给定的列表 days 中列出的每一天的旅行所需要的最低消费。
 *
 * 示例:
 *   输入: days = [1,4,6,7,8,20], costs = [2,7,15]
 *   输出: 11
 *   解释:
 *     在第 1 天，你花了 costs[0] = $2 买了一张为期 1 天的通行证，它将在第 1 天生效。
 *     在第 3 天，你花了 costs[1] = $7 买了一张为期 7 天的通行证，它将在第 3, 4, ..., 9 天生效。
 *     在第 20 天，你花了 costs[0] = $2 买了一张为期 1 天的通行证，它将在第 20 天生效。
 *     总费用为 $11。
 *
 * @param days 旅行日期数组
 * @param costs 票价数组
 * @return 最低消费
 */
int mincostTickets(std::vector<int>& days, std::vector<int>& costs) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 石子合并
 *
 * 有 n 堆石子排成一排，第 i 堆石子有 stones[i] 个。
 * 每次只能合并相邻的两堆石子，合并的代价为这两堆石子的总数。
 * 求将所有石子合并成一堆的最小代价。
 *
 * 示例:
 *   输入: stones = [3,2,4,1]
 *   输出: 20
 *   解释:
 *     合并 [2,4]，代价 6，得到 [3,6,1]
 *     合并 [3,6]，代价 9，得到 [9,1]
 *     合并 [9,1]，代价 10，得到 [10]
 *     总代价 6 + 9 + 10 = 25（这不是最优解）
 *     最优解：合并 [3,2]->5, [5,4]->9, [9,1]->10, 总代价 5+9+10=24（还不是最优）
 *     更优解需要尝试其他顺序
 *
 * 经典区间 DP 问题
 * dp[i][j] = 合并 [i,j] 范围内石子的最小代价
 *
 * @param stones 石子数量数组
 * @return 最小合并代价
 */
int mergeStones(std::vector<int>& stones) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 合并石头的最低成本（每次合并 K 堆）
 *
 * 有 n 堆石头排成一排，第 i 堆中有 stones[i] 块石头。
 * 每次移动需要将连续的 k 堆石头合并为一堆，而这次移动的成本为这 k 堆中石头的总数。
 *
 * 返回把所有石头合并成一堆的最低成本。如果无法合并成一堆，返回 -1。
 *
 * 示例:
 *   输入: stones = [3,2,4,1], k = 2
 *   输出: 20
 *
 *   输入: stones = [3,2,4,1], k = 3
 *   输出: -1
 *   解释: 任何方法都无法把它们合并成一堆
 *
 * @param stones 石子数量数组
 * @param k 每次合并的堆数
 * @return 最小合并代价，无解返回 -1
 */
int mergeKStones(std::vector<int>& stones, int k) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 多边形三角剖分的最低得分
 *
 * 你有一个凸的 n 边形，其每个顶点都有一个整数值。给定一个整数数组 values，
 * 其中 values[i] 是第 i 个顶点的值（从 0 开始）。
 *
 * 将凸多边形剖分为 n - 2 个三角形。对于每个三角形，该三角形的值是顶点标记的乘积。
 * 三角剖分的分数是所有 n - 2 个三角形的值之和。
 *
 * 返回你能够达到的最低分数的三角剖分。
 *
 * 示例:
 *   输入: values = [1,2,3]
 *   输出: 6
 *   解释: 多边形已经三角化，唯一三角形的分数为 6
 *
 *   输入: values = [3,7,4,5]
 *   输出: 144
 *   解释: 有两种三角剖分：
 *         分数 3*7*5 + 4*7*5 = 245
 *         分数 3*4*5 + 3*4*7 = 144（最低）
 *
 * @param values 顶点值数组
 * @return 最低得分
 */
int minScoreTriangulation(std::vector<int>& values) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目8: 矩阵链乘法
 *
 * 给定 n 个矩阵的维度数组 p，其中第 i 个矩阵的维度是 p[i-1] x p[i]。
 * 求计算矩阵链乘积 M1 * M2 * ... * Mn 所需的最少标量乘法次数。
 *
 * 示例:
 *   输入: p = [10, 30, 5, 60]
 *   解释: 3 个矩阵，维度分别为 10x30, 30x5, 5x60
 *   输出: 4500
 *   解释: (M1 * M2) * M3 = 10*30*5 + 10*5*60 = 1500 + 3000 = 4500
 *         M1 * (M2 * M3) = 30*5*60 + 10*30*60 = 9000 + 18000 = 27000
 *         所以最优顺序是先算 M1*M2
 *
 * @param p 矩阵维度数组
 * @return 最少乘法次数
 */
int matrixChainMultiplication(std::vector<int>& p) {
    // TODO: 实现你的解法
    return 0;
}


// ==================== 测试代码 ====================
#include <cassert>

void testLongestPalindrome() {
    std::string result = longestPalindrome("babad");
    assert(result == "bab" || result == "aba");

    result = longestPalindrome("cbbd");
    assert(result == "bb");

    result = longestPalindrome("a");
    assert(result == "a");

    std::cout << "longestPalindrome: PASSED\n";
}

void testCountSubstrings() {
    assert(countSubstrings("abc") == 3);
    assert(countSubstrings("aaa") == 6);
    assert(countSubstrings("abba") == 6);

    std::cout << "countSubstrings: PASSED\n";
}

void testMaxCoins() {
    std::vector<int> nums = {3, 1, 5, 8};
    assert(maxCoins(nums) == 167);

    nums = {1, 5};
    assert(maxCoins(nums) == 10);

    std::cout << "maxCoins: PASSED\n";
}

void testMincostTickets() {
    std::vector<int> days = {1, 4, 6, 7, 8, 20};
    std::vector<int> costs = {2, 7, 15};
    assert(mincostTickets(days, costs) == 11);

    days = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31};
    costs = {2, 7, 15};
    assert(mincostTickets(days, costs) == 17);

    std::cout << "mincostTickets: PASSED\n";
}

void testMergeStones() {
    std::vector<int> stones = {3, 2, 4, 1};
    int result = mergeStones(stones);
    assert(result == 20);

    std::cout << "mergeStones: PASSED\n";
}

void testMergeKStones() {
    std::vector<int> stones = {3, 2, 4, 1};
    assert(mergeKStones(stones, 2) == 20);

    stones = {3, 2, 4, 1};
    assert(mergeKStones(stones, 3) == -1);

    std::cout << "mergeKStones: PASSED\n";
}

void testMinScoreTriangulation() {
    std::vector<int> values = {1, 2, 3};
    assert(minScoreTriangulation(values) == 6);

    values = {3, 7, 4, 5};
    assert(minScoreTriangulation(values) == 144);

    std::cout << "minScoreTriangulation: PASSED\n";
}

void testMatrixChainMultiplication() {
    std::vector<int> p = {10, 30, 5, 60};
    assert(matrixChainMultiplication(p) == 4500);

    p = {40, 20, 30, 10, 30};
    assert(matrixChainMultiplication(p) == 26000);

    std::cout << "matrixChainMultiplication: PASSED\n";
}

int main() {
    std::cout << "=== Interval DP Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLongestPalindrome();
    // testCountSubstrings();
    // testMaxCoins();
    // testMincostTickets();
    // testMergeStones();
    // testMergeKStones();
    // testMinScoreTriangulation();
    // testMatrixChainMultiplication();

    std::cout << "All tests passed!\n";
    return 0;
}
