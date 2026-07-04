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

**问题描述**：`vector` 默认预留多少空间？如何动态增长？扩容因子是多少？

**默认预留空间：通常为 0**

```cpp
#include <vector>
#include <iostream>

void vector_default_capacity() {
    std::vector<int> v;
    std::cout << "size: " << v.size() << "\n";        // 0
    std::cout << "capacity: " << v.capacity() << "\n"; // 0（通常）

    v.push_back(1);
    std::cout << "After 1 push, capacity: " << v.capacity() << "\n";  // 1 或更大
}
```

**各实现的扩容策略：**

| 实现 | 初始 capacity | 扩容倍数 |
|------|--------------|----------|
| GCC (libstdc++) | 0 | **2 倍** |
| Clang (libc++) | 0 | **2 倍** |
| MSVC | 0 | **1.5 倍** |

```
GCC 扩容示例：
push 1: capacity = 1
push 2: capacity = 2
push 3: capacity = 4
push 5: capacity = 8
push 9: capacity = 16
...
```

**扩容机制详解**：

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

**哈希冲突的常见解决方法**：

| 方法 | 原理 | 优点 | 缺点 |
|------|------|------|------|
| **链地址法（拉链法）** | 每个桶维护一个链表，冲突元素追加到链表 | 实现简单；删除方便；负载因子可 >1 | 链表指针额外开销；cache 不友好 |
| **开放地址法 — 线性探测** | 冲突时依次往后找空槽 `(h+1, h+2, ...)` | cache 友好；内存紧凑 | 容易产生"聚集"(clustering)；删除需标记 |
| **开放地址法 — 二次探测** | 冲突时按平方步长找空槽 `(h+1², h+2², ...)` | 减缓聚集问题 | 可能无法探测所有槽位；删除复杂 |
| **开放地址法 — 双重哈希** | 用第二个哈希函数计算步长 `(h + i·h₂(key))` | 冲突分布更均匀 | 需要设计两个互不相关的哈希函数 |
| **Robin Hood Hashing** | 开放地址变体，"富者让位"——新元素可以抢占探测距离更短的元素 | 减少最坏探测长度；查找方差小 | 实现复杂 |
| **Cuckoo Hashing** | 两个哈希表+两个哈希函数，冲突时踢走旧元素 | 最坏查找 O(1) | 插入可能触发连锁踢出；负载因子需 <50% |

**C++ STL 使用的方法：链地址法（Separate Chaining）**

```cpp
// STL unordered_map 内部结构示意（GCC libstdc++ 实现）
//
// buckets[] (桶数组，存指针)
// ┌───┐
// │ 0 │ → nullptr
// ├───┤
// │ 1 │ → [Node A] → [Node D] → nullptr
// ├───┤
// │ 2 │ → [Node B] → nullptr
// ├───┤
// │ 3 │ → nullptr
// ├───┤
// │ 4 │ → [Node C] → [Node E] → [Node F] → nullptr
// └───┘
//
// 每个 Node 包含: { hash_value, key, value, next_ptr }
//
// GCC 实际实现细节：
// - 使用单链表串联所有元素（便于迭代器遍历）
// - 每个桶存储指向链表中对应段的指针
// - 这样 begin()/end() 遍历不需要跳过空桶

#include <unordered_map>
#include <iostream>

void hash_collision_demo() {
    std::unordered_map<int, std::string> m;
    m[1] = "one";
    m[2] = "two";
    m[3] = "three";

    // 查看桶分布
    std::cout << "bucket_count: " << m.bucket_count() << "\n";
    for (size_t i = 0; i < m.bucket_count(); ++i) {
        std::cout << "bucket " << i << " has "
                  << m.bucket_size(i) << " elements";
        if (m.bucket_size(i) > 0) {
            std::cout << ": ";
            for (auto it = m.begin(i); it != m.end(i); ++it) {
                std::cout << "{" << it->first << "," << it->second << "} ";
            }
        }
        std::cout << "\n";
    }

    // 负载因子相关
    std::cout << "load_factor: " << m.load_factor() << "\n";
    std::cout << "max_load_factor: " << m.max_load_factor() << "\n";
    // 当 load_factor > max_load_factor 时触发 rehash
}
```

**为什么 STL 选择链地址法而非开放地址法？**

