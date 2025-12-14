/**
 * @file topological_sort.h
 * @brief 拓扑排序相关题目 - 题目说明与声明
 *
 * 包含题目：
 * 1. 课程表 (LeetCode 207) - 判断是否可以完成所有课程
 * 2. 课程表 II (LeetCode 210) - 返回学习顺序
 * 3. 课程表 IV (LeetCode 1462) - 查询先决条件
 * 4. 火星词典 (LeetCode 269) - 外星语言排序
 * 5. 并行课程 (LeetCode 1136) - 最少学期数
 */

#ifndef TOPOLOGICAL_SORT_H
#define TOPOLOGICAL_SORT_H

#include <vector>
#include <string>

namespace TopologicalSort {

// ==================== 题目声明 ====================

/**
 * 题目1: 课程表
 * LeetCode 207
 *
 * 你这个学期必须选修 numCourses 门课程，记为 0 到 numCourses - 1。
 * 在选修某些课程之前需要一些先修课程。
 * 先修课程按数组 prerequisites 给出，其中 prerequisites[i] = [ai, bi]，
 * 表示如果要学习课程 ai 则必须先学习课程 bi��
 *
 * 请你判断是否可能完成所有课程的学习？
 *
 * 思路: Kahn算法（BFS）或 DFS 检测环
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * 示例:
 *   输入: numCourses = 2, prerequisites = [[1,0]]
 *   输出: true
 *
 *   输入: numCourses = 2, prerequisites = [[1,0],[0,1]]
 *   输出: false (存在环)
 */
bool canFinish(int numCourses, std::vector<std::vector<int>>& prerequisites);
bool canFinishSolution(int numCourses, std::vector<std::vector<int>>& prerequisites);

/**
 * 题目2: 课程表 II
 * LeetCode 210
 *
 * 返回你为了学完所有课程所安排的学习顺序。
 * 可能会有多个正确的顺序，只需返回其中一种。
 * 如果不可能完成所有课程，返回空数组。
 *
 * 思路: Kahn算法，维护入度数组
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * 示例:
 *   输入: numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]
 *   输出: [0,2,1,3] 或 [0,1,2,3]
 */
std::vector<int> findOrder(int numCourses, std::vector<std::vector<int>>& prerequisites);
std::vector<int> findOrderSolution(int numCourses, std::vector<std::vector<int>>& prerequisites);

/**
 * 题目3: 课程表 IV
 * LeetCode 1462
 *
 * 给定课程的先决条件和一系列查询，判断课程 queries[i][0] 是否是 queries[i][1] 的先决条件。
 *
 * 思路: 传递闭包 (Floyd-Warshall 思想) 或多源 BFS
 * 时间复杂度: O(n^3) 或 O(n * (V + E))
 * 空间复杂度: O(n^2)
 *
 * 示例:
 *   输入: numCourses = 2, prerequisites = [[1,0]], queries = [[0,1],[1,0]]
 *   输出: [false, true]
 */
std::vector<bool> checkIfPrerequisite(int numCourses,
                                       std::vector<std::vector<int>>& prerequisites,
                                       std::vector<std::vector<int>>& queries);
std::vector<bool> checkIfPrerequisiteSolution(int numCourses,
                                               std::vector<std::vector<int>>& prerequisites,
                                               std::vector<std::vector<int>>& queries);

/**
 * 题目4: 火星词典
 * LeetCode 269
 *
 * 给定一组按外星语言字典序排序的单词，推断出字母的顺序。
 *
 * 思路: 构建字符依赖图，拓扑排序
 * 时间复杂度: O(C) C为所有单词的总字符数
 * 空间复杂度: O(1) 最多26个字母
 *
 * 示例:
 *   输入: words = ["wrt","wrf","er","ett","rftt"]
 *   输出: "wertf"
 */
std::string alienOrder(std::vector<std::string>& words);
std::string alienOrderSolution(std::vector<std::string>& words);

/**
 * 题目5: 并行课程
 * LeetCode 1136
 *
 * 给定 n 门课程和依赖关系，每个学期可以选修任意数量的没有先决条件的课程。
 * 返回完成所有课程所需的最少学期数。如果无法完成，返回 -1。
 *
 * 思路: BFS 层序遍历，层数即为学期数
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * 示例:
 *   输入: n = 3, relations = [[1,3],[2,3]]
 *   输出: 2
 */
int minimumSemesters(int n, std::vector<std::vector<int>>& relations);
int minimumSemestersSolution(int n, std::vector<std::vector<int>>& relations);

// ==================== 测试函数声明 ====================

void testTopologicalSort();          // 测试面试者实现
void testTopologicalSortSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTopologicalSort();
    testTopologicalSortSolution();
}

} // namespace TopologicalSort

#endif // TOPOLOGICAL_SORT_H
