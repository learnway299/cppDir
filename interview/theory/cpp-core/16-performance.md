# 16 - 性能优化 (Performance Optimization)

## 简介

性能优化是 C++ 的核心优势之一。理解 CPU 缓存、内联、分支预测等底层机制对于编写高性能代码至关重要。

**关键概念**：
- **CPU 缓存**：缓存友好的数据结构
- **内联**：减少函数调用开销
- **分支预测**：避免分支误判
- **编译器优化**：理解优化原理

---

## 1. CPU 缓存优化

### 1.1 缓存层次与性能影响？

**缓存层次**：
- L1 Cache：~1 ns，32-64 KB
- L2 Cache：~3-10 ns，256 KB - 1 MB
- L3 Cache：~10-20 ns，2-30 MB
- RAM：~100 ns，GB 级

**缓存行（Cache Line）**：通常 64 字节

```cpp
// ❌ 缓存不友好：跳跃访问
int sum = 0;
for (int i = 0; i < N; i++) {
    sum += data[random_index()];  // 缓存失效
}

// ✅ 缓存友好：顺序访问
int sum = 0;
for (int i = 0; i < N; i++) {
    sum += data[i];  // 预取和缓存命中
}
```

### 1.2 伪共享（False Sharing）？

```cpp
// ❌ 伪共享：两个线程修改相邻变量
struct BadCounter {
    std::atomic<int> counter1;  // 同一缓存行
    std::atomic<int> counter2;  // 同一缓存行
};

// ✅ 避免伪共享：填充对齐
struct alignas(64) GoodCounter {
    std::atomic<int> counter1;
    char padding[64 - sizeof(std::atomic<int>)];
};
```

### 1.3 数据结构的缓存友好性？

```cpp
// ❌ AoS（Array of Structures）：缓存不友好
struct Particle {
    float x, y, z;
    float vx, vy, vz;
};
std::vector<Particle> particles;

// 只需要位置时，也会加载速度数据
for (const auto& p : particles) {
    process_position(p.x, p.y, p.z);
}

// ✅ SoA（Structure of Arrays）：缓存友好
struct Particles {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
};

// 只加载需要的数据
for (size_t i = 0; i < particles.x.size(); i++) {
    process_position(particles.x[i], particles.y[i], particles.z[i]);
}
```

---

## 2. 函数内联

### 2.1 内联的作用？

```cpp
// 非内联：函数调用开销
int add(int a, int b) {
    return a + b;
}

// 内联：消除调用开销
inline int add_inline(int a, int b) {
    return a + b;
}

// 编译器展开为：
int result = a + b;  // 直接计算
```

### 2.2 何时内联？

**适合内联**：
- 小函数（1-5 行）
- 频繁调用
- 性能关键路径

**不适合内联**：
- 大函数（代码膨胀）
- 递归函数
- 虚函数（编译时无法确定）

```cpp
// ✅ 适合内联
inline int square(int x) {
    return x * x;
}

// ❌ 不适合内联
inline void large_function() {
    // 100 行代码...
}
```

### 2.3 强制内联？

```cpp
// GCC/Clang
__attribute__((always_inline)) inline
int force_inline(int x) {
    return x * 2;
}

// MSVC
__forceinline int force_inline(int x) {
    return x * 2;
}
```

---

## 3. 分支预测

### 3.1 分支预测的影响？

```cpp
// 分支预测失败：~10-20 个时钟周期损失

// ❌ 不可预测的分支
for (int i = 0; i < N; i++) {
    if (data[i] % 2 == 0) {  // 随机分支
        process_even(data[i]);
    } else {
        process_odd(data[i]);
    }
}

// ✅ 可预测的分支（已排序）
std::sort(data.begin(), data.end());
for (int i = 0; i < N; i++) {
    if (data[i] < threshold) {  // 连续的 true，然后连续的 false
        process_small(data[i]);
    } else {
        process_large(data[i]);
    }
}
```

### 3.2 无分支编程？

```cpp
// 分支版本
int abs_value(int x) {
    if (x < 0) return -x;
    else return x;
}

// 无分支版本
int abs_value_branchless(int x) {
    int mask = x >> 31;  // 全 0 或全 1
    return (x + mask) ^ mask;
}
```

