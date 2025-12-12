# 08 - STL 算法与迭代器 (STL Algorithms & Iterators)

## 简介

STL 算法是 C++ 标准库提供的通用算法集合，通过迭代器与容器解耦，实现了高度的泛型性和复用性。

**核心特点**：
- **泛型编程**：通过模板和迭代器适用于多种容器
- **高效实现**：经过优化的标准实现
- **声明式编程**：表达"做什么"而非"怎么做"

---

## 1. 迭代器 (Iterators)

### 1.1 迭代器的五种分类？

**问题描述**：STL 迭代器有哪些类型？各有什么特点？

**迭代器层次结构**：

```
输入迭代器 (Input Iterator)         输出迭代器 (Output Iterator)
         ↓                                    ↓
    前向迭代器 (Forward Iterator) ←──────────┘
         ↓
    双向迭代器 (Bidirectional Iterator)
         ↓
    随机访问迭代器 (Random Access Iterator)
         ↓
    连续迭代器 (Contiguous Iterator, C++20)
```

**各类型特性**：

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <forward_list>
#include <iterator>

void iterator_categories() {
    // 1. 输入迭代器 (Input Iterator)
    // - 只读，单次遍历
    // - 示例：istream_iterator
    // 支持：++, *, ==, !=

    // 2. 输出迭代器 (Output Iterator)
    // - 只写，单次遍历
    // - 示例：ostream_iterator, back_inserter
    // 支持：++, *

    // 3. 前向迭代器 (Forward Iterator)
    // - 可读写，多次遍历
    // - 示例：forward_list::iterator
    // 支持：++, *, ==, !=
    std::forward_list<int> flist = {1, 2, 3};
    auto fit = flist.begin();
    // fit--;  // ❌ 不支持

    // 4. 双向迭代器 (Bidirectional Iterator)
    // - 可读写，多次遍历，可双向移动
    // - 示例：list::iterator, set::iterator
    // 支持：++, --, *, ==, !=
    std::list<int> lst = {1, 2, 3};
    auto lit = lst.begin();
    ++lit;  // ✅
    --lit;  // ✅

    // 5. 随机访问迭代器 (Random Access Iterator)
    // - 可读写，多次遍历，支持随机访问
    // - 示例：vector::iterator, deque::iterator
    // 支持：++, --, +, -, +=, -=, [], <, >, <=, >=
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto vit = vec.begin();
    vit += 3;            // ✅ 随机访问
    std::cout << vit[1] << "\n";  // ✅ 下标访问
    std::cout << (vit - vec.begin()) << "\n";  // ✅ 距离计算
}
```

**算法对迭代器的要求**：

```cpp
#include <algorithm>
#include <vector>
#include <list>

void algorithm_iterator_requirements() {
    std::vector<int> vec = {5, 2, 8, 1, 9};
    std::list<int> lst = {5, 2, 8, 1, 9};

    // sort: 要求随机访问迭代器
    std::sort(vec.begin(), vec.end());  // ✅
    // std::sort(lst.begin(), lst.end());  // ❌ 编译错误

    // list 有自己的 sort 成员函数
    lst.sort();  // ✅

    // find: 只要求输入迭代器
    auto it1 = std::find(vec.begin(), vec.end(), 8);  // ✅
    auto it2 = std::find(lst.begin(), lst.end(), 8);  // ✅

    // reverse: 要求双向迭代器
    std::reverse(vec.begin(), vec.end());  // ✅
    std::reverse(lst.begin(), lst.end());  // ✅
}
```

**关键要点**：
- 迭代器类型决定了可用的算法
- 容器提供符合其特性的迭代器类型
- 算法根据迭代器类型选择最优实现（标签分派）

---

### 1.2 迭代器适配器有哪些？

**问题描述**：如何使用插入迭代器、反向迭代器、移动迭代器？

**1. 插入迭代器 (Insert Iterators)**：

```cpp
#include <iterator>
#include <vector>
#include <algorithm>