```
1. 通用性：链地址法对哈希函数质量要求较低，适合标准库的通用场景
2. 删除友好：直接从链表中删除节点，无需"墓碑标记"
3. 负载因子灵活：负载因子可以 >1（每个桶多个元素），开放地址法必须 <1
4. 迭代器稳定性：rehash 时节点可以直接移到新桶，指向节点的指针不失效
5. 异常安全：节点独立分配，插入失败不影响已有元素

代价：
- 每个节点额外指针开销（8 字节/节点 on 64-bit）
- 链表遍历 cache 不友好（节点分散在堆上）
- 这也是为什么在性能极致场景下，工程中常用开放地址法的替代实现
  （如 Google 的 absl::flat_hash_map 使用 Swiss Table / 开放地址法）
```

**面试扩展：开放地址法的实现示例**

```cpp
// 简单的线性探测哈希表实现
template<typename K, typename V>
class LinearProbingMap {
    enum class State { EMPTY, OCCUPIED, DELETED };
    struct Slot {
        K key;
        V value;
        State state = State::EMPTY;
    };
    std::vector<Slot> table;
    size_t count = 0;

    size_t probe(const K& key) const {
        size_t idx = std::hash<K>{}(key) % table.size();
        while (table[idx].state != State::EMPTY) {
            if (table[idx].state == State::OCCUPIED && table[idx].key == key)
                return idx;  // 找到
            idx = (idx + 1) % table.size();  // 线性探测
        }
        return idx;
    }

public:
    LinearProbingMap(size_t cap = 16) : table(cap) {}

    void insert(const K& key, const V& value) {
        if ((double)count / table.size() > 0.7) rehash();  // 负载因子阈值
        size_t idx = probe(key);
        table[idx] = {key, value, State::OCCUPIED};
        ++count;
    }

    void erase(const K& key) {
        size_t idx = probe(key);
        if (table[idx].state == State::OCCUPIED) {
            table[idx].state = State::DELETED;  // 标记删除，不能直接置空
            --count;
        }
    }

    // 为什么删除要标记而不能直接置空？
    // 因为线性探测依赖连续占用的槽位来查找元素
    // 如果中间出现空槽，后面的元素就"断链"找不到了

    void rehash() { /* 扩容并重新插入所有 OCCUPIED 元素 */ }
};
```

**关键要点**：
- **STL 使用链地址法**：通用性好、删除友好、异常安全
- **负载因子**：元素数量 / 桶数量，默认 max_load_factor = 1.0
- **自动扩容**：负载因子超过阈值时 rehash，所有元素重新分配到新桶
- **要求**：键类型需要支持 `std::hash` 和 `operator==`
- **高性能替代**：`absl::flat_hash_map`（开放地址法）在很多场景下比 `std::unordered_map` 快 2-3 倍

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

### 2.4 自定义类型放入容器的要求

**问题描述**：自定义类要放入 `set`、`map`、`unordered_set`、`unordered_map` 需要实现什么方法？

**要求总结：**

| 容器 | 需要实现 | 默认使用 |
|------|----------|----------|
| `set` / `map` | `operator<` 或自定义比较器 | `std::less<T>` |
| `unordered_set` / `unordered_map` | `std::hash` 特化 + `operator==` | `std::hash<T>` + `std::equal_to<T>` |

**1. set / map（有序容器）：需要 `operator<`**

```cpp
#include <set>
#include <map>

class Person {
public:
    std::string name;
    int age;

    Person(std::string n, int a) : name(std::move(n)), age(a) {}

    // 方式1：重载 operator<
    bool operator<(const Person& other) const {
        if (age != other.age) return age < other.age;
        return name < other.name;
    }
};

// 方式2：自定义比较器（函数对象）
struct PersonCompare {
    bool operator()(const Person& a, const Person& b) const {
        return a.age < b.age;
    }
};

void set_map_custom_type() {
    // 使用 operator<
    std::set<Person> s1;
    s1.insert(Person("Alice", 25));
    s1.insert(Person("Bob", 30));

    std::map<Person, int> m1;
    m1[Person("Alice", 25)] = 1;

    // 使用自定义比较器
    std::set<Person, PersonCompare> s2;
    std::map<Person, int, PersonCompare> m2;
}
```

**2. unordered_set / unordered_map（哈希容器）：需要 `std::hash` + `operator==`**

