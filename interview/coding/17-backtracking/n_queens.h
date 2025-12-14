/**
 * @file n_queens.h
 * @brief N 皇后问题 - 声明
 */
#ifndef N_QUEENS_H
#define N_QUEENS_H

#include <vector>
#include <string>

namespace NQueens {

// 面试者实现的函数
std::vector<std::vector<std::string>> solveNQueens(int n);
int totalNQueens(int n);
bool canPlace(std::vector<std::string>& board, int row, int col);

// 参考答案函数
std::vector<std::vector<std::string>> solveNQueensSolution(int n);
int totalNQueensSolution(int n);
bool canPlaceSolution(std::vector<std::string>& board, int row, int col);

// 测试函数
void runTests();

} // namespace NQueens

#endif // N_QUEENS_H