void insert_iterators() {
    std::vector<int> src = {1, 2, 3, 4, 5};
    std::vector<int> dst;

    // back_inserter: 尾部插入
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    // dst: {1, 2, 3, 4, 5}

    // front_inserter: 头部插入（需要 push_front）
    std::list<int> lst;
    std::copy(src.begin(), src.end(), std::front_inserter(lst));
    // lst: {5, 4, 3, 2, 1} (逆序)

    // inserter: 指定位置插入
    std::vector<int> vec = {10, 20, 30};
    std::copy(src.begin(), src.end(), std::inserter(vec, vec.begin() + 1));
    // vec: {10, 1, 2, 3, 4, 5, 20, 30}
}
```

**2. 反向迭代器 (Reverse Iterators)**：

```cpp
void reverse_iterators() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // 正向遍历
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";  // 1 2 3 4 5
    }

    // 反向遍历
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        std::cout << *it << " ";  // 5 4 3 2 1
    }

    // 反向迭代器与正向迭代器转换
    auto rit = vec.rbegin();
    ++rit;  // 指向 4
    auto it = rit.base();  // 转换为正向迭代器，指向 5（注意偏移）
}
```

**3. 移动迭代器 (Move Iterators, C++11)**：

```cpp
#include <string>

void move_iterators() {
    std::vector<std::string> src = {"hello", "world", "cpp"};
    std::vector<std::string> dst;

    // 拷贝
    dst.clear();
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    // src 仍有效

    // 移动（避免拷贝）
    dst.clear();
    std::copy(std::make_move_iterator(src.begin()),
              std::make_move_iterator(src.end()),
              std::back_inserter(dst));
    // src 中的字符串已被移动，处于有效但未指定状态
}
```

**关键要点**：
- **插入迭代器**：避免手动扩容，自动调用 `push_back`/`push_front`/`insert`
- **反向迭代器**：`base()` 返回的正向迭代器指向下一个位置
- **移动迭代器**：用于优化性能，避免不必要的拷贝

---

## 2. 非修改序列算法 (Non-modifying Sequence Operations)

### 2.1 查找算法：`find` vs `find_if` vs `binary_search`？

**问题描述**：如何在容器中查找元素？

**线性查找**：

```cpp
#include <algorithm>
#include <vector>

void linear_search() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // find: 查找值
    auto it1 = std::find(vec.begin(), vec.end(), 3);
    if (it1 != vec.end()) {
        std::cout << "找到: " << *it1 << "\n";
    }

    // find_if: 查找满足条件的元素
    auto it2 = std::find_if(vec.begin(), vec.end(),
                            [](int x) { return x > 3; });
    if (it2 != vec.end()) {
        std::cout << "第一个 > 3 的元素: " << *it2 << "\n";  // 4
    }

    // find_if_not: 查找不满足条件的元素
    auto it3 = std::find_if_not(vec.begin(), vec.end(),
                                 [](int x) { return x % 2 == 0; });
    // 找到第一个奇数

    // count: 统计出现次数
    int cnt = std::count(vec.begin(), vec.end(), 3);

    // count_if: 统计满足条件的个数
    int even_cnt = std::count_if(vec.begin(), vec.end(),
                                  [](int x) { return x % 2 == 0; });
}
```

**二分查找（要求有序）**：

```cpp
void binary_search_demo() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 5, 5, 6, 7};

    // binary_search: 判断是否存在
    bool exists = std::binary_search(vec.begin(), vec.end(), 5);  // true

    // lower_bound: 第一个 >= 值的位置
    auto it1 = std::lower_bound(vec.begin(), vec.end(), 5);
    std::cout << "lower_bound(5): " << (it1 - vec.begin()) << "\n";  // 4

    // upper_bound: 第一个 > 值的位置
    auto it2 = std::upper_bound(vec.begin(), vec.end(), 5);
    std::cout << "upper_bound(5): " << (it2 - vec.begin()) << "\n";  // 7

    // equal_range: 返回 [lower_bound, upper_bound)
    auto [first, last] = std::equal_range(vec.begin(), vec.end(), 5);  // C++17
    std::cout << "5 出现 " << (last - first) << " 次\n";  // 3
}
```

**时间复杂度对比**：

| 算法 | 时间复杂度 | 要求 |
|------|-----------|------|
| `find` | O(n) | 无 |
| `find_if` | O(n) | 无 |
| `binary_search` | O(log n) | 有序 |
| `lower_bound` | O(log n) | 有序 |
| `upper_bound` | O(log n) | 有序 |

**常见陷阱**：
```cpp
// ❌ 错误：在无序容器上使用二分查找
std::vector<int> vec = {5, 1, 3, 2, 4};
auto it = std::lower_bound(vec.begin(), vec.end(), 3);  // ❌ 结果未定义

