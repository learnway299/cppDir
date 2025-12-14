/**
 * @file sudoku.h
 * @brief 数独求解 - 声明
 */
#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>

namespace Sudoku {

// 面试者实现的函数
void solveSudoku(std::vector<std::vector<char>>& board);
bool isValidSudoku(std::vector<std::vector<char>>& board);

// 参考答案函数
void solveSudokuSolution(std::vector<std::vector<char>>& board);
bool isValidSudokuSolution(std::vector<std::vector<char>>& board);

// 测试函数
void runTests();

} // namespace Sudoku

#endif // SUDOKU_H
