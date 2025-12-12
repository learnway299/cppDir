# 07 - STL 容器 (STL Containers)

## 简介

STL 容器是 C++ 标准库提供的数据结构集合，分为三大类：
- **顺序容器 (Sequence Containers)**：元素按线性顺序存储
- **关联容器 (Associative Containers)**：元素按键值自动排序
- **容器适配器 (Container Adapters)**：对底层容器的封装

---

## 1. 顺序容器 (Sequence Containers)

### 1.1 `vector` vs `deque` vs `list` 的区别？

**问题描述**：三种顺序容器的特点和使用场景。

**核心差异**：

```cpp
#include <vector>
#include <deque>
#include <list>
#include <iostream>

void compare_containers() {
    // vector: 动态数组，连续内存
    std::vector<int> vec = {1, 2, 3, 4, 5};
    vec.push_back(6);           // O(1) 平均
    // vec.push_front(0);       // ❌ 不支持

    // deque: 双端队列，分段连续内存
    std::deque<int> deq = {1, 2, 3, 4, 5};
    deq.push_back(6);           // O(1)
    deq.push_front(0);          // O(1)

    // list: 双向链表，非连续内存
    std::list<int> lst = {1, 2, 3, 4, 5};
    lst.push_back(6);           // O(1)
    lst.push_front(0);          // O(1)

    // 随机访问
    std::cout << vec[2] << "\n";     // O(1) ✅
    std::cout << deq[2] << "\n";     // O(1) ✅
    // std::cout << lst[2] << "\n";  // ❌ 不支持随机访问

    // 插入/删除
    vec.insert(vec.begin() + 2, 99); // O(n)
    deq.insert(deq.begin() + 2, 99); // O(n)

    auto it = lst.begin();
    std::advance(it, 2);
    lst.insert(it, 99);              // O(1) 在已知迭代器位置
}
```

**关键特性对比**：

| 特性 | `vector` | `deque` | `list` |
|------|----------|---------|--------|
| 内存布局 | 连续 | 分段连续 | 链表 |
| 随机访问 | O(1) | O(1) | ❌ |
| 头部插入/删除 | O(n) | O(1) | O(1) |
| 尾部插入/删除 | O(1) 均摊 | O(1) | O(1) |
| 中间插入/删除 | O(n) | O(n) | O(1)* |
| 迭代器失效 | 插入/删除后可能失效 | 插入/删除后可能失效 | 仅删除元素失效 |
| 内存占用 | 低 | 中 | 高（指针开销） |

*注：list 中间插入 O(1) 的前提是已经定位到插入位置。

**关键要点**：
- `vector`：默认选择，cache-friendly，需要频繁随机访问时使用
- `deque`：需要双端操作时使用（如实现队列）
- `list`：需要频繁插入/删除且不需要随机访问时使用

**常见陷阱**：
```cpp
// ❌ 错误：vector 频繁头部插入
std::vector<int> v;
for (int i = 0; i < 10000; ++i) {
    v.insert(v.begin(), i);  // 每次 O(n)，总共 O(n²)
}

// ✅ 正确：使用 deque 或先 push_back 再 reverse
std::deque<int> d;
for (int i = 0; i < 10000; ++i) {
    d.push_front(i);  // O(1)
}
```

---

### 1.2 `vector` 的扩容机制？

**问题描述**：`vector` 如何动态增长？扩容因子是多少？

**扩容机制**：

```cpp
#include <vector>
#include <iostream>

void vector_capacity_demo() {
    std::vector<int> v;

    std::cout << "初始 capacity: " << v.capacity() << "\n";  // 0

    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
        std::cout << "size: " << v.size()
                  << ", capacity: " << v.capacity() << "\n";
    }
    // 输出示例（GCC）：0 -> 1 -> 2 -> 4 -> 8 -> 16
    // MSVC 可能是：0 -> 1 -> 2 -> 3 -> 4 -> 6 -> 9 -> 13...
}

// 预留空间避免频繁扩容
void reserve_demo() {
    std::vector<int> v;
    v.reserve(1000);  // 预分配空间，避免扩容

    for (int i = 0; i < 1000; ++i) {
        v.push_back(i);  // 不会触发扩容
    }
}

// resize vs reserve
void resize_vs_reserve() {
    std::vector<int> v1;
    v1.reserve(100);  // 仅分配空间，size 仍为 0
    // v1[0] = 1;     // ❌ 未定义行为！

    std::vector<int> v2;
    v2.resize(100);   // 分配空间并初始化，size = 100
    v2[0] = 1;        // ✅ 正确
}
```

