/**
 * @file n_queens_solution.cpp
 * @brief N 皇后问题 - 参考答案
 */
#include "n_queens.h"
#include <unordered_set>
#include <iostream>
#include <cassert>

namespace NQueens {

// ==================== 辅助函数 ====================
static bool isValid(std::vector<std::string>& board, int row, int col, int n) {
    // 检查列
    for (int i = 0; i < row; ++i) {
        if (board[i][col] == 'Q') return false;
    }
    // 检查左上对角线
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j) {
        if (board[i][j] == 'Q') return false;
    }
    // 检查右上对角线
    for (int i = row - 1, j = col + 1; i >= 0 && j < n; --i, ++j) {
        if (board[i][j] == 'Q') return false;
    }
    return true;
}

static void backtrackSolve(std::vector<std::vector<std::string>>& result,
                            std::vector<std::string>& board, int row, int n) {
    if (row == n) {
        result.push_back(board);
        return;
    }
    for (int col = 0; col < n; ++col) {
        if (isValid(board, row, col, n)) {
            board[row][col] = 'Q';
            backtrackSolve(result, board, row + 1, n);
            board[row][col] = '.';
        }
    }
}

static void backtrackCount(int& count, std::vector<int>& queens, int row, int n,
                            std::unordered_set<int>& cols,
                            std::unordered_set<int>& diag1,
                            std::unordered_set<int>& diag2) {
    if (row == n) {
        ++count;
        return;
    }
    for (int col = 0; col < n; ++col) {
        int d1 = row - col;
        int d2 = row + col;
        if (cols.count(col) || diag1.count(d1) || diag2.count(d2)) {
            continue;
        }
        queens[row] = col;
        cols.insert(col);
        diag1.insert(d1);
        diag2.insert(d2);
        backtrackCount(count, queens, row + 1, n, cols, diag1, diag2);
        queens[row] = -1;
        cols.erase(col);
        diag1.erase(d1);
        diag2.erase(d2);
    }
}

// ==================== 参考答案实现 ====================

std::vector<std::vector<std::string>> solveNQueensSolution(int n) {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> board(n, std::string(n, '.'));
    backtrackSolve(result, board, 0, n);
    return result;
}

int totalNQueensSolution(int n) {
    int count = 0;
    std::vector<int> queens(n, -1);
    std::unordered_set<int> cols, diag1, diag2;
    backtrackCount(count, queens, 0, n, cols, diag1, diag2);
    return count;
}

bool canPlaceSolution(std::vector<std::string>& board, int row, int col) {
    int n = static_cast<int>(board.size());
    return isValid(board, row, col, n);
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== N Queens Tests ===" << std::endl;

    // 测试 N 皇后
    {
        auto solutions = solveNQueensSolution(4);
        assert(solutions.size() == 2);
        std::cout << "  [PASS] Solve N Queens (n=4)" << std::endl;
    }

    // 测试计数版本
    {
        assert(totalNQueensSolution(4) == 2);
        assert(totalNQueensSolution(8) == 92);
        std::cout << "  [PASS] Total N Queens" << std::endl;
    }

    // 测试 canPlace
    {
        std::vector<std::string> board = {
            "Q...",
            "....",
            "....",
            "...."
        };
        assert(canPlaceSolution(board, 1, 0) == false);  // 同一列
        assert(canPlaceSolution(board, 1, 2) == true);   // 可以放置
        assert(canPlaceSolution(board, 1, 1) == false);  // 对角线
        std::cout << "  [PASS] Can Place" << std::endl;
    }

    std::cout << "=== All N Queens Tests Passed! ===" << std::endl;
}

} // namespace NQueens

/**
 * 关键要点：
 *
 * 1. 问题分析：
 *    - 每行只能放一个皇后
 *    - 按行搜索，每行选择一列
 *    - 需要检查列、主对角线、副对角线
 *
 * 2. 冲突检测：
 *    - 列冲突：相同列
 *    - 主对角线：row - col 相同
 *    - 副对角线：row + col 相同
 *
 * 3. 优化技巧：
 *    - 使用集合 O(1) 检查冲突
 *    - 只存储皇后位置，不存储整个棋盘
 *
 * 4. 时间复杂度：
 *    - O(N!) 上界
 *    - 实际由于剪枝远小于此
 */
