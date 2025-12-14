/**
 * @file test_all.cpp
 * @brief 17-backtracking 所有模块测试
 */

#include "combination.h"
#include "generate_parentheses.h"
#include "n_queens.h"
#include "palindrome_partition.h"
#include "permutation.h"
#include "sudoku.h"
#include "word_search.h"

#include <iostream>

//  main() {
//     std::cout << "========================================" << std::endl;
//     std::cout << "   17-Backtracking 模块测试" << std::endl;
//     std::cout << "========================================" << std::endl;
//
//     try {
//         // 测试所有模块
//         Combination::runTests();
//         GenerateParentheses::runTests();
//         NQueens::runTests();
//         PalindromePartition::runTests();
//         Permutation::runTests();
//         Sudoku::runTests();
//         WordSearch::runTests();
//
//         std::cout << "\n========================================" << std::endl;
//         std::cout << "   所有测试通过！" << std::endl;
//         std::cout << "========================================" << std::endl;
//         return 0;
//     } catch (const std::exception& e) {
//         std::cerr << "\n测试失败: " << e.what() << std::endl;
//         return 1;
//     }
// }int