**关键要点**：
- **扩容因子**：GCC 通常是 2 倍，MSVC 是 1.5 倍
- **扩容代价**：O(n)，需要重新分配内存并拷贝所有元素
- **`reserve(n)`**：预留空间，不改变 size
- **`resize(n)`**：改变 size，会构造/析构元素
- **`shrink_to_fit()`**：释放多余空间（C++11）

**常见陷阱**：
```cpp
// ❌ 错误：扩容导致迭代器失效
std::vector<int> v = {1, 2, 3};
auto it = v.begin();
v.push_back(4);  // 可能触发扩容
// *it;          // ❌ 迭代器可能已失效

// ✅ 正确：使用下标或预留空间
std::vector<int> v2 = {1, 2, 3};
v2.reserve(10);  // 预留空间
auto it2 = v2.begin();
v2.push_back(4);
std::cout << *it2 << "\n";  // ✅ 安全
```

---

### 1.3 `array` 和 `forward_list` 的使用场景？

**问题描述**：C++11 引入的固定大小数组和单向链表。

**`array`**：固定大小数组

```cpp
#include <array>
#include <algorithm>

void array_demo() {
    // 编译期确定大小，栈上分配
    std::array<int, 5> arr = {1, 2, 3, 4, 5};

    // 支持 STL 算法
    std::sort(arr.begin(), arr.end());

    // 边界检查
    arr.at(0) = 10;     // 有边界检查，抛出异常
    arr[0] = 10;        // 无边界检查，更快

    // 获取大小
    std::cout << arr.size() << "\n";  // 5

    // C 数组兼容
    int* p = arr.data();
}

// array vs C 数组
void array_vs_c_array() {
    int c_arr[5] = {1, 2, 3, 4, 5};
    std::array<int, 5> cpp_arr = {1, 2, 3, 4, 5};

    // ❌ C 数组：不知道大小，会退化为指针
    // sizeof(c_arr) / sizeof(c_arr[0])  // 仅在声明作用域有效

    // ✅ array：始终知道大小
    std::cout << cpp_arr.size() << "\n";  // 5
}
```

**`forward_list`**：单向链表

```cpp
#include <forward_list>

void forward_list_demo() {
    std::forward_list<int> flist = {1, 2, 3, 4, 5};

    // 只能前向遍历
    for (auto it = flist.begin(); it != flist.end(); ++it) {
        std::cout << *it << " ";
    }
    // ❌ --it;  // 不支持

    // 插入在迭代器之后
    auto it = flist.before_begin();
    flist.insert_after(it, 0);  // 头部插入

    // 删除
    flist.remove(3);  // 删除值为 3 的元素
}
```

**关键要点**：
- **`array`**：
  - 栈上分配，零开销抽象
  - 适合小型固定大小数据
  - 比 C 数组更安全（不会退化为指针）
- **`forward_list`**：
  - 内存占用比 `list` 小（单向指针）
  - 适合对内存极度敏感的场景
  - 很少使用，大多数情况用 `list` 更方便

---

## 2. 关联容器 (Associative Containers)

### 2.1 `set` / `map` 的底层实现？

**问题描述**：关联容器如何保证有序和快速查找？

**底层实现**：红黑树 (Red-Black Tree)

```cpp
#include <set>
#include <map>
#include <iostream>

void set_map_demo() {
    // set: 有序集合，自动去重
    std::set<int> s = {3, 1, 4, 1, 5, 9};
    // 内部自动排序：{1, 3, 4, 5, 9}

    for (int x : s) {
        std::cout << x << " ";  // 1 3 4 5 9
    }

    // 查找
    if (s.find(4) != s.end()) {
        std::cout << "找到 4\n";
    }

    // map: 键值对，键有序
    std::map<std::string, int> m = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 22}
    };

    // 自动按键排序：Alice -> Bob -> Charlie
    for (const auto& [name, age] : m) {  // C++17 结构化绑定
        std::cout << name << ": " << age << "\n";
    }

    // 插入
    m["David"] = 28;
    m.insert({"Eve", 26});

    // 查找
    auto it = m.find("Bob");
    if (it != m.end()) {
        std::cout << it->second << "\n";  // 30
    }
}
```

**时间复杂度**：