// ✅ 正确：先排序
std::sort(vec.begin(), vec.end());
auto it2 = std::lower_bound(vec.begin(), vec.end(), 3);  // ✅
```

---

### 2.2 `all_of` / `any_of` / `none_of` 的使用？

**问题描述**：如何判断容器中元素是否满足条件？

**实现示例**：

```cpp
#include <algorithm>
#include <vector>

void check_conditions() {
    std::vector<int> vec = {2, 4, 6, 8, 10};

    // all_of: 所有元素都满足条件
    bool all_even = std::all_of(vec.begin(), vec.end(),
                                 [](int x) { return x % 2 == 0; });
    std::cout << "all_even: " << all_even << "\n";  // true

    // any_of: 至少一个元素满足条件
    bool has_large = std::any_of(vec.begin(), vec.end(),
                                  [](int x) { return x > 5; });
    std::cout << "has_large: " << has_large << "\n";  // true

    // none_of: 没有元素满足条件
    bool no_odd = std::none_of(vec.begin(), vec.end(),
                                [](int x) { return x % 2 == 1; });
    std::cout << "no_odd: " << no_odd << "\n";  // true
}
```

**关键要点**：
- 短路求值：找到第一个不满足/满足的元素即停止
- 空容器：`all_of` 和 `none_of` 返回 `true`，`any_of` 返回 `false`

---

## 3. 修改序列算法 (Modifying Sequence Operations)

### 3.1 `copy` / `copy_if` / `transform` 的区别？

**问题描述**：如何拷贝和转换容器元素？

**拷贝算法**：

```cpp
#include <algorithm>
#include <vector>

void copy_algorithms() {
    std::vector<int> src = {1, 2, 3, 4, 5};
    std::vector<int> dst(5);  // 必须预先分配空间

    // copy: 拷贝所有元素
    std::copy(src.begin(), src.end(), dst.begin());

    // copy_if: 条件拷贝
    std::vector<int> evens;
    std::copy_if(src.begin(), src.end(), std::back_inserter(evens),
                 [](int x) { return x % 2 == 0; });
    // evens: {2, 4}

    // copy_n: 拷贝 n 个元素
    std::vector<int> first3;
    std::copy_n(src.begin(), 3, std::back_inserter(first3));
    // first3: {1, 2, 3}
}
```

**转换算法**：

```cpp
void transform_demo() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<int> result(5);

    // 一元操作：每个元素平方
    std::transform(vec.begin(), vec.end(), result.begin(),
                   [](int x) { return x * x; });
    // result: {1, 4, 9, 16, 25}

    // 二元操作：两个容器对应元素相加
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {10, 20, 30};
    std::vector<int> sum(3);
    std::transform(v1.begin(), v1.end(), v2.begin(), sum.begin(),
                   [](int a, int b) { return a + b; });
    // sum: {11, 22, 33}
}
```

**关键要点**：
- `copy` 系列：不改变元素值
- `transform`：可以改变元素类型和值
- 目标容器需要足够空间，或使用插入迭代器

---

### 3.2 `fill` / `generate` / `iota` 的使用？

**问题描述**：如何填充容器？

```cpp
#include <algorithm>
#include <numeric>
#include <vector>

void fill_algorithms() {
    std::vector<int> vec(5);

    // fill: 填充固定值
    std::fill(vec.begin(), vec.end(), 42);
    // vec: {42, 42, 42, 42, 42}

    // fill_n: 填充 n 个元素
    std::fill_n(vec.begin(), 3, 99);
    // vec: {99, 99, 99, 42, 42}

    // generate: 使用函数生成
    int n = 0;
    std::generate(vec.begin(), vec.end(), [&n] { return n++; });
    // vec: {0, 1, 2, 3, 4}

    // iota: 填充递增序列 (C++11)
    std::iota(vec.begin(), vec.end(), 10);
    // vec: {10, 11, 12, 13, 14}
}
```

---

### 3.3 `remove` / `remove_if` 与 erase-remove idiom？

**问题描述**：如何删除容器中的元素？

**remove 陷阱**：

```cpp
#include <algorithm>
#include <vector>