### 3.3 分支提示？

```cpp
// GCC/Clang：分支提示
if (__builtin_expect(rare_condition, 0)) {
    // 不太可能执行的代码
}

// C++20：likely/unlikely
if (common_condition) [[likely]] {
    // 经常执行
} else [[unlikely]] {
    // 很少执行
}
```

---

## 4. 编译器优化

### 4.1 常见优化技术？

**1. 循环展开**：
```cpp
// 原始循环
for (int i = 0; i < N; i++) {
    sum += data[i];
}

// 编译器展开
for (int i = 0; i < N; i += 4) {
    sum += data[i];
    sum += data[i+1];
    sum += data[i+2];
    sum += data[i+3];
}
```

**2. 常量折叠**：
```cpp
int x = 2 * 3 + 4;  // 编译期计算为 10
```

**3. 死代码消除**：
```cpp
if (false) {
    // 这段代码被优化掉
}
```

### 4.2 如何帮助编译器优化？

**1. 使用 const**：
```cpp
void process(const int* data, int size) {
    // const 告诉编译器 data 不会被修改
}
```

**2. 使用 restrict（C99/C++）**：
```cpp
void copy(int* __restrict__ dest, const int* __restrict__ src, int n) {
    // 告诉编译器 dest 和 src 不重叠
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
```

**3. 避免别名**：
```cpp
// ❌ 编译器难以优化
void process(int& a, int& b) {
    a = 1;
    b = 2;
    int x = a;  // 编译器无法确定 a 是否被修改
}

// ✅ 使用局部变量
void process(int& a, int& b) {
    int local_a = a;
    int local_b = b;
    local_a = 1;
    local_b = 2;
    a = local_a;
    b = local_b;
}
```

---

## 5. 内存分配优化

### 5.1 避免频繁分配？

```cpp
// ❌ 频繁分配
for (int i = 0; i < 1000; i++) {
    std::vector<int> temp(100);
    process(temp);
}

// ✅ 复用内存
std::vector<int> buffer(100);
for (int i = 0; i < 1000; i++) {
    buffer.clear();
    process(buffer);
}
```

### 5.2 内存池？

```cpp
class MemoryPool {
    std::vector<char> pool;
    size_t offset = 0;

public:
    MemoryPool(size_t size) : pool(size) {}

    void* allocate(size_t size) {
        if (offset + size > pool.size()) {
            throw std::bad_alloc();
        }
        void* ptr = &pool[offset];
        offset += size;
        return ptr;
    }

    void reset() {
        offset = 0;
    }
};
```

---

## 6. SIMD 优化

### 6.1 向量化计算？

```cpp
#include <immintrin.h>  // AVX/SSE

// 标量版本
void add_arrays(float* result, const float* a, const float* b, int n) {
    for (int i = 0; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

// SIMD 版本（AVX：一次处理 8 个 float）
void add_arrays_simd(float* result, const float* a, const float* b, int n) {
    int simd_count = n / 8;
    for (int i = 0; i < simd_count; i++) {
        __m256 va = _mm256_loadu_ps(&a[i * 8]);
        __m256 vb = _mm256_loadu_ps(&b[i * 8]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&result[i * 8], vr);
    }
    // 处理剩余元素
    for (int i = simd_count * 8; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
```

---

## 7. 性能测量

### 7.1 如何测量性能？

```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();

// 被测代码
function_to_benchmark();

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "Time: " << duration.count() << " us\n";
```

### 7.2 基准测试注意事项？

1. **多次运行取平均**
2. **预热缓存**
3. **避免编译器优化掉代码**：
```cpp
// 防止优化掉
volatile int result = function();
```

---

## 总结

### 性能优化原则

1. **测量优先**：先测量，再优化
2. **找到热点**：80/20 法则
3. **缓存友好**：顺序访问，避免随机访问
4. **减少分支**：可预测或无分支
5. **减少分配**：复用内存，使用内存池
6. **帮助编译器**：const、restrict、inline

### 关键记忆点

- L1 缓存访问 ~1 ns，RAM 访问 ~100 ns
- 缓存行 64 字节，注意对齐
- 分支预测失败 ~10-20 周期
- 内联适合小函数、热路径
- SIMD 可提升 4-8 倍性能