```cpp
#include <unordered_set>
#include <unordered_map>
#include <functional>

class Person {
public:
    std::string name;
    int age;

    Person(std::string n, int a) : name(std::move(n)), age(a) {}

    // 必须：operator==
    bool operator==(const Person& other) const {
        return name == other.name && age == other.age;
    }
};

// 方式1：特化 std::hash
namespace std {
    template<>
    struct hash<Person> {
        size_t operator()(const Person& p) const {
            size_t h1 = hash<string>{}(p.name);
            size_t h2 = hash<int>{}(p.age);
            return h1 ^ (h2 << 1);  // 组合哈希值
        }
    };
}

// 方式2：自定义哈希函数对象
struct PersonHash {
    size_t operator()(const Person& p) const {
        return std::hash<std::string>{}(p.name) ^
               (std::hash<int>{}(p.age) << 1);
    }
};

void unordered_custom_type() {
    // 使用 std::hash 特化
    std::unordered_set<Person> s1;
    s1.insert(Person("Alice", 25));

    std::unordered_map<Person, int> m1;
    m1[Person("Alice", 25)] = 1;

    // 使用自定义哈希函数对象
    std::unordered_set<Person, PersonHash> s2;
    std::unordered_map<Person, int, PersonHash> m2;
}
```

**严格弱序要求（`operator<` 必须满足）：**

```cpp
// operator< 必须满足以下条件，否则导致未定义行为：
// 1. 非自反性：!(a < a)
// 2. 非对称性：若 a < b，则 !(b < a)
// 3. 传递性：若 a < b 且 b < c，则 a < c
// 4. 等价传递性：若 !(a<b) && !(b<a) 且 !(b<c) && !(c<b)，则 !(a<c) && !(c<a)

// ❌ 错误示例：不满足严格弱序
struct BadCompare {
    bool operator()(int a, int b) const {
        return a <= b;  // ❌ 违反非自反性：a <= a 为 true
    }
};

// ✅ 正确示例
struct GoodCompare {
    bool operator()(int a, int b) const {
        return a < b;  // ✅ 满足严格弱序
    }
};
```

**关键要点：**
- `set`/`map`：只需 `operator<`（或比较器），用于排序和查找
- `unordered_set`/`unordered_map`：需要 `operator==`（判断相等）+ `std::hash`（计算哈希值）
- 哈希函数质量影响性能，好的哈希函数应尽量减少冲突

**常见陷阱：**
- 忘记实现 `operator==`（unordered 容器必需）
- `operator<` 不满足严格弱序（导致未定义行为）
- 哈希函数质量差导致性能下降（大量冲突）

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

## 6. 各容器默认容量与扩容机制

### 6.1 总览表

| 容器 | 默认初始容量 | 扩容机制 | 扩容代价 |
|------|-------------|---------|---------|
| `vector` | 0 | GCC/Clang 2倍，MSVC 1.5倍 | O(n)，重新分配+拷贝全部元素 |
| `deque` | 1个块（512字节/元素大小 个元素） | 新增固定大小的块 | O(1)，不移动已有元素 |
| `list` | 0（无预分配） | 每次 insert/push 单独分配一个节点 | O(1)，无扩容概念 |
| `forward_list` | 0（无预分配） | 每次 insert 单独分配一个节点 | O(1)，无扩容概念 |
| `string` | SSO 缓冲区（GCC/MSVC 15字符，libc++ 22字符） | 类似 vector，GCC 2倍，MSVC 1.5倍 | O(n)，重新分配+拷贝 |
| `set`/`map` | 0（无预分配） | 每次 insert 单独分配一个红黑树节点 | O(1)，无扩容概念 |
| `unordered_set`/`unordered_map` | 实现相关（GCC 默认约 1 个桶） | 当 load_factor > max_load_factor(默认1.0) 时 rehash，桶数约翻倍 | O(n)，所有元素重新哈希 |

### 6.2 各容器扩容机制详解

#### vector 扩容

```cpp
#include <vector>
#include <iostream>

void vector_growth() {
    std::vector<int> v;
    size_t prev_cap = 0;
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
        if (v.capacity() != prev_cap) {
            std::cout << "size=" << v.size()
                      << " capacity=" << v.capacity() << "\n";
            prev_cap = v.capacity();
        }
    }
}
// GCC 输出:  1→1, 2→2, 3→4, 5→8, 9→16, 17→32, 33→64, 65→128
// MSVC 输出: 1→1, 2→2, 3→3, 4→4, 5→6, 7→9, 10→13, 14→19, ...
```

- **GCC/Clang**：每次 capacity 不足时翻倍（×2）
- **MSVC**：每次乘以约 1.5 倍（更节省内存，但扩容更频繁）
- **扩容过程**：allocate 新内存 → move/copy 全部元素 → deallocate 旧内存
- **均摊复杂度**：`push_back` 均摊 O(1)

