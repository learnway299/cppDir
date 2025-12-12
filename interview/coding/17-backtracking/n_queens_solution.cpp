/**
 * @file n_queens_solution.cpp
 * @brief N 皇后问题 - 解答
 */
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

using namespace std;

// ==================== 题目1: N 皇后 ====================
class Solution51 {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> result;
        vector<string> board(n, string(n, '.'));
        backtrack(result, board, 0, n);
        return result;
    }

private:
    void backtrack(vector<vector<string>>& result, vector<string>& board,
                   int row, int n) {
        if (row == n) {
            result.push_back(board);
            return;
        }

        for (int col = 0; col < n; ++col) {
            if (isValid(board, row, col, n)) {
                board[row][col] = 'Q';
                backtrack(result, board, row + 1, n);
                board[row][col] = '.';
            }
        }
    }

    bool isValid(vector<string>& board, int row, int col, int n) {
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
};

// ==================== 优化版本：使用集合 ====================
class Solution51Optimized {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> result;
        vector<int> queens(n, -1);  // queens[i] = j 表示第 i 行皇后在第 j 列
        unordered_set<int> cols, diag1, diag2;
        backtrack(result, queens, 0, n, cols, diag1, diag2);
        return result;
    }

private:
    void backtrack(vector<vector<string>>& result, vector<int>& queens,
                   int row, int n,
                   unordered_set<int>& cols,
                   unordered_set<int>& diag1,  // 主对角线 (row - col)
                   unordered_set<int>& diag2)  // 副对角线 (row + col)
    {
        if (row == n) {
            result.push_back(generateBoard(queens, n));
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

            backtrack(result, queens, row + 1, n, cols, diag1, diag2);

            queens[row] = -1;
            cols.erase(col);
            diag1.erase(d1);
            diag2.erase(d2);
        }
    }

    vector<string> generateBoard(vector<int>& queens, int n) {
        vector<string> board(n, string(n, '.'));
        for (int i = 0; i < n; ++i) {
            board[i][queens[i]] = 'Q';
        }
        return board;
    }
};

// ==================== 题目2: N 皇后 II ====================
class Solution52 {
public:
    int totalNQueens(int n) {
        int count = 0;
        vector<int> queens(n, -1);
        unordered_set<int> cols, diag1, diag2;
        backtrack(count, queens, 0, n, cols, diag1, diag2);
        return count;
    }

private:
    void backtrack(int& count, vector<int>& queens, int row, int n,
                   unordered_set<int>& cols,
                   unordered_set<int>& diag1,
                   unordered_set<int>& diag2) {
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

            backtrack(count, queens, row + 1, n, cols, diag1, diag2);

            queens[row] = -1;
            cols.erase(col);
            diag1.erase(d1);
            diag2.erase(d2);
        }
    }
};

// ==================== 位运算优化版本 ====================
class Solution52Bitwise {
public:
    int totalNQueens(int n) {
        int count = 0;
        int limit = (1 << n) - 1;  // n 位全 1
        backtrack(count, limit, 0, 0, 0);
        return count;
    }

private:
    void backtrack(int& count, int limit, int cols, int diag1, int diag2) {
        if (cols == limit) {
            ++count;
            return;
        }

        // 可以放置的位置
        int avail = limit & ~(cols | diag1 | diag2);

        while (avail) {
            int pos = avail & (-avail);  // 取最低位的 1
            avail ^= pos;  // 移除这个位

            backtrack(count, limit,
                      cols | pos,
                      (diag1 | pos) << 1,
                      (diag2 | pos) >> 1);
        }
    }
};

// ==================== 测试代码 ====================
void printBoard(const vector<string>& board) {
    for (const auto& row : board) {
        cout << row << "\n";
    }
    cout << "\n";
}

int main() {
    // N 皇后
    cout << "=== 4-Queens Solutions ===\n";
    auto solutions = Solution51().solveNQueens(4);
    for (const auto& board : solutions) {
        printBoard(board);
    }

    // 计数
    cout << "=== N-Queens Count ===\n";
    for (int n = 1; n <= 10; ++n) {
        cout << "n=" << n << ": " << Solution52Bitwise().totalNQueens(n) << " solutions\n";
    }

    return 0;
}

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
 *    - 位运算进一步优化
 *
 * 4. 位运算版本：
 *    - 用整数的每一位表示列
 *    - cols: 已占用的列
 *    - diag1: 主对角线冲突（左移传递）
 *    - diag2: 副对角线冲突（右移传递）
 *    - x & (-x): 取最低位的 1
 *
 * 5. 时间复杂度：
 *    - O(N!) 上界
 *    - 实际由于剪枝远小于此
 *
 * 6. N 皇后解的数量：
 *    - n=4: 2
 *    - n=8: 92
 *    - n=10: 724
 */
