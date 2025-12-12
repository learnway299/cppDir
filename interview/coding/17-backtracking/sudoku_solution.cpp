/**
 * @file sudoku_solution.cpp
 * @brief 数独求解 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        solve(board);
    }

private:
    bool solve(vector<vector<char>>& board) {
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

    bool isValid(vector<vector<char>>& board, int row, int col, char c) {
        for (int i = 0; i < 9; ++i) {
            if (board[row][i] == c) return false;
            if (board[i][col] == c) return false;
            int r = 3 * (row / 3) + i / 3;
            int cl = 3 * (col / 3) + i % 3;
            if (board[r][cl] == c) return false;
        }
        return true;
    }
};

// 位运算优化版本
class SolutionOptimized {
public:
    void solveSudoku(vector<vector<char>>& board) {
        rows.assign(9, 0);
        cols.assign(9, 0);
        boxes.assign(9, 0);

        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] != '.') {
                    int num = board[i][j] - '1';
                    int mask = 1 << num;
                    rows[i] |= mask;
                    cols[j] |= mask;
                    boxes[(i / 3) * 3 + j / 3] |= mask;
                }
            }
        }
        solve(board, 0, 0);
    }

private:
    vector<int> rows, cols, boxes;

    bool solve(vector<vector<char>>& board, int r, int c) {
        if (r == 9) return true;
        int nr = (c == 8) ? r + 1 : r;
        int nc = (c == 8) ? 0 : c + 1;

        if (board[r][c] != '.') return solve(board, nr, nc);

        int boxIdx = (r / 3) * 3 + c / 3;
        int available = ~(rows[r] | cols[c] | boxes[boxIdx]) & 0x1FF;

        while (available) {
            int num = __builtin_ctz(available);
            int mask = 1 << num;
            available &= ~mask;

            board[r][c] = '1' + num;
            rows[r] |= mask;
            cols[c] |= mask;
            boxes[boxIdx] |= mask;

            if (solve(board, nr, nc)) return true;

            board[r][c] = '.';
            rows[r] &= ~mask;
            cols[c] &= ~mask;
            boxes[boxIdx] &= ~mask;
        }
        return false;
    }
};

int main() {
    vector<vector<char>> board = {
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

    Solution().solveSudoku(board);

    cout << "Solved Sudoku:\n";
    for (auto& row : board) {
        for (char c : row) cout << c << ' ';
        cout << '\n';
    }
    return 0;
}
