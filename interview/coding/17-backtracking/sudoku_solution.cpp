/**
 * @file sudoku_solution.cpp
 * @brief 数独求解 - 参考答案
 */
#include "sudoku.h"
#include <iostream>
#include <cassert>

namespace Sudoku {

// ==================== 辅助函数 ====================
static bool isValid(std::vector<std::vector<char>>& board, int row, int col, char c) {
    for (int i = 0; i < 9; ++i) {
        if (board[row][i] == c) return false;
        if (board[i][col] == c) return false;
        int r = 3 * (row / 3) + i / 3;
        int cl = 3 * (col / 3) + i % 3;
        if (board[r][cl] == c) return false;
    }
    return true;
}

static bool solve(std::vector<std::vector<char>>& board) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] == '.') {
                for (char c = '1'; c <= '9'; ++c) {
                    if (isValid(board, i, j, c)) {
                        board[i][j] = c;
                        if (solve(board)) return true;
                        board[i][j] = '.';
                    }
                }
                return false;
            }
        }
    }
    return true;
}

// ==================== 参考答案实现 ====================

void solveSudokuSolution(std::vector<std::vector<char>>& board) {
    solve(board);
}

bool isValidSudokuSolution(std::vector<std::vector<char>>& board) {
    std::vector<int> rows(9, 0), cols(9, 0), boxes(9, 0);

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] != '.') {
                int num = board[i][j] - '1';
                int mask = 1 << num;
                int boxIdx = (i / 3) * 3 + j / 3;

                if ((rows[i] & mask) || (cols[j] & mask) || (boxes[boxIdx] & mask)) {
                    return false;
                }

                rows[i] |= mask;
                cols[j] |= mask;
                boxes[boxIdx] |= mask;
            }
        }
    }
    return true;
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== Sudoku Tests ===" << std::endl;

    // 测试解数独
    {
        std::vector<std::vector<char>> board = {
            {'5','3','.','.','7','.','.','.','.'},
            {'6','.','.','1','9','5','.','.','.'},
            {'.','9','8','.','.','.','.','6','.'},
            {'8','.','.','.','6','.','.','.','3'},
            {'4','.','.','8','.','3','.','.','1'},
            {'7','.','.','.','2','.','.','.','6'},
            {'.','6','.','.','.','.','2','8','.'},
            {'.','.','.','4','1','9','.','.','5'},
            {'.','.','.','.','8','.','.','7','9'}
        };

        solveSudokuSolution(board);

        // 验证解的正确性
        assert(isValidSudokuSolution(board));
        // 验证没有空格
        bool hasEmpty = false;
        for (auto& row : board) {
            for (char c : row) {
                if (c == '.') hasEmpty = true;
            }
        }
        assert(!hasEmpty);
        std::cout << "  [PASS] Solve Sudoku" << std::endl;
    }

    // 测试验证数独
    {
        std::vector<std::vector<char>> valid = {
            {'5','3','4','6','7','8','9','1','2'},
            {'6','7','2','1','9','5','3','4','8'},
            {'1','9','8','3','4','2','5','6','7'},
            {'8','5','9','7','6','1','4','2','3'},
            {'4','2','6','8','5','3','7','9','1'},
            {'7','1','3','9','2','4','8','5','6'},
            {'9','6','1','5','3','7','2','8','4'},
            {'2','8','7','4','1','9','6','3','5'},
            {'3','4','5','2','8','6','1','7','9'}
        };
        assert(isValidSudokuSolution(valid));
        std::cout << "  [PASS] Validate Sudoku" << std::endl;
    }

    std::cout << "=== All Sudoku Tests Passed! ===" << std::endl;
}

} // namespace Sudoku

/**
 * 关键要点：
 *
 * 1. 约束条件：
 *    - 每行 1-9 各出现一次
 *    - 每列 1-9 各出现一次
 *    - 每个 3x3 宫格 1-9 各出现一次
 *
 * 2. 基本回溯：
 *    - 找到空格，尝试 1-9
 *    - 检查是否有效
 *    - 递归解决剩余部分
 *    - 回溯时恢复空格
 *
 * 3. 宫格索引：
 *    - boxIdx = (row / 3) * 3 + col / 3
 */