void remove_demo() {
    std::vector<int> vec = {1, 2, 3, 2, 4, 2, 5};

    // ❌ remove 不会真正删除元素！
    auto new_end = std::remove(vec.begin(), vec.end(), 2);
    std::cout << "size: " << vec.size() << "\n";  // 仍然是 7！

    // remove 只是将非删除元素移到前面，返回新的逻辑结尾
    // vec: {1, 3, 4, 5, ?, ?, ?}
    //                   ↑ new_end

    // ✅ erase-remove idiom：真正删除
    vec.erase(new_end, vec.end());
    std::cout << "size: " << vec.size() << "\n";  // 4
    // vec: {1, 3, 4, 5}
}

void remove_if_demo() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};

    // 删除所有偶数
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [](int x) { return x % 2 == 0; }),
              vec.end());
    // vec: {1, 3, 5}
}
```

**关键要点**：
- `remove` 系列算法不改变容器大小
- 必须配合 `erase` 使用（erase-remove idiom）
- C++20 引入了 `std::erase_if` 简化操作

```cpp
// C++20
#include <vector>
void erase_if_demo() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};
    std::erase_if(vec, [](int x) { return x % 2 == 0; });
    // vec: {1, 3, 5}
}
```

---

## 4. 排序与相关算法 (Sorting Operations)

### 4.1 `sort` / `stable_sort` / `partial_sort` 的区别？

**问题描述**：不同排序算法的特点和使用场景。

**排序算法对比**：

```cpp
#include <algorithm>
#include <vector>

struct Person {
    std::string name;
    int age;
};

void sorting_algorithms() {
    // sort: 不稳定排序，O(n log n) 平均
    std::vector<int> vec1 = {5, 2, 8, 1, 9};
    std::sort(vec1.begin(), vec1.end());
    // vec1: {1, 2, 5, 8, 9}

    // 自定义比较器
    std::sort(vec1.begin(), vec1.end(), std::greater<int>());
    // vec1: {9, 8, 5, 2, 1}

    // stable_sort: 稳定排序，O(n log² n) 最坏
    std::vector<Person> people = {
        {"Alice", 25},
        {"Bob", 25},
        {"Charlie", 22}
    };
    std::stable_sort(people.begin(), people.end(),
                     [](const Person& a, const Person& b) {
                         return a.age < b.age;
                     });
    // Charlie -> Alice -> Bob (Alice 和 Bob 相对顺序不变)

    // partial_sort: 部分排序，O(n log k)
    std::vector<int> vec2 = {5, 2, 8, 1, 9, 3, 7};
    std::partial_sort(vec2.begin(), vec2.begin() + 3, vec2.end());
    // vec2: {1, 2, 3, ?, ?, ?, ?}（前 3 个有序，其余未定义）

    // nth_element: 找第 n 小的元素，O(n) 平均
    std::vector<int> vec3 = {5, 2, 8, 1, 9, 3, 7};
    std::nth_element(vec3.begin(), vec3.begin() + 3, vec3.end());
    // vec3[3] 是第 4 小的元素
    // vec3[0..2] 都 <= vec3[3] <= vec3[4..6]
}
```

**时间复杂度对比**：

| 算法 | 平均 | 最坏 | 稳定性 | 适用场景 |
|------|------|------|--------|---------|
| `sort` | O(n log n) | O(n log n) | ❌ | 通用排序 |
| `stable_sort` | O(n log n) | O(n log² n) | ✅ | 需要保持相对顺序 |
| `partial_sort` | O(n log k) | O(n log k) | ❌ | 只需要前 k 个有序 |
| `nth_element` | O(n) | O(n²) | ❌ | 找第 k 小/大元素 |

**关键要点**：
- **`sort`**：通常使用 Introsort（快排+堆排序+插入排序）
- **`stable_sort`**：通常使用归并排序
- **`partial_sort`**：使用堆排序
- **`nth_element`**：使用快速选择算法

---

### 4.2 `partition` / `stable_partition` 的使用？

**问题描述**：如何根据条件划分容器？

```cpp
#include <algorithm>
#include <vector>

