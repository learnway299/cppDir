/**
 * @file topological_sort.cpp
 * @brief 拓扑排序 - 面试题
 *
 * 包含题目：
 * 1. 课程表 (LeetCode 207)
 * 2. 课程表 II (LeetCode 210)
 * 3. 课程表 IV (LeetCode 1462)
 * 4. 外星文字典 (LeetCode 269)
 */

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

/**
 * 题目1：课程表 (LeetCode 207)
 *
 * 你这个学期必须选修 numCourses 门课程，记为 0 到 numCourses - 1。
 * 给你一个数组 prerequisites，其中 prerequisites[i] = [ai, bi]，
 * 表示在选修课程 ai 前必须先选修 bi。
 * 请你判断是否可能完成所有课程的学习？
 *
 * 示例：
 * 输入：numCourses = 2, prerequisites = [[1,0]]
 * 输出：true
 *
 * 输入：numCourses = 2, prerequisites = [[1,0],[0,1]]
 * 输出：false（存在环）
 */
class CanFinish {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        // TODO: 实现判断是否能完成所有课程
        return false;
    }
};

/**
 * 题目2：课程表 II (LeetCode 210)
 *
 * 返回你为了学完所有课程所安排的学习顺序。可能会有多个正确的顺序，
 * 你只要返回任意一种就可以了。如果不可能完成所有课程，返回一个空数组。
 *
 * 示例：
 * 输入：numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]
 * 输出：[0,2,1,3] 或 [0,1,2,3]
 */
class FindOrder {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        // TODO: 返回课程学习顺序
        return {};
    }
};

/**
 * 题目3：课程表 IV (LeetCode 1462)
 *
 * 给你一个整数 numCourses 表示课程数量和一个数组 prerequisites 表示先决条件。
 * 另给你一个数组 queries，其中 queries[j] = [uj, vj]。
 * 对于第 j 个查询，判断课程 uj 是否是课程 vj 的先决条件。
 *
 * 示例：
 * 输入：numCourses = 2, prerequisites = [[1,0]], queries = [[0,1],[1,0]]
 * 输出：[false, true]
 */
class CheckIfPrerequisite {
public:
    vector<bool> checkIfPrerequisite(int numCourses,
                                      vector<vector<int>>& prerequisites,
                                      vector<vector<int>>& queries) {
        // TODO: 实现先决条件查询
        return {};
    }
};

/**
 * 题目4：外星文字典 (LeetCode 269)
 *
 * 现有一种使用英语字母的外星文语言，这门语言的字母顺序与英语顺序不同。
 * 给定一个字符串列表 words，作为这门语言的词典，words 已经按这门新语言的字母顺序进行了排序。
 * 请你根据该词典还原出此语言中已知的字母顺序。
 *
 * 示例：
 * 输入：words = ["wrt","wrf","er","ett","rftt"]
 * 输出："wertf"
 */
class AlienOrder {
public:
    string alienOrder(vector<string>& words) {
        // TODO: 实现外星文字典排序
        return "";
    }
};

/**
 * 题目5：并行课程 (LeetCode 1136)
 *
 * 给你一个整数 n 表示某所大学的课程总数，数组 relations 表示先决条件。
 * relations[i] = [xi, yi] 表示课程 xi 必须在课程 yi 之前完成。
 * 在一个学期内，你可以学习任意数量的课程，但必须已学习所有先决课程。
 * 返回学习完所有课程所需的最少学期数。如果无法完成所有课程，返回 -1。
 *
 * 示例：
 * 输入：n = 3, relations = [[1,3],[2,3]]
 * 输出：2
 */
class MinimumSemesters {
public:
    int minimumSemesters(int n, vector<vector<int>>& relations) {
        // TODO: 实现最少学期数计算
        return -1;
    }
};

int main() {
    return 0;
}