| 操作 | 时间复杂度 |
|------|-----------|
| 插入 | O(log n) |
| 删除 | O(log n) |
| 查找 | O(log n) |
| 遍历 | O(n) 有序 |

**关键要点**：
- **红黑树性质**：
  1. 节点是红色或黑色
  2. 根节点是黑色
  3. 叶节点（NIL）是黑色
  4. 红色节点的子节点必须是黑色
  5. 从任一节点到其叶节点的所有路径包含相同数量的黑色节点
- **保证平衡**：最长路径不超过最短路径的 2 倍
- **有序性**：中序遍历得到有序序列

**常见陷阱**：
```cpp
// ❌ 错误：map[] 会插入默认值
std::map<std::string, int> m;
if (m["key"] == 0) {  // ❌ 如果 key 不存在，会插入 {key, 0}
    // ...
}

// ✅ 正确：使用 find
if (m.find("key") != m.end() && m["key"] == 0) {
    // ...
}

// ✅ 或使用 count
if (m.count("key") && m["key"] == 0) {
    // ...
}
```

---

### 2.2 `multiset` / `multimap` 与 `set` / `map` 的区别？

**问题描述**：允许重复键的关联容器。

**使用示例**：

```cpp
#include <set>
#include <map>

void multi_containers() {
    // set: 不允许重复
    std::set<int> s = {1, 2, 2, 3};
    // 结果：{1, 2, 3}

    // multiset: 允许重复
    std::multiset<int> ms = {1, 2, 2, 3};
    // 结果：{1, 2, 2, 3}

    std::cout << "2 出现次数: " << ms.count(2) << "\n";  // 2

    // multimap: 允许重复键
    std::multimap<std::string, int> mm = {
        {"Alice", 25},
        {"Bob", 30},
        {"Alice", 28}  // 重复键
    };

    // 查找所有 Alice
    auto range = mm.equal_range("Alice");
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << it->first << ": " << it->second << "\n";
    }
    // 输出：
    // Alice: 25
    // Alice: 28
}
```

**关键要点**：
- `multiset` / `multimap` 允许存储重复元素
- `equal_range()` 返回所有匹配元素的区间
- 无法使用 `[]` 操作符（`multimap`）

---

### 2.3 `unordered_set` / `unordered_map` 的底层实现？

**问题描述**：无序关联容器如何实现？何时使用？

**底层实现**：哈希表 (Hash Table)

```cpp
#include <unordered_set>
#include <unordered_map>

void unordered_demo() {
    // unordered_set: 无序，但查找更快
    std::unordered_set<int> us = {3, 1, 4, 1, 5, 9};
    // 内部无序，可能是：{9, 1, 5, 3, 4}

    // O(1) 平均查找
    if (us.find(4) != us.end()) {
        std::cout << "找到 4\n";
    }

    // unordered_map
    std::unordered_map<std::string, int> um = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 22}
    };

    // 遍历顺序不确定
    for (const auto& [name, age] : um) {
        std::cout << name << ": " << age << "\n";
    }
}

// 自定义哈希函数
struct Person {
    std::string name;
    int age;

    bool operator==(const Person& other) const {
        return name == other.name && age == other.age;
    }
};

// 自定义哈希
struct PersonHash {
    std::size_t operator()(const Person& p) const {
        return std::hash<std::string>()(p.name) ^
               (std::hash<int>()(p.age) << 1);
    }
};

void custom_hash() {
    std::unordered_set<Person, PersonHash> persons;
    persons.insert({"Alice", 25});
}
```

**时间复杂度**：

| 操作 | 平均 | 最坏 |
|------|------|------|
| 插入 | O(1) | O(n) |
| 删除 | O(1) | O(n) |
| 查找 | O(1) | O(n) |

**关键要点**：
- **哈希冲突处理**：链地址法（拉链法）
- **负载因子**：元素数量 / 桶数量，默认 1.0
- **自动扩容**：负载因子超过阈值时重新哈希
- **要求**：键类型需要支持 `std::hash` 和 `operator==`

**何时使用**：
```cpp
// ✅ 使用 unordered_map/set：
// - 只需要查找，不需要有序
// - 追求极致性能

// ✅ 使用 map/set：
// - 需要有序遍历
// - 需要范围查询（lower_bound, upper_bound）
// - 键类型难以定义哈希函数
```