void partition_demo() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // partition: 不稳定划分
    auto mid = std::partition(vec.begin(), vec.end(),
                              [](int x) { return x % 2 == 0; });
    // 偶数在前，奇数在后，但顺序可能改变
    // vec: {8, 2, 6, 4, 5, 3, 7, 1, 9}
    //                   ↑ mid

    // stable_partition: 稳定划分
    vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    mid = std::stable_partition(vec.begin(), vec.end(),
                                 [](int x) { return x % 2 == 0; });
    // vec: {2, 4, 6, 8, 1, 3, 5, 7, 9}（相对顺序不变）

    // is_partitioned: 判断是否已划分
    bool partitioned = std::is_partitioned(vec.begin(), vec.end(),
                                           [](int x) { return x % 2 == 0; });

    // partition_point: 找到划分点
    auto point = std::partition_point(vec.begin(), vec.end(),
                                      [](int x) { return x % 2 == 0; });
}
```

**应用场景**：
- 快速排序的核心操作
- 将满足条件的元素移到前面

---

## 5. 二分查找算法详解

### 5.1 `lower_bound` / `upper_bound` / `equal_range` 的使用？

**问题描述**：如何在有序序列中高效查找？

**详细示例**：

```cpp
#include <algorithm>
#include <vector>

void binary_search_detailed() {
    std::vector<int> vec = {1, 2, 3, 3, 3, 4, 5};
    //                       0  1  2  3  4  5  6 (索引)

    // lower_bound: 第一个 >= 值的位置
    auto lb = std::lower_bound(vec.begin(), vec.end(), 3);
    std::cout << "lower_bound(3): " << (lb - vec.begin()) << "\n";  // 2

    // upper_bound: 第一个 > 值的位置
    auto ub = std::upper_bound(vec.begin(), vec.end(), 3);
    std::cout << "upper_bound(3): " << (ub - vec.begin()) << "\n";  // 5

    // equal_range: [lower_bound, upper_bound)
    auto [first, last] = std::equal_range(vec.begin(), vec.end(), 3);
    std::cout << "equal_range(3): [" << (first - vec.begin())
              << ", " << (last - vec.begin()) << ")\n";  // [2, 5)

    // 统计出现次数
    int count = last - first;  // 3

    // 插入位置（保持有序）
    auto insert_pos = std::lower_bound(vec.begin(), vec.end(), 3);
    vec.insert(insert_pos, 3);  // 插入到第一个 3 的位置
}
```

**查找边界的应用**：

```cpp
// 应用 1：判断元素是否存在
bool contains(const std::vector<int>& vec, int value) {
    auto it = std::lower_bound(vec.begin(), vec.end(), value);
    return it != vec.end() && *it == value;
}

// 应用 2：查找范围内的元素个数
int count_in_range(const std::vector<int>& vec, int low, int high) {
    auto lb = std::lower_bound(vec.begin(), vec.end(), low);
    auto ub = std::upper_bound(vec.begin(), vec.end(), high);
    return ub - lb;
}

// 应用 3：去重
std::vector<int> unique_sorted(std::vector<int> vec) {
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    return vec;
}
```

---

## 6. 堆算法 (Heap Operations)

### 6.1 如何手动维护堆？

**问题描述**：不使用 `priority_queue`，如何操作堆？

```cpp
#include <algorithm>
#include <vector>

void heap_operations() {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6};

    // make_heap: 建堆，O(n)
    std::make_heap(vec.begin(), vec.end());
    // vec: {9, 6, 4, 3, 5, 1, 2, 1} (大顶堆)

    std::cout << "堆顶: " << vec.front() << "\n";  // 9

    // push_heap: 插入元素到堆
    vec.push_back(10);  // 先在尾部添加
    std::push_heap(vec.begin(), vec.end());  // 然后调整堆
    std::cout << "新堆顶: " << vec.front() << "\n";  // 10

    // pop_heap: 删除堆顶
    std::pop_heap(vec.begin(), vec.end());  // 将堆顶移到末尾
    int top = vec.back();  // 获取堆顶
    vec.pop_back();  // 删除末尾
    std::cout << "弹出: " << top << "\n";  // 10

    // sort_heap: 堆排序，O(n log n)
    std::sort_heap(vec.begin(), vec.end());
    // vec 变为有序序列（升序）

    // is_heap: 判断是否为堆
    bool is_heap = std::is_heap(vec.begin(), vec.end());

    // is_heap_until: 找到第一个破坏堆性质的位置
    auto it = std::is_heap_until(vec.begin(), vec.end());
}

