/**
 * @file permutation.h
 * @brief 排列问题 - 声明
 */
#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <vector>

namespace Permutation {

// 面试者实现的函数
std::vector<std::vector<int>> permute(std::vector<int>& nums);
std::vector<std::vector<int>> permuteUnique(std::vector<int>& nums);
void nextPermutation(std::vector<int>& nums);

// 参考答案函数
std::vector<std::vector<int>> permuteSolution(std::vector<int>& nums);
std::vector<std::vector<int>> permuteUniqueSolution(std::vector<int>& nums);
void nextPermutationSolution(std::vector<int>& nums);

// 测试函数
void runTests();

} // namespace Permutation

#endif // PERMUTATION_H