**常见陷阱**：
```cpp
// ❌ 错误：unordered_map 无法使用自定义类型（未定义哈希）
struct Point { int x, y; };
std::unordered_map<Point, int> m;  // ❌ 编译错误

// ✅ 正确：提供哈希函数
struct PointHash {
    std::size_t operator()(const Point& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};
struct PointEqual {
    bool operator()(const Point& a, const Point& b) const {
        return a.x == b.x && a.y == b.y;
    }
};
std::unordered_map<Point, int, PointHash, PointEqual> m2;  // ✅
```

---

## 3. 容器适配器 (Container Adapters)

### 3.1 `stack` / `queue` / `priority_queue` 的底层容器？

**问题描述**：适配器如何实现？可以使用哪些底层容器？

**实现原理**：

```cpp
#include <stack>
#include <queue>
#include <deque>
#include <vector>

void adapter_demo() {
    // stack: 默认使用 deque
    std::stack<int> s1;
    std::stack<int, std::vector<int>> s2;  // 使用 vector
    std::stack<int, std::deque<int>> s3;   // 显式指定 deque

    s1.push(1);
    s1.push(2);
    s1.push(3);
    std::cout << s1.top() << "\n";  // 3
    s1.pop();

    // queue: 默认使用 deque
    std::queue<int> q1;
    std::queue<int, std::list<int>> q2;    // 使用 list

    q1.push(1);
    q1.push(2);
    std::cout << q1.front() << "\n";  // 1
    q1.pop();

    // priority_queue: 默认使用 vector + max heap
    std::priority_queue<int> pq1;
    pq1.push(3);
    pq1.push(1);
    pq1.push(4);
    std::cout << pq1.top() << "\n";  // 4（最大值）

    // 自定义比较器（min heap）
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
    min_pq.push(3);
    min_pq.push(1);
    min_pq.push(4);
    std::cout << min_pq.top() << "\n";  // 1（最小值）
}
```

**底层容器要求**：

| 适配器 | 默认底层容器 | 可选底层容器 | 要求的操作 |
|--------|-------------|-------------|----------|
| `stack` | `deque` | `vector`, `list`, `deque` | `back()`, `push_back()`, `pop_back()` |
| `queue` | `deque` | `list`, `deque` | `front()`, `back()`, `push_back()`, `pop_front()` |
| `priority_queue` | `vector` | `vector`, `deque` | 随机访问，`push_back()`, `pop_back()` |

**关键要点**：
- **适配器模式**：通过封装提供受限接口
- **`priority_queue`**：内部使用堆算法维护
- **不能使用迭代器**：适配器不提供迭代器访问

---

### 3.2 如何实现自定义优先队列比较器？

**问题描述**：如何控制 `priority_queue` 的排序规则？

**实现方式**：

```cpp
#include <queue>
#include <vector>

struct Task {
    int priority;
    std::string name;
};

// 方式 1：使用函数对象
struct TaskCompare {
    bool operator()(const Task& a, const Task& b) const {
        // 注意：返回 true 表示 a 优先级低于 b（小顶堆逻辑）
        return a.priority < b.priority;  // 高优先级在前
    }
};

void custom_priority_queue() {
    std::priority_queue<Task, std::vector<Task>, TaskCompare> pq;

    pq.push({1, "Low"});
    pq.push({3, "High"});
    pq.push({2, "Medium"});

    while (!pq.empty()) {
        std::cout << pq.top().name << "\n";  // High -> Medium -> Low
        pq.pop();
    }
}

// 方式 2：使用 lambda（C++11）
void lambda_priority_queue() {
    auto cmp = [](const Task& a, const Task& b) {
        return a.priority < b.priority;
    };

    std::priority_queue<Task, std::vector<Task>, decltype(cmp)> pq(cmp);

    pq.push({1, "Low"});
    pq.push({3, "High"});
    pq.push({2, "Medium"});
}

// 方式 3：使用 std::greater（内置类型）
void builtin_priority_queue() {
    // 默认是 max heap（大顶堆）
    std::priority_queue<int> max_pq;
    max_pq.push(3);
    max_pq.push(1);
    max_pq.push(4);
    std::cout << max_pq.top() << "\n";  // 4

    // 使用 std::greater 实现 min heap（小顶堆）
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
    min_pq.push(3);
    min_pq.push(1);
    min_pq.push(4);
    std::cout << min_pq.top() << "\n";  // 1
}
```

