# 21 - 位运算与底层操作 (Bit Operations)

## 简介

位运算是计算机底层操作的基础，在性能优化、嵌入式开发、算法实现中广泛应用。

**核心概念**：
- **基本位运算**：与、或、异或、取反、移位
- **位操作技巧**：设置、清除、切换、检查位
- **应用场景**：权限管理、标志位、性能优化
- **高级技巧**：位掩码、位域、���分支编程

---

## 1. 基本位运算

### 1.1 六大位运算符？

| 运算符 | 名称 | 示例 | 结果 |
|--------|------|------|------|
| `&` | 按位与 | `0b1100 & 0b1010` | `0b1000` |
| `\|` | 按位或 | `0b1100 \| 0b1010` | `0b1110` |
| `^` | 按位异或 | `0b1100 ^ 0b1010` | `0b0110` |
| `~` | 按位取反 | `~0b1100` | `0b0011` (假设 4 位) |
| `<<` | 左移 | `0b0011 << 2` | `0b1100` |
| `>>` | 右移 | `0b1100 >> 2` | `0b0011` |

**真值表**：

| A | B | A & B | A \| B | A ^ B |
|---|---|-------|--------|-------|
| 0 | 0 | 0     | 0      | 0     |
| 0 | 1 | 0     | 1      | 1     |
| 1 | 0 | 0     | 1      | 1     |
| 1 | 1 | 1     | 1      | 0     |

### 1.2 移位运算？

**左移（`<<`）**：
```cpp
unsigned int x = 1;
unsigned int y = x << 3;  // y = 8 (1 * 2^3)

// 左移 n 位 = 乘以 2^n
int a = 5 << 2;  // 5 * 4 = 20
```

**右移（`>>`）**：
```cpp
// 无符号数：逻辑右移（高位补 0）
unsigned int x = 8;
unsigned int y = x >> 2;  // y = 2 (8 / 4)

// 有符号数：算术右移（高位补符号位，实现定义）
int a = -8;
int b = a >> 2;  // 实现定义：可能是 -2
```

**注意事项**：
```cpp
// ❌ 未定义行为
int x = 1 << 32;   // 位移量 >= 类型位数
int y = 1 << -1;   // 负位移量
int z = -1 << 1;   // 左移负数（C++20 前 UB）
```

---

## 2. 常用位操作技巧

### 2.1 设置、清除、切换位？

**设置第 n 位为 1**：
```cpp
// 设置第 n 位
x |= (1 << n);

// 示例：设置第 3 位
int x = 0b1010;  // 10
x |= (1 << 3);   // 0b1010 | 0b1000 = 0b1010 = 18
```

**清除第 n 位为 0**：
```cpp
// 清除第 n 位
x &= ~(1 << n);

// 示例：清除第 1 位
int x = 0b1010;  // 10
x &= ~(1 << 1);  // 0b1010 & 0b1101 = 0b1000 = 8
```

**切换第 n 位**：
```cpp
// 切换第 n 位（0 变 1，1 变 0）
x ^= (1 << n);

// 示例：切换第 2 位
int x = 0b1010;  // 10
x ^= (1 << 2);   // 0b1010 ^ 0b0100 = 0b1110 = 14
```

**检查第 n 位是否为 1**：
```cpp
// 检查第 n 位
bool is_set = (x & (1 << n)) != 0;

// 示例
int x = 0b1010;  // 10
bool bit1 = (x & (1 << 1)) != 0;  // true
bool bit0 = (x & (1 << 0)) != 0;  // false
```

### 2.2 位掩码（Bitmask）？

```cpp
// 权限系统
enum Permission {
    READ    = 1 << 0,  // 0b0001
    WRITE   = 1 << 1,  // 0b0010
    EXECUTE = 1 << 2,  // 0b0100
    DELETE  = 1 << 3   // 0b1000
};

// 设置权限
int user_perm = 0;
user_perm |= READ;          // 添加读权限
user_perm |= WRITE;         // 添加写权限

// 检查权限
if (user_perm & READ) {
    std::cout << "有读权限\n";
}

// 移除权限
user_perm &= ~WRITE;        // 移除写权限

// 切换权限
user_perm ^= EXECUTE;       // 切换执行权限

// 检查多个权限
if ((user_perm & (READ | WRITE)) == (READ | WRITE)) {
    std::cout << "同时有读写权限\n";
}
```

### 2.3 常见位操作技巧？

**1. 判断奇偶**：
```cpp
bool is_odd(int x) {
    return x & 1;  // 最低位为 1 则为奇数
}
```

**2. 交换两个数（不用临时变量）**：
```cpp
void swap(int& a, int& b) {
    a ^= b;
    b ^= a;
    a ^= b;
}
```

**3. 判断是否为 2 的幂**：
```cpp
bool is_power_of_two(unsigned int x) {
    return x != 0 && (x & (x - 1)) == 0;
}

// 原理：
// 8  = 0b1000
// 7  = 0b0111
// 8 & 7 = 0
```