// 自定义比较器（小顶堆）
void min_heap_demo() {
    std::vector<int> vec = {3, 1, 4, 1, 5};
    std::make_heap(vec.begin(), vec.end(), std::greater<int>());
    // 小顶堆
}
```

**关键要点**：
- `make_heap` 只调整顺序，不分配内存
- `push_heap` / `pop_heap` 要求容器已经是堆
- 堆的根节点在 `vec[0]`

---

## 7. 集合操作 (Set Operations)

### 7.1 有序集合的交并差运算？

**问题描述**：如何对有序容器进行集合操作？

```cpp
#include <algorithm>
#include <vector>

void set_operations() {
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector<int> v2 = {3, 4, 5, 6, 7};
    std::vector<int> result;

    // set_union: 并集
    std::set_union(v1.begin(), v1.end(),
                   v2.begin(), v2.end(),
                   std::back_inserter(result));
    // result: {1, 2, 3, 4, 5, 6, 7}

    // set_intersection: 交集
    result.clear();
    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(result));
    // result: {3, 4, 5}

    // set_difference: 差集（v1 - v2）
    result.clear();
    std::set_difference(v1.begin(), v1.end(),
                        v2.begin(), v2.end(),
                        std::back_inserter(result));
    // result: {1, 2}

    // set_symmetric_difference: 对称差（v1 ⊕ v2）
    result.clear();
    std::set_symmetric_difference(v1.begin(), v1.end(),
                                   v2.begin(), v2.end(),
                                   std::back_inserter(result));
    // result: {1, 2, 6, 7}

    // includes: 判断是否为子集
    bool is_subset = std::includes(v1.begin(), v1.end(),
                                   v2.begin(), v2.end());  // false
}
```

**注意事项**：
- 要求输入容器已排序
- 输出容器也是有序的
- 允许重复元素

---

## 8. 最值与求和算法

### 8.1 `min_element` / `max_element` / `minmax_element` 的使用？

```cpp
#include <algorithm>
#include <numeric>
#include <vector>

void minmax_demo() {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6};

    // min_element: 最小值迭代器
    auto min_it = std::min_element(vec.begin(), vec.end());
    std::cout << "min: " << *min_it << " at " << (min_it - vec.begin()) << "\n";

    // max_element: 最大值迭代器
    auto max_it = std::max_element(vec.begin(), vec.end());
    std::cout << "max: " << *max_it << " at " << (max_it - vec.begin()) << "\n";

    // minmax_element: 同时找最小和最大值 (C++11)
    auto [min_it2, max_it2] = std::minmax_element(vec.begin(), vec.end());
    std::cout << "min: " << *min_it2 << ", max: " << *max_it2 << "\n";

    // accumulate: 求和
    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    std::cout << "sum: " << sum << "\n";  // 31

    // accumulate: 自定义操作（求积）
    int product = std::accumulate(vec.begin(), vec.end(), 1,
                                  [](int a, int b) { return a * b; });
}
```

---

## 9. Lambda 表达式与算法

### 9.1 如何在算法中使用 Lambda？

**问题描述**：Lambda 表达式简化算法使用。

```cpp
#include <algorithm>
#include <vector>