#### deque 扩容

```cpp
#include <deque>
#include <iostream>

void deque_growth() {
    std::deque<int> d;
    // deque 内部由多个固定大小的块(block/chunk)组成
    // GCC libstdc++: 块大小 = max(512 / sizeof(T), 1) 个元素
    //   int (4字节): 每块 128 个元素
    //   char (1字节): 每块 512 个元素
    //   大对象 (>512字节): 每块 1 个元素

    // 扩容时只需分配一个新块，不移动已有数据
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);   // 当前块满了 → 分配新块，O(1)
        d.push_front(-i); // 前端块满了 → 分配新块，O(1)
    }

    // deque 的中央映射表(map)也可能扩容
    // 但映射表存的是指针，扩容代价远小于 vector
}
```

- **块大小**：GCC 默认 `max(512 / sizeof(T), 1)` 个元素
- **扩容方式**：新增一个块，**不需要移动已有元素**
- **中央映射表**：存储各块指针的数组，满时翻倍扩容（代价很小）

#### list / forward_list — 无扩容

```cpp
#include <list>
#include <forward_list>

void list_growth() {
    std::list<int> lst;
    // 每次 push_back/push_front/insert 都独立分配一个节点
    // 节点结构(list): { T data; Node* prev; Node* next; }
    // 节点结构(forward_list): { T data; Node* next; }

    for (int i = 0; i < 1000; ++i) {
        lst.push_back(i);  // 每次单独 new 一个节点，O(1)
    }
    // 无"扩容"概念，但大量小分配可能导致内存碎片
}
```

- **无扩容**：每个元素独立分配/释放
- **缺点**：大量小内存分配，cache 不友好，内存碎片

#### string 扩容

```cpp
#include <string>
#include <iostream>

void string_growth() {
    std::string s;
    size_t prev_cap = 0;
    for (int i = 0; i < 200; ++i) {
        s += 'x';
        if (s.capacity() != prev_cap) {
            std::cout << "size=" << s.size()
                      << " capacity=" << s.capacity() << "\n";
            prev_cap = s.capacity();
        }
    }
}
// GCC 典型输出:
// size=1  capacity=15   ← SSO 缓冲区（不分配堆内存）
// size=16 capacity=30   ← 超过 SSO 阈值，堆分配，之后 2 倍增长
// size=31 capacity=60
// size=61 capacity=120
// size=121 capacity=240
```

- **SSO 阶段**：GCC/MSVC 可存 15 字符，libc++ 可存 22 字符，无堆分配
- **堆分配后**：扩容策略与 vector 一致（GCC 2倍，MSVC 1.5倍）

#### unordered_set / unordered_map 扩容 (rehash)

```cpp
#include <unordered_set>
#include <iostream>

void unordered_growth() {
    std::unordered_set<int> us;
    std::cout << "初始 bucket_count: " << us.bucket_count() << "\n";
    // GCC: 通常为 1
    // MSVC: 通常为 8

    std::cout << "max_load_factor: " << us.max_load_factor() << "\n";
    // 默认 1.0

    size_t prev_buckets = us.bucket_count();
    for (int i = 0; i < 100; ++i) {
        us.insert(i);
        if (us.bucket_count() != prev_buckets) {
            std::cout << "size=" << us.size()
                      << " buckets=" << us.bucket_count()
                      << " load_factor=" << us.load_factor() << "\n";
            prev_buckets = us.bucket_count();
        }
    }
    // GCC 典型桶数序列: 1→3→7→17→37→79→167→...（接近翻倍的质数）
    // MSVC 典型桶数序列: 8→64→512→...（翻倍）

    // 手动控制
    us.reserve(10000);          // 预分配足够桶数以容纳 n 个元素
    us.rehash(200);             // 直接设置桶数
    us.max_load_factor(0.5f);   // 降低负载因子 → 更多桶 → 更少冲突
}
```

- **触发条件**：`load_factor() = size() / bucket_count() > max_load_factor()` 时自动 rehash
- **rehash 代价**：O(n)，所有元素重新计算哈希值并分配到新桶
- **桶数增长**：GCC 使用接近 2 倍的**质数**序列，MSVC 使用 2 的幂次

#### set / map — 无扩容

