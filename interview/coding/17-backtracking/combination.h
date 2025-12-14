/**
 * @file combination.h
 * @brief 组合问题 - 声明
 */
#ifndef COMBINATION_H
#define COMBINATION_H

#include <vector>

namespace Combination {

// 面试者实现的函数
std::vector<std::vector<int>> combine(int n, int k);
std::vector<std::vector<int>> combinationSum(std::vector<int>& candidates, int target);
std::vector<std::vector<int>> combinationSum2(std::vector<int>& candidates, int target);
std::vector<std::vector<int>> combinationSum3(int k, int n);
std::vector<std::vector<int>> subsets(std::vector<int>& nums);
std::vector<std::vector<int>> subsetsWithDup(std::vector<int>& nums);

// 参考答案函数
std::vector<std::vector<int>> combineSolution(int n, int k);
std::vector<std::vector<int>> combinationSumSolution(std::vector<int>& candidates, int target);
std::vector<std::vector<int>> combinationSum2Solution(std::vector<int>& candidates, int target);
std::vector<std::vector<int>> combinationSum3Solution(int k, int n);
std::vector<std::vector<int>> subsetsSolution(std::vector<int>& nums);
std::vector<std::vector<int>> subsetsWithDupSolution(std::vector<int>& nums);

// 测试函数
void runTests();

} // namespace Combination

#endif // COMBINATION_H