void lambda_with_algorithms() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // 无捕获
    std::for_each(vec.begin(), vec.end(), [](int x) {
        std::cout << x << " ";
    });

    // 按值捕获
    int threshold = 3;
    auto it = std::find_if(vec.begin(), vec.end(), [threshold](int x) {
        return x > threshold;
    });

    // 按引用捕获
    int sum = 0;
    std::for_each(vec.begin(), vec.end(), [&sum](int x) {
        sum += x;
    });
    std::cout << "sum: " << sum << "\n";

    // 泛型 lambda (C++14)
    auto print = [](const auto& x) {
        std::cout << x << "\n";
    };

    // 捕获所有 (谨慎使用)
    auto lambda1 = [=](int x) { return x > threshold; };  // 按值捕获所有
    auto lambda2 = [&](int x) { sum += x; };              // 按引用捕获所有
}
```

---

## 10. 常见陷阱与最佳实践

### 10.1 算法使用的常见陷阱

**陷阱 1：目标容器空间不足**

```cpp
// ❌ 错误：目标容器未分配空间
std::vector<int> src = {1, 2, 3};
std::vector<int> dst;
std::copy(src.begin(), src.end(), dst.begin());  // ❌ 未定义行为

// ✅ 正确：预分配空间
std::vector<int> dst2(3);
std::copy(src.begin(), src.end(), dst2.begin());

// ✅ 或使用插入迭代器
std::vector<int> dst3;
std::copy(src.begin(), src.end(), std::back_inserter(dst3));
```

**陷阱 2：在无序容器上使用二分查找**

```cpp
// ❌ 错误：未排序
std::vector<int> vec = {5, 1, 3, 2, 4};
auto it = std::lower_bound(vec.begin(), vec.end(), 3);  // ❌ 结果未定义

// ✅ 正确：先排序
std::sort(vec.begin(), vec.end());
auto it2 = std::lower_bound(vec.begin(), vec.end(), 3);
```

**陷阱 3：`remove` 不真正删除元素**

```cpp
// ❌ 错误：remove 不改变容器大小
std::vector<int> vec = {1, 2, 3, 2, 4};
std::remove(vec.begin(), vec.end(), 2);
// vec.size() 仍然是 5！

// ✅ 正确：erase-remove idiom
vec.erase(std::remove(vec.begin(), vec.end(), 2), vec.end());
```

**陷阱 4：迭代器失效**

```cpp
// ❌ 错误：排序后迭代器失效
std::vector<int> vec = {5, 2, 8, 1, 9};
auto it = vec.begin() + 2;
std::sort(vec.begin(), vec.end());
// *it;  // ❌ 未定义行为（迭代器可能失效）

// ✅ 正确：使用下标或重新获取迭代器
size_t index = 2;
std::sort(vec.begin(), vec.end());
int value = vec[index];  // ✅
```

---

## 11. 性能优化技巧

### 11.1 选择合适的算法

```cpp
// ❌ 低效：查找最大的 k 个元素
std::vector<int> vec = {/*大量数据*/};
std::sort(vec.begin(), vec.end(), std::greater<int>());  // O(n log n)
vec.resize(k);

// ✅ 高效：使用 partial_sort
std::partial_sort(vec.begin(), vec.begin() + k, vec.end(),
                  std::greater<int>());  // O(n log k)
vec.resize(k);

// ✅ 或使用 nth_element + partition
std::nth_element(vec.begin(), vec.begin() + k, vec.end(),
                 std::greater<int>());  // O(n) 平均
vec.resize(k);
```

---

## 总结

### 算法分类速查

| 类别 | 常用算法 |
|------|---------|
| **查找** | `find`, `find_if`, `binary_search`, `lower_bound`, `upper_bound` |
| **统计** | `count`, `count_if`, `all_of`, `any_of`, `none_of` |
| **拷贝** | `copy`, `copy_if`, `copy_n`, `transform` |
| **填充** | `fill`, `fill_n`, `generate`, `iota` |
| **删除** | `remove`, `remove_if`, `unique` |
| **排序** | `sort`, `stable_sort`, `partial_sort`, `nth_element` |
| **划分** | `partition`, `stable_partition` |
| **堆** | `make_heap`, `push_heap`, `pop_heap`, `sort_heap` |
| **集合** | `set_union`, `set_intersection`, `set_difference` |
| **最值** | `min_element`, `max_element`, `minmax_element` |
| **求和** | `accumulate`, `reduce` (C++17) |

### 关键记忆点

1. **迭代器类型决定可用算法**
2. **二分查找要求有序**
3. **`remove` 不真正删除，需要配合 `erase`**
4. **目标容器需要足够空间，或使用插入迭代器**
5. **Lambda 表达式简化算法使用**
6. **选择合适的算法优化性能**