**常见陷阱**：
```cpp
// ❌ 错误：比较器逻辑反了
struct WrongCompare {
    bool operator()(int a, int b) const {
        return a > b;  // 本意想要大顶堆，实际得到小顶堆
    }
};

// ✅ 理解：priority_queue 的比较器定义"谁优先级低"
// - 返回 true：a 优先级低于 b，b 应该在 a 前面
// - 类似 sort 的比较器，但含义相反
```

---

## 4. 容器选择指南

### 4.1 如何选择合适的容器？

**决策树**：

```
需要频繁随机访问？
├─ 是 → 固定大小？
│   ├─ 是 → array
│   └─ 否 → vector
└─ 否 → 需要快速查找？
    ├─ 是 → 需要有序？
    │   ├─ 是 → set/map
    │   └─ 否 → unordered_set/unordered_map
    └─ 否 → 需要双端操作？
        ├─ 是 → deque
        └─ 否 → 需要频繁中间插入/删除？
            ├─ 是 → list
            └─ 否 → 特定接口？
                ├─ LIFO → stack
                ├─ FIFO → queue
                └─ 优先级 → priority_queue
```

**典型场景**：

```cpp
// 场景 1：需要动态数组 → vector
std::vector<int> scores;
for (int i = 0; i < n; ++i) {
    scores.push_back(read_score());
}

// 场景 2：需要快速查找是否存在 → unordered_set
std::unordered_set<int> visited;
if (visited.find(x) != visited.end()) {
    // ...
}

// 场景 3：需要按顺序处理 + 快速查找 → map
std::map<int, std::string> id_to_name;
for (const auto& [id, name] : id_to_name) {  // 按 id 有序
    // ...
}

// 场景 4：需要维护最大/最小值 → priority_queue
std::priority_queue<int> max_heap;
max_heap.push(x);
int max_val = max_heap.top();

// 场景 5：需要 LRU 缓存 → list + unordered_map
// （见后续 Coding 部分）
```

---

## 5. 迭代器失效问题

### 5.1 哪些操作会导致迭代器失效？

**问题描述**：容器修改后迭代器可能失效，如何避免？

**失效规则**：

```cpp
#include <vector>
#include <list>
#include <set>

void iterator_invalidation() {
    // vector: 插入/删除可能导致所有迭代器失效
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto it = v.begin();
    v.push_back(6);  // 可能扩容 → 所有迭代器失效
    // *it;          // ❌ 未定义行为

    // 删除元素
    auto it2 = v.begin() + 2;
    v.erase(it2);  // it2 及之后的迭代器失效
    // *it2;       // ❌ 未定义行为

    // list: 删除仅使被删除元素的迭代器失效
    std::list<int> lst = {1, 2, 3, 4, 5};
    auto it3 = lst.begin();
    ++it3;  // 指向 2
    auto it4 = it3;
    ++it4;  // 指向 3
    lst.erase(it3);  // 仅 it3 失效
    std::cout << *it4 << "\n";  // ✅ 3，it4 仍有效

    // set/map: 删除仅使被删除元素的迭代器失效
    std::set<int> s = {1, 2, 3, 4, 5};
    auto it5 = s.find(3);
    auto it6 = it5;
    ++it6;  // 指向 4
    s.erase(it5);  // 仅 it5 失效
    std::cout << *it6 << "\n";  // ✅ 4，it6 仍有效
}

// 安全删除
void safe_erase() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    // ❌ 错误：迭代器失效
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (*it % 2 == 0) {
            v.erase(it);  // it 失效，++it 未定义行为
        }
    }

    // ✅ 正确：使用 erase 返回值
    for (auto it = v.begin(); it != v.end(); ) {
        if (*it % 2 == 0) {
            it = v.erase(it);  // erase 返回下一个有效迭代器
        } else {
            ++it;
        }
    }

    // ✅ 或使用 remove-erase idiom
    v.erase(std::remove_if(v.begin(), v.end(),
                           [](int x) { return x % 2 == 0; }),
            v.end());
}
```

**迭代器失效总结**：

| 容器 | 插入 | 删除 |
|------|------|------|
| `vector` | 所有迭代器可能失效（扩容时） | 删除位置及之后失效 |
| `deque` | 所有迭代器可能失效 | 仅删除位置失效（首尾除外） |
| `list` | 不失效 | 仅删除位置失效 |
| `set`/`map` | 不失效 | 仅删除位置失效 |
| `unordered_set`/`unordered_map` | 所有迭代器可能失效（rehash 时） | 仅删除位置失效 |

---

## 6. 性能优化技巧

### 6.1 容器性能优化建议

**1. 预留空间**：