**4. 统计二进制中 1 的个数**：
```cpp
int count_bits(unsigned int x) {
    int count = 0;
    while (x) {
        count++;
        x &= (x - 1);  // 清除最低位的 1
    }
    return count;
}

// 或使用内建函数
int count = __builtin_popcount(x);  // GCC/Clang
```

**5. 获取最低位的 1**：
```cpp
unsigned int lowest_bit(unsigned int x) {
    return x & (-x);
}

// 原理：
// x     = 0b01010000
// -x    = 0b10110000（补码）
// x & -x = 0b00010000（最低位的 1）
```

**6. 清除最低位的 1**：
```cpp
unsigned int clear_lowest_bit(unsigned int x) {
    return x & (x - 1);
}
```

**7. 翻转所有位**：
```cpp
unsigned int flip_bits(unsigned int x) {
    return ~x;
}
```

---

## 3. 位域（Bit Fields）

### 3.1 位域的使用？

```cpp
struct Flags {
    unsigned int is_active : 1;   // 1 位
    unsigned int is_visible : 1;  // 1 位
    unsigned int priority : 3;    // 3 位（0-7）
    unsigned int unused : 27;     // 27 位（填充）
};

Flags f;
f.is_active = 1;
f.is_visible = 0;
f.priority = 5;

// 节省内存：整个结构体只占 4 字节
```

**注意事项**：
- 位域的内存布局是实现定义的
- 无法获取位域的地址（`&f.is_active` 非法）
- 不适合跨平台的数据序列化

### 3.2 联合体与位操作？

```cpp
union Color {
    uint32_t value;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba;
};

Color c;
c.value = 0xFF00FF80;  // RGBA: 255, 0, 255, 128

// 访问单个通道
uint8_t red = c.rgba.r;
uint8_t alpha = c.rgba.a;

// 或使用位操作
uint8_t red_alt = (c.value >> 24) & 0xFF;
uint8_t alpha_alt = c.value & 0xFF;
```

---

## 4. 高级位操作

### 4.1 位操作实现算术？

**1. 乘以 2 的幂**：
```cpp
int multiply_by_8(int x) {
    return x << 3;  // x * 8
}
```

**2. 除以 2 的幂**：
```cpp
int divide_by_4(int x) {
    return x >> 2;  // x / 4（向下取整）
}
```

**3. 模运算（2 的幂）**：
```cpp
int mod_16(int x) {
    return x & 15;  // x % 16
}

// 原理：16 = 0b10000, 15 = 0b01111
```

**4. 绝对值（无分支）**：
```cpp
int abs_value(int x) {
    int mask = x >> 31;  // 全 0 或全 1
    return (x + mask) ^ mask;
}
```

**5. 最大值（无分支）**：
```cpp
int max_branchless(int a, int b) {
    int diff = a - b;
    int mask = diff >> 31;  // a < b 时为 -1，否则为 0
    return a - (diff & mask);
}
```

### 4.2 位操作实现常用函数？

**1. 反转位（Reverse Bits）**：
```cpp
uint32_t reverse_bits(uint32_t x) {
    x = ((x & 0xAAAAAAAA) >> 1) | ((x & 0x55555555) << 1);
    x = ((x & 0xCCCCCCCC) >> 2) | ((x & 0x33333333) << 2);
    x = ((x & 0xF0F0F0F0) >> 4) | ((x & 0x0F0F0F0F) << 4);
    x = ((x & 0xFF00FF00) >> 8) | ((x & 0x00FF00FF) << 8);
    x = (x >> 16) | (x << 16);
    return x;
}
```

**2. 计算对数（log2）**：
```cpp
int log2_floor(unsigned int x) {
    int result = 0;
    while (x >>= 1) {
        result++;
    }
    return result;
}

// 或使用内建函数
int log2_builtin(unsigned int x) {
    return 31 - __builtin_clz(x);  // clz: count leading zeros
}
```

**3. 下一个 2 的幂**：
```cpp
uint32_t next_power_of_two(uint32_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}
```

### 4.3 SWAR（SIMD Within A Register）？

```cpp
// 并行计算 4 个字节的和
uint32_t sum_bytes(uint32_t x) {
    // x = [A][B][C][D] (4 个字节)
    x = (x & 0xFF00FF00) >> 8 | (x & 0x00FF00FF);  // [A+B][C+D]
    x = (x & 0xFFFF0000) >> 16 | (x & 0x0000FFFF);  // [A+B+C+D]
    return x;
}
```

---

## 5. 实际应用

### 5.1 位图（Bitmap）？