```cpp
#include <set>
#include <map>

void tree_growth() {
    std::set<int> s;
    // 底层红黑树，每次 insert 分配一个树节点
    // 节点结构: { T data; Node* left; Node* right; Node* parent; Color color; }

    for (int i = 0; i < 1000; ++i) {
        s.insert(i);  // 分配一个节点 + O(log n) 旋转调整
    }
    // 无扩容概念，插入后通过旋转保持平衡
}
```

- **无扩容**：每次插入分配一个独立节点，通过旋转保持红黑树平衡
- 节点开销较大：每个节点除了数据还存储 3 个指针 + 颜色标记

### 6.3 扩容策略面试要点

**Q: 为什么 vector 选择 2 倍或 1.5 倍扩容，而不是固定增量？**

```
固定增量（如每次+100）：
  - n 次 push_back 的总拷贝次数 ≈ n²/200 → O(n²)

倍数增长（如每次 ×2）：
  - n 次 push_back 的总拷贝次数 ≈ 2n → O(n)
  - 均摊每次 push_back 为 O(1)
```

**Q: 2 倍 vs 1.5 倍的取舍？**

```
2 倍（GCC/Clang）：
  - 优点：扩容次数更少
  - 缺点：新 capacity 总是 > 之前所有 capacity 之和
           → 之前释放的内存永远无法被复用

1.5 倍（MSVC）：
  - 优点：经过几次扩容后，之前释放的内存总和 > 新需要的大小
           → 内存分配器有机会复用旧内存
  - 缺点：扩容次数稍多

实际差异在大多数场景下不显著，使用 reserve() 才是避免扩容的最佳实践。
```

**Q: 如何避免不必要的扩容？**

```cpp
// 1. vector/string: 使用 reserve()
std::vector<int> v;
v.reserve(10000);  // 预分配

// 2. unordered 容器: 使用 reserve() 或 rehash()
std::unordered_map<int, int> m;
m.reserve(10000);  // 预分配足够桶

// 3. 使用初始化列表或迭代器范围构造
std::vector<int> v2(arr.begin(), arr.end());  // 一次分配

// 4. shrink_to_fit() 释放多余空间（C++11，非强制）
v.shrink_to_fit();
```

---

## 7. 性能优化技巧

### 7.1 容器性能优化建议

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

## 8. 常见面试题

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

## 9. 小对象优化 (Small Object Optimization)

### 9.1 什么是 SSO (Small String Optimization)？

**问题描述**：`std::string` 如何优化短字符串的存储？

**SSO 原理**：

```cpp
#include <string>
#include <iostream>

// std::string 的典型内存布局（简化）
// 未优化版本：
struct NaiveString {
    char* data;         // 8 字节（64位）
    size_t size;        // 8 字节
    size_t capacity;    // 8 字节
    // 总共 24 字节，但即使存 "hi" 也要 heap 分配
};

// SSO 优化版本（概念）：
struct SSOString {
    union {
        struct {
            char* data;
            size_t size;
            size_t capacity;
        } heap;                  // 长字符串：堆分配
        struct {
            char buffer[23];     // 短字符串：栈内存
            unsigned char size;  // 剩余 1 字节存长度
        } local;                 // 短字符串直接存在对象内
    };
    // 通过某种标志位区分是 heap 还是 local
};

void sso_demo() {
    std::string short_str = "hello";      // SSO：存在栈上
    std::string long_str(100, 'x');       // 堆分配

    // 验证 SSO 效果
    std::cout << "sizeof(string): " << sizeof(std::string) << "\n";  // 通常 32 字节

    // 短字符串不触发堆分配
    std::string s1 = "short";
    std::cout << "capacity: " << s1.capacity() << "\n";  // 通常 15 或 22

    // 超过 SSO 阈值才分配
    std::string s2(100, 'x');
    std::cout << "capacity: " << s2.capacity() << "\n";  // >= 100
}
```

**各实现的 SSO 阈值**：

| 实现 | sizeof(string) | SSO 阈值（含 '\0'） |
|------|---------------|-------------------|
| GCC (libstdc++) | 32 字节 | **15 字符** |
| Clang (libc++) | 24 字节 | **22 字符** |
| MSVC | 32 字节 | **15 字符** |

**验证 SSO**：

```cpp
#include <string>
#include <iostream>

void verify_sso() {
    // 测试 SSO 边界
    for (int len = 10; len <= 30; ++len) {
        std::string s(len, 'x');
        // 如果 capacity 远大于 len，说明发生了堆分配
        std::cout << "len=" << len
                  << ", capacity=" << s.capacity()
                  << ", SSO=" << (s.capacity() < 32 ? "yes" : "no") << "\n";
    }
}

// 输出示例（GCC）：
// len=10, capacity=15, SSO=yes
// len=15, capacity=15, SSO=yes
// len=16, capacity=30, SSO=no  ← 超过阈值，堆分配
```