```cpp
// ❌ 频繁扩容
std::vector<int> v;
for (int i = 0; i < 1000000; ++i) {
    v.push_back(i);  // 多次扩容
}

// ✅ 预留空间
std::vector<int> v2;
v2.reserve(1000000);
for (int i = 0; i < 1000000; ++i) {
    v2.push_back(i);  // 无扩容
}
```

**2. 使用 `emplace` 系列函数**：

```cpp
#include <vector>
#include <string>

struct Person {
    std::string name;
    int age;
    Person(std::string n, int a) : name(std::move(n)), age(a) {}
};

void emplace_demo() {
    std::vector<Person> people;

    // ❌ push_back：构造临时对象 + 拷贝/移动
    people.push_back(Person("Alice", 25));

    // ✅ emplace_back：直接在容器中构造
    people.emplace_back("Bob", 30);  // 无临时对象
}
```

**3. 避免不必要的拷贝**：

```cpp
// ❌ 拷贝整个 map
std::map<int, std::string> get_map() {
    std::map<int, std::string> m;
    // ...
    return m;  // 可能触发拷贝（取决于编译器优化）
}

// ✅ 返回引用或使用移动语义
std::map<int, std::string> get_map_rvo() {
    std::map<int, std::string> m;
    // ...
    return m;  // RVO/NRVO 优化
}

void use_map() {
    auto m = get_map_rvo();  // 移动而非拷贝
}
```

**4. 选择合适的容器**：

```cpp
// ❌ 使用 vector 频繁插入/删除
std::vector<int> v;
for (int i = 0; i < 10000; ++i) {
    v.insert(v.begin() + rand() % v.size(), i);  // O(n)
}

// ✅ 使用 list
std::list<int> lst;
auto it = lst.begin();
for (int i = 0; i < 10000; ++i) {
    lst.insert(it, i);  // O(1)
}
```

---

## 7. 常见面试题

### Q1：如何在 O(1) 时间内删除 `vector` 中的元素？

```cpp
// 解法：将待删除元素与最后一个元素交换，然后 pop_back
void fast_erase(std::vector<int>& v, size_t index) {
    if (index < v.size()) {
        std::swap(v[index], v.back());
        v.pop_back();
    }
}
// 注意：会改变元素顺序
```

### Q2：`vector<bool>` 有什么特殊之处？

```cpp
// vector<bool> 是特化版本，使用位压缩存储
std::vector<bool> vb = {true, false, true};

// ❌ 无法获取元素引用
// bool& ref = vb[0];  // 编译错误

// ✅ 使用代理对象
auto ref = vb[0];  // 返回代理对象，不是 bool&
ref = false;       // 通过代理修改

// 替代方案：使用 vector<char> 或 std::bitset
std::vector<char> vc = {1, 0, 1};  // 可以获取引用
```

### Q3：如何实现一个固定大小的环形缓冲区？

```cpp
#include <deque>

template<typename T>
class RingBuffer {
    std::deque<T> buffer;
    size_t max_size;

public:
    RingBuffer(size_t size) : max_size(size) {}

    void push(const T& item) {
        if (buffer.size() == max_size) {
            buffer.pop_front();
        }
        buffer.push_back(item);
    }

    const T& front() const { return buffer.front(); }
    const T& back() const { return buffer.back(); }
    size_t size() const { return buffer.size(); }
};
```

---

## 总结

### 容器选择快速参考

| 需求 | 推荐容器 |
|------|---------|
| 动态数组 | `vector` |
| 双端队列 | `deque` |
| 频繁插入/删除（中间） | `list` |
| 固定大小数组 | `array` |
| 快速查找（有序） | `set`/`map` |
| 快速查找（无序） | `unordered_set`/`unordered_map` |
| 允许重复键 | `multiset`/`multimap` |
| LIFO | `stack` |
| FIFO | `queue` |
| 优先级队列 | `priority_queue` |

### 关键记忆点

1. **`vector`**：动态数组，连续内存，默认首选
2. **`deque`**：双端队列，支持高效首尾操作
3. **`list`**：双向链表，高效插入/删除
4. **`set`/`map`**：红黑树，O(log n) 有序查找
5. **`unordered_set`/`unordered_map`**：哈希表，O(1) 平均查找
6. **适配器**：受限接口，底层可定制
7. **迭代器失效**：插入/删除后小心使用迭代器
8. **性能优化**：`reserve()`、`emplace()`、选对容器