```cpp
class Bitmap {
    std::vector<uint64_t> bits;
    size_t size;

public:
    Bitmap(size_t n) : bits((n + 63) / 64), size(n) {}

    void set(size_t pos) {
        bits[pos / 64] |= (1ULL << (pos % 64));
    }

    void clear(size_t pos) {
        bits[pos / 64] &= ~(1ULL << (pos % 64));
    }

    bool test(size_t pos) const {
        return (bits[pos / 64] & (1ULL << (pos % 64))) != 0;
    }

    void toggle(size_t pos) {
        bits[pos / 64] ^= (1ULL << (pos % 64));
    }
};
```

### 5.2 布隆过滤器（Bloom Filter）？

```cpp
class BloomFilter {
    std::vector<bool> bits;
    size_t num_hashes;

    size_t hash(const std::string& key, size_t seed) const {
        size_t h = seed;
        for (char c : key) {
            h = h * 31 + c;
        }
        return h % bits.size();
    }

public:
    BloomFilter(size_t size, size_t hashes)
        : bits(size), num_hashes(hashes) {}

    void add(const std::string& key) {
        for (size_t i = 0; i < num_hashes; i++) {
            bits[hash(key, i)] = true;
        }
    }

    bool might_contain(const std::string& key) const {
        for (size_t i = 0; i < num_hashes; i++) {
            if (!bits[hash(key, i)]) {
                return false;
            }
        }
        return true;
    }
};
```

### 5.3 压缩坐标（Morton Code / Z-Order Curve）？

```cpp
// 交错两个 16 位数的位
uint32_t morton_encode(uint16_t x, uint16_t y) {
    auto spread = [](uint16_t v) -> uint32_t {
        uint32_t x = v;
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;
        return x;
    };

    return spread(x) | (spread(y) << 1);
}
```

---

## 6. 编译器内建函数

### 6.1 GCC/Clang 内建函数？

```cpp
// 计算前导零
int clz = __builtin_clz(x);  // count leading zeros

// 计算尾随零
int ctz = __builtin_ctz(x);  // count trailing zeros

// 计算 1 的个数
int popcount = __builtin_popcount(x);

// 奇偶校验
int parity = __builtin_parity(x);  // 1 的个数是否为奇数
```

### 6.2 MSVC 内建函数？

```cpp
#include <intrin.h>

// 位扫描
unsigned long index;
_BitScanForward(&index, x);  // 找到最低位 1
_BitScanReverse(&index, x);  // 找到最高位 1

// 计数
__popcnt(x);  // 统计 1 的个数
```

---

## 7. 性能考虑

### 7.1 位操作 vs 算术操作？

```cpp
// ✅ 快：位操作
bool is_even = !(x & 1);

// ❌ 慢：模运算
bool is_even = (x % 2) == 0;

// ✅ 快：位移
int multiply_by_8 = x << 3;

// ⚠️ 现代编译器会优化成位移
int multiply_by_8 = x * 8;
```

### 7.2 分支 vs 位操作？

```cpp
// ❌ 有分支（可能导致分支预测失败）
int abs_branching(int x) {
    if (x < 0) return -x;
    else return x;
}

// ✅ 无分支（但可能不如编译器优化的分支版本）
int abs_branchless(int x) {
    int mask = x >> 31;
    return (x + mask) ^ mask;
}

// ✅ 使用标准库（编译器会优化）
int abs_std(int x) {
    return std::abs(x);
}
```

---

## 总结

### 常用位操作速查表

| 操作 | 代码 | 说明 |
|------|------|------|
| 设置第 n 位 | `x \|= (1 << n)` | 将第 n 位置为 1 |
| 清除第 n 位 | `x &= ~(1 << n)` | 将第 n 位置为 0 |
| 切换第 n 位 | `x ^= (1 << n)` | 翻转第 n 位 |
| 检查第 n 位 | `(x & (1 << n)) != 0` | 检查第 n 位是否为 1 |
| 清除最低位 1 | `x & (x - 1)` | 将最低位的 1 清零 |
| 获取最低位 1 | `x & (-x)` | 保留最低位的 1 |
| 是否为 2 的幂 | `x != 0 && (x & (x - 1)) == 0` | 判断是否只有一个 1 |
| 统计 1 的个数 | `__builtin_popcount(x)` | GCC/Clang 内建 |

### 关键记忆点

1. **基本运算**：`&`, `|`, `^`, `~`, `<<`, `>>`
2. **设置位**：`|= (1 << n)`，清除位：`&= ~(1 << n)`
3. **x & (x - 1)**：清除最低位的 1
4. **x & -x**：获取最低位的 1
5. **无分支编程**：用位运算替代分支
6. **编译器内建函数**：`__builtin_popcount`, `__builtin_clz`
7. **权限系统**：使用位掩码管理标志位

---

## 参考资源

- [Bit Twiddling Hacks](https://graphics.stanford.edu/~seander/bithacks.html)
- [Hacker's Delight](https://www.hackersdelight.org/)
- [GCC Built-in Functions](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