**SSO 的性能影响**：

```cpp
#include <chrono>
#include <string>
#include <vector>

void sso_performance() {
    const int N = 1000000;

    // 短字符串（SSO）
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        std::string s = "hello";  // SSO，无堆分配
    }
    auto end1 = std::chrono::high_resolution_clock::now();

    // 长字符串（堆分配）
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        std::string s = "this is a very long string that exceeds SSO threshold";
    }
    auto end2 = std::chrono::high_resolution_clock::now();

    auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    auto time2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    std::cout << "Short string (SSO): " << time1 << " ms\n";
    std::cout << "Long string (heap): " << time2 << " ms\n";
    // 短字符串通常快 2-5 倍
}
```

**关键要点**：

| 要点 | 说明 |
|------|------|
| SSO 目的 | 避免短字符串的堆分配，提升性能 |
| SSO 阈值 | 通常 15-22 字符（取决于实现） |
| 空间换时间 | `sizeof(string)` 增大，但短字符串无需 malloc |
| 移动语义影响 | SSO 字符串移动时仍需拷贝数据 |

**常见陷阱**：

```cpp
// 陷阱 1：SSO 字符串的移动并不总是 O(1)
std::string s1 = "short";
std::string s2 = std::move(s1);  // 实际是拷贝！因为 SSO

std::string s3(100, 'x');
std::string s4 = std::move(s3);  // 真正的移动（指针交换）

// 陷阱 2：c_str() 返回的指针
std::string s = "hello";
const char* p = s.c_str();
s += " world";  // 可能导致 SSO -> heap 迁移
// p 可能失效！
```

---

### 9.2 SBO (Small Buffer Optimization) 在其他容器中的应用？

**问题描述**：SSO 的思想在其他地方的应用。

**`std::function` 的 SBO**：

```cpp
#include <functional>
#include <iostream>

void sbo_function() {
    // 小闭包：存在 std::function 内部
    int x = 10;
    std::function<int()> f1 = [x]() { return x; };  // SBO

    // 大闭包：堆分配
    int arr[100];
    std::function<int()> f2 = [arr]() { return arr[0]; };  // 堆分配

    std::cout << "sizeof(function): " << sizeof(f1) << "\n";  // 通常 32 字节
}
```

**`std::any` 的 SBO**：

```cpp
#include <any>

void sbo_any() {
    // 小对象：存在 std::any 内部
    std::any a1 = 42;           // SBO
    std::any a2 = 3.14;         // SBO

    // 大对象：堆分配
    std::any a3 = std::string(100, 'x');  // 可能堆分配
}
```

**SBO 的通用实现思路**：

```cpp
// SBO 容器的典型结构
template<typename T, size_t BufferSize = 64>
class SBOContainer {
    union {
        T* heap_ptr;                    // 堆指针
        alignas(T) char buffer[BufferSize];  // 内部缓冲区
    };
    bool is_local;  // 标记是否使用内部缓冲区

public:
    template<typename U>
    void store(U&& value) {
        if constexpr (sizeof(U) <= BufferSize && std::is_nothrow_move_constructible_v<U>) {
            // 小对象：放在内部缓冲区
            new (buffer) T(std::forward<U>(value));
            is_local = true;
        } else {
            // 大对象：堆分配
            heap_ptr = new T(std::forward<U>(value));
            is_local = false;
        }
    }

    ~SBOContainer() {
        if (is_local) {
            reinterpret_cast<T*>(buffer)->~T();
        } else {
            delete heap_ptr;
        }
    }
};
```

**关键要点**：

| 容器 | SBO 阈值 | 说明 |
|------|----------|------|
| `std::string` | 15-22 字符 | SSO |
| `std::function` | ~16-32 字节 | 小闭包优化 |
| `std::any` | ~16-32 字节 | 小对象优化 |
| 自定义容器 | 可配置 | 根据使用场景调整 |

**SBO 的权衡**：

```cpp
// 优点：
// - 避免小对象的堆分配
// - 更好的缓存局部性
// - 减少内存碎片

// 缺点：
// - 对象本身更大（sizeof 增加）
// - 移动小对象时仍需拷贝
// - 实现复杂度增加
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
