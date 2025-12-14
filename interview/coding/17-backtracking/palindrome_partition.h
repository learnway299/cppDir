/**
 * @file palindrome_partition.h
 * @brief 分割回文串 - 声明
 */
#ifndef PALINDROME_PARTITION_H
#define PALINDROME_PARTITION_H

#include <vector>
#include <string>

namespace PalindromePartition {

// 面试者实现的函数
std::vector<std::vector<std::string>> partition(const std::string& s);
int minCut(const std::string& s);

// 参考答案函数
std::vector<std::vector<std::string>> partitionSolution(const std::string& s);
int minCutSolution(const std::string& s);

// 测试函数
void runTests();

} // namespace PalindromePartition

#endif // PALINDROME_PARTITION_H
