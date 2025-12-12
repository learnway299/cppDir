# C++ Core Knowledge

C++ 核心知识 - 面试必备的 26 个主题。

---

## 知识体系

### 基础层 (Basic) - 11 个主题

必须掌握的核心知识，是面试的基本门槛。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [基础语法](01-basic-syntax.md) | const、static、指针引用、作用域 |
| 02 | [类型系统](02-type-system.md) | 基本类型、类型转换、类型推导 |
| 03 | [函数](03-function.md) | 重载、默认参数、函数指针、lambda |
| 04 | [OOP 基础](04-oop-basic.md) | 类、构造析构、访问控制 |
| 05 | [OOP 进阶](05-oop-advanced.md) | 继承、多态、虚函数、RTTI |
| 06 | [内存管理](06-memory.md) | 堆栈、new/delete、智能指针、RAII |
| 07 | [STL 容器](07-stl-container.md) | 顺序容器、关联容器、容器适配器 |
| 08 | [STL 算法](08-stl-algorithm.md) | STL 算法与迭代器 |
| 09 | [模板基础](09-template-basic.md) | 函数模板、类模板、特化 |
| 10 | [模板进阶](10-template-advanced.md) | SFINAE、变参模板、元编程 |
| 11 | [多线程基础](11-thread-basic.md) | thread、mutex、条件变量 |

### 进阶层 (Intermediate) - 7 个主题

深入理解 C++ 底层机制，体现技术深度。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 12 | [多线程进阶](12-thread-advanced.md) | 原子操作、内存序、无锁编程 |
| 13 | [编译与链接](13-compile-link.md) | 预处理、编译单元、符号、动态库 |
| 14 | [异常处理](14-exception.md) | 异常机制与错误处理策略 |
| 15 | [现代 C++](15-modern-cpp.md) | C++11/14/17/20/23 新特性 |
| 16 | [性能优化](16-performance.md) | 缓存、内联、分支预测 |
| 17 | [调试工具](17-debug-tools.md) | GDB、Sanitizer、Profiler |
| 18 | [设计模式](18-design-pattern.md) | RAII、Pimpl、CRTP |

### 深度层 (Advanced) - 8 个主题

高级话题，展示专家级理解。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 19 | [ABI](19-abi.md) | ABI 与二进制兼容性 |
| 20 | [未定义行为](20-undefined-behavior.md) | UB 陷阱与防范 |
| 21 | [位运算](21-bit-operations.md) | 位操作与底层技巧 |
| 22 | [字符串编码](22-string-encoding.md) | 字符集、编码、std::string |
| 23 | [文件与 IO](23-file-io.md) | 文件操作、流、内存映射 |
| 24 | [时间与日期](24-time-date.md) | chrono、时区、高精度计时 |
| 25 | [元编程](25-reflection-metaprogramming.md) | 反射、编译期计算 |
| 26 | [跨语言互操作](26-cross-language.md) | C/Python/Rust 互操作 |

---

## 学习建议

### 面试重点 (必须掌握)

1. **内存管理** - 智能指针、RAII、内存泄漏
2. **多态机制** - 虚函数表、动态绑定
3. **模板** - 基本用法、SFINAE、类型萃取
4. **多线程** - 同步原语、死锁、条件变量
5. **STL** - 容器选择、迭代器失效、算法复杂度

### 加分项 (体现深度)

1. **内存序与原子操作**
2. **模板元编程**
3. **ABI 兼容性**
4. **性能优化技巧**

---

## 文件列表

```
cpp-core/
├── README.md              # 本文件
├── 01-basic-syntax.md
├── 02-type-system.md
├── 03-function.md
├── 04-oop-basic.md
├── 05-oop-advanced.md
├── 06-memory.md
├── 07-stl-container.md
├── 08-stl-algorithm.md
├── 09-template-basic.md
├── 10-template-advanced.md
├── 11-thread-basic.md
├── 12-thread-advanced.md
├── 13-compile-link.md
├── 14-exception.md
├── 15-modern-cpp.md
├── 16-performance.md
├── 17-debug-tools.md
├── 18-design-pattern.md
├── 19-abi.md
├── 20-undefined-behavior.md
├── 21-bit-operations.md
├── 22-string-encoding.md
├── 23-file-io.md
├── 24-time-date.md
├── 25-reflection-metaprogramming.md
└── 26-cross-language.md
```
