# C++ Interview Guide
```
狗尾续貂 博君一笑

C++ 面试指南 - 系统性准备 C++ 与 Qt 面试的完整资料库。
```

## 目录结构

```
interview/
├── README.md                 # 本文件 - 面试指南总览
├── theory/                   # 理论知识部分
│   ├── cpp-core/            # C++ 核心知识 (26 个主题)
│   │   ├── README.md
│   │   ├── 01-basic-syntax.md
│   │   └── ...
│   └── qt-framework/        # Qt 框架知识 (18 个主题)
│       ├── README.md
│       ├── 01-signal-slot.md
│       └── ...
└── coding/                   # 编程实战部分
    ├── README.md
    ├── 01-basic/            # 基础题
    ├── 02-linked-list/      # 链表
    ├── 03-tree/             # 树
    ├── 04-dp/               # 动态规划
    ├── 05-string/           # 字符串
    ├── 06-stack-queue/      # 栈与队列
    ├── 07-hash-design/      # 哈希与设计
    ├── 08-sorting/          # 排序
    ├── 09-graph/            # 图
    ├── 10-smart-pointer/    # 智能指针实现
    ├── 11-memory/           # 内存管理
    ├── 12-oop-design/       # OOP 设计模式
    ├── 13-thread/           # 多线程编程
    ├── 14-lockfree/         # 无锁编程
    ├── 15-template/         # 模板元编程
    ├── 16-coroutine/        # 协程
    ├── 17-backtracking/     # 回溯
    ├── 18-two-pointers/     # 双指针
    ├── 19-bit-manipulation/ # 位运算
    ├── 20-heap/             # 堆
    └── 21-system-design/    # 系统设计
```

---

## 内容概览

### 理论部分 (Theory)

#### C++ 核心知识 (26 个主题)

| 层级 | 主题数 | 内容 |
|------|--------|------|
| 基础层 | 11 | 语法、类型、函数、OOP、内存、STL、模板、多线程基础 |
| 进阶层 | 7 | 高级多线程、编译链接、异常、现代C++、性能优化、调试 |
| 深度层 | 8 | ABI、UB、位运算、字符串编码、IO、时间、元编程、跨语言 |

#### Qt 框架知识 (18 个主题)

| 层级 | 主题数 | 内容 |
|------|--------|------|
| 基础层 | 6 | 信号槽、对象模型、元对象、事件系统、事件循环、消息机制 |
| 进阶层 | 6 | 多线程、Model/View、图形、网络IO、高级主题、控件 |
| 深度层 | 6 | 容器、样式主题、国际化、内部机制、现代C++集成、调试 |

### 编程部分 (Coding)

共 21 个分类，约 90+ 道经典面试题，每题包含：
- 问题描述与分析
- 解题思路
- 完整代码实现
- 复杂度分析
- 常见变体与扩展

---

## 使用建议

### 学习路径

1. **基础巩固** (1-2 周)
   - 理论：C++ 基础层 11 个主题
   - Coding：01-basic, 02-linked-list, 03-tree

2. **核心强化** (2-3 周)
   - 理论：C++ 进阶层 + Qt 基础层
   - Coding：04-dp, 05-string, 06-stack-queue, 07-hash-design

3. **深度提升** (2-3 周)
   - 理论：C++ 深度层 + Qt 进阶/深度层
   - Coding：10-smart-pointer, 11-memory, 12-oop-design, 13-thread

4. **综合冲刺** (1-2 周)
   - 回顾所有高频考点
   - Coding：14-lockfree, 15-template, 21-system-design

### 面试前复习清单

- [ ] C++ 内存模型与智能指针
- [ ] 虚函数表与多态实现
- [ ] 模板与 SFINAE
- [ ] 多线程同步原语
- [ ] STL 容器底层实现
- [ ] 常见设计模式
- [ ] Qt 信号槽机制
- [ ] 手写代码：LRU Cache、智能指针、线程池

---

## 参考资源

- [C++ Reference](https://en.cppreference.com/)
- [Qt Documentation](https://doc.qt.io/)
- [LeetCode](https://leetcode.com/)
- Effective C++ / Effective Modern C++
- C++ Concurrency in Action

---

## 更新日志

- 2025-12: 初始版本创建
