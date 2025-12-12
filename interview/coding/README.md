# Coding Practice

C++ 面试编程题 - 21 个分类，90+ 道经典面试题。

---

## 分类目录

### 算法基础 (Algorithm Basics)

| 编号 | 分类 | 题目数 | 难度 |
|------|------|--------|------|
| 01 | [basic](01-basic/) | 2 | Easy |
| 02 | [linked-list](02-linked-list/) | 6 | Easy-Medium |
| 03 | [tree](03-tree/) | 7 | Medium |
| 04 | [dp](04-dp/) | 5 | Medium-Hard |
| 05 | [string](05-string/) | 5 | Medium |
| 06 | [stack-queue](06-stack-queue/) | 3 | Medium |
| 07 | [hash-design](07-hash-design/) | 5 | Medium |
| 08 | [sorting](08-sorting/) | 4 | Medium |
| 09 | [graph](09-graph/) | 5 | Medium-Hard |

### C++ 特色题 (C++ Specific)

| 编号 | 分类 | 题目数 | 难度 |
|------|------|--------|------|
| 10 | [smart-pointer](10-smart-pointer/) | 4 | Medium |
| 11 | [memory](11-memory/) | 4 | Hard |
| 12 | [oop-design](12-oop-design/) | 7 | Medium |
| 13 | [thread](13-thread/) | 8 | Medium-Hard |
| 14 | [lockfree](14-lockfree/) | 5 | Hard |
| 15 | [template](15-template/) | 4 | Hard |
| 16 | [coroutine](16-coroutine/) | 5 | Hard |

### 算法进阶 (Advanced Algorithm)

| 编号 | 分类 | 题目数 | 难度 |
|------|------|--------|------|
| 17 | [backtracking](17-backtracking/) | 7 | Medium |
| 18 | [two-pointers](18-two-pointers/) | 9 | Easy-Medium |
| 19 | [bit-manipulation](19-bit-manipulation/) | 7 | Medium |
| 20 | [heap](20-heap/) | 5 | Medium |
| 21 | [system-design](21-system-design/) | 5 | Hard |

---

## 文件命名规范

每道题包含两个文件：

```
problem_name.cpp           # 题目描述 + 函数签名
problem_name_solution.cpp  # 完整解答 + 复杂度分析
```

### 题目文件模板

```cpp
/**
 * 题目：XXX
 * 难度：Easy/Medium/Hard
 * 来源：LeetCode XXX / 面试真题
 *
 * 题目描述：
 * ...
 *
 * 示例：
 * 输入：...
 * 输出：...
 *
 * 提示：
 * - 约束条件
 */

// 请实现以下函数
class Solution {
public:
    ReturnType functionName(ParamType param) {
        // TODO: 实现你的解法
    }
};
```

### 解答文件模板

```cpp
/**
 * 解法分析
 *
 * 思路：
 * 1. ...
 * 2. ...
 *
 * 时间复杂度：O(?)
 * 空间复杂度：O(?)
 */

class Solution {
public:
    ReturnType functionName(ParamType param) {
        // 完整实现
    }
};

// 测试用例
#include <cassert>
int main() {
    Solution sol;
    // 测试代码
    return 0;
}
```

---

## 面试高频题

### 必刷 Top 20

1. **链表** - 反转链表、环检测、合并有序链表
2. **树** - 遍历、LCA、路径和
3. **动态规划** - 背包、最长子序列
4. **字符串** - 滑动窗口、回文
5. **设计** - LRU Cache、LFU Cache

### C++ 面试特色

1. **智能指针实现** - unique_ptr、shared_ptr
2. **线程同步** - 生产者消费者、线程池
3. **设计模式** - 单例、工厂、观察者
4. **内存管理** - 内存池、对象池

---

## 练习建议

### 刷题顺序

1. **第一周**：01-basic, 02-linked-list, 18-two-pointers
2. **第二周**：03-tree, 06-stack-queue, 17-backtracking
3. **第三周**：04-dp, 05-string, 20-heap
4. **第四周**：10-smart-pointer, 12-oop-design, 13-thread

### 面试前一天

重点复习：
- LRU Cache (07-hash-design)
- 智能指针 (10-smart-pointer)
- 线程池 (13-thread)
- 单例模式 (12-oop-design)
