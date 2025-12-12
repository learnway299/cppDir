# 24 - 时间与日期 (Time and Date)

## 简介

时间处理是软件开发中的常见需求。C++ 从 C 继承了传统的时间 API，C++11/20 引入了现代的 `<chrono>` 库。

**核心概念**：
- **时间点（Time Point）**：特定的时刻
- **时间段（Duration）**：时间间隔
- **时钟（Clock）**：获取当前时间的机制
- **时区（Time Zone）**：地理位置的时间偏移

---

## 1. C 风格时间 API

### 1.1 time_t 与 time()？

```cpp
#include <ctime>

// 获取当前时间（Unix 时间戳：秒数）
time_t now = time(nullptr);  // 从 1970-01-01 00:00:00 UTC 开始

std::cout << "时间戳: " << now << "\n";  // 例如：1700000000
```

### 1.2 tm 结构体？

```cpp
struct tm {
    int tm_sec;   // 秒（0-60，60 for leap second）
    int tm_min;   // 分（0-59）
    int tm_hour;  // 时（0-23）
    int tm_mday;  // 日（1-31）
    int tm_mon;   // 月（0-11）
    int tm_year;  // 年（从 1900 年开始）
    int tm_wday;  // 星期几（0-6，0=Sunday）
    int tm_yday;  // 一年中的第几天（0-365）
    int tm_isdst; // 夏令时标志
};
```

**time_t ↔ tm 转换**：
```cpp
// time_t → tm（本地时间）
time_t now = time(nullptr);
tm* local = localtime(&now);  // ⚠️ 非线程安全

std::cout << "年: " << (local->tm_year + 1900) << "\n";
std::cout << "月: " << (local->tm_mon + 1) << "\n";
std::cout << "日: " << local->tm_mday << "\n";

// time_t → tm（UTC 时间）
tm* utc = gmtime(&now);  // ⚠️ 非线程安全

// tm → time_t
time_t timestamp = mktime(local);
```

### 1.3 格式化输出？

```cpp
time_t now = time(nullptr);
tm* local = localtime(&now);

// strftime：格式化时间
char buffer[100];
strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
std::cout << buffer << "\n";  // 2024-11-15 10:30:45

// 常用格式
strftime(buffer, sizeof(buffer), "%Y/%m/%d", local);  // 2024/11/15
strftime(buffer, sizeof(buffer), "%H:%M", local);     // 10:30
strftime(buffer, sizeof(buffer), "%A", local);        // Wednesday
strftime(buffer, sizeof(buffer), "%B %d, %Y", local); // November 15, 2024
```

**格式符速查**：

| 格式符 | 说明 | 示例 |
|--------|------|------|
| `%Y` | 四位年份 | 2024 |
| `%m` | 月（01-12） | 11 |
| `%d` | 日（01-31） | 15 |
| `%H` | 时（00-23） | 14 |
| `%M` | 分（00-59） | 30 |
| `%S` | 秒（00-60） | 45 |
| `%A` | 星期全称 | Wednesday |
| `%a` | 星期缩写 | Wed |
| `%B` | 月份全称 | November |
| `%b` | 月份缩写 | Nov |

### 1.4 线程安全版本？

```cpp
// POSIX 线程安全版本
time_t now = time(nullptr);
tm local;
localtime_r(&now, &local);  // Linux

// Windows 线程安全版本
localtime_s(&local, &now);  // MSVC

// 跨平台封装
tm safe_localtime(time_t t) {
    tm result;
#ifdef _WIN32
    localtime_s(&result, &t);
#else
    localtime_r(&t, &result);
#endif
    return result;
}
```

---

## 2. std::chrono（C++11）

### 2.1 时间段（Duration）？

```cpp
#include <chrono>
using namespace std::chrono;

// 时间段类型
seconds s(10);           // 10 秒
milliseconds ms(500);    // 500 毫秒
microseconds us(1000);   // 1000 微秒
nanoseconds ns(1000000); // 1000000 纳秒

// 转换
auto s_to_ms = duration_cast<milliseconds>(s);  // 10000 ms
auto ms_to_s = duration_cast<seconds>(ms);      // 0 秒（向下取整）

// 算术运算
auto total = s + ms;  // 10500 ms（自动转换为更小单位）

// 获取数值
std::cout << s.count() << " 秒\n";  // 10

// 字面量（C++14）
using namespace std::chrono_literals;
auto d1 = 10s;    // 10 秒
auto d2 = 500ms;  // 500 毫秒
auto d3 = 1min;   // 1 分钟
auto d4 = 2h;     // 2 小时
```

### 2.2 时钟（Clock）？

**三种时钟**：

| 时钟 | 说明 | 用途 |
|------|------|------|
| `system_clock` | 系统时钟（可调整） | 获取当前日期时间 |
| `steady_clock` | 单调时钟（不可调整） | 测量时间间隔 |
| `high_resolution_clock` | 高精度时钟 | 性能测量 |

```cpp
// 系统时钟
auto now = system_clock::now();

// 单调时钟（不受系统时间调整影响）
auto start = steady_clock::now();
// ... 执行代码 ...
auto end = steady_clock::now();
auto elapsed = duration_cast<milliseconds>(end - start);
std::cout << "耗时: " << elapsed.count() << " ms\n";

// 高精度时钟
auto high_res = high_resolution_clock::now();
```

### 2.3 时间点（Time Point）？

```cpp
// 获取当前时间点
auto now = system_clock::now();

// 时间点 → time_t
time_t tt = system_clock::to_time_t(now);

// time_t → 时间点
auto tp = system_clock::from_time_t(tt);

// 时间点算术
auto tomorrow = now + 24h;  // C++14
auto yesterday = now - hours(24);

// 时间点比较
if (now < tomorrow) {
    std::cout << "现在早于明天\n";
}
```

### 2.4 性能计时？

```cpp
#include <chrono>
#include <iostream>

template<typename Func>
auto measure_time(Func&& func) {
    using namespace std::chrono;

    auto start = steady_clock::now();
    func();
    auto end = steady_clock::now();

    return duration_cast<microseconds>(end - start).count();
}

// 使用
auto time_us = measure_time([] {
    // 被测代码
    std::this_thread::sleep_for(100ms);
});

std::cout << "耗时: " << time_us << " µs\n";
```

---

## 3. C++20 日历与时区

### 3.1 日历（Calendar）？

```cpp
#include <chrono>

using namespace std::chrono;

// 年月日
auto today = year_month_day{year{2024}, month{11}, day{15}};

// 从 sys_days 构造
auto ymd = year_month_day{sys_days{system_clock::now()}};

std::cout << "年: " << (int)ymd.year() << "\n";
std::cout << "月: " << (unsigned)ymd.month() << "\n";
std::cout << "日: " << (unsigned)ymd.day() << "\n";

// 星期几
auto wd = weekday{sys_days{system_clock::now()}};
std::cout << "星期: " << wd << "\n";  // Mon, Tue, ...

// 日期运算
auto next_week = sys_days{ymd} + days{7};
auto next_month = ymd + months{1};

// 检查有效性
if (!ymd.ok()) {
    std::cout << "无效日期\n";
}
```

### 3.2 时区（Time Zone，C++20）？

```cpp
#include <chrono>

using namespace std::chrono;

// 获取时区
auto tz = locate_zone("America/New_York");

// 当前时区的时间
auto local_time = zoned_time{current_zone(), system_clock::now()};
std::cout << local_time << "\n";

// 指定时区的时间
auto ny_time = zoned_time{tz, system_clock::now()};
std::cout << "纽约时间: " << ny_time << "\n";

// 时区转换
auto utc_time = system_clock::now();
auto tokyo_time = zoned_time{"Asia/Tokyo", utc_time};
std::cout << "东京时间: " << tokyo_time << "\n";
```

---

## 4. 实用函数

### 4.1 获取当前日期时间字符串？

```cpp
// 方法 1：C 风格
std::string current_datetime() {
    time_t now = time(nullptr);
    tm local = safe_localtime(now);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local);
    return buffer;
}

// 方法 2：C++20
std::string current_datetime_cpp20() {
    auto now = std::chrono::system_clock::now();
    return std::format("{:%Y-%m-%d %H:%M:%S}", now);
}
```

### 4.2 计算时间差？

```cpp
// 两个时间点的差值
auto start = steady_clock::now();
// ... 执行代码 ...
auto end = steady_clock::now();

auto diff_ms = duration_cast<milliseconds>(end - start).count();
auto diff_us = duration_cast<microseconds>(end - start).count();

std::cout << "耗时: " << diff_ms << " ms (" << diff_us << " µs)\n";
```

### 4.3 睡眠指定时间？

```cpp
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

// 睡眠 1 秒
std::this_thread::sleep_for(1s);

// 睡眠 500 毫秒
std::this_thread::sleep_for(500ms);

// 睡眠到指定时间点
auto wakeup_time = system_clock::now() + 10s;
std::this_thread::sleep_until(wakeup_time);
```

### 4.4 解析日期字符串？

```cpp
// C 风格：strptime（POSIX）
#ifndef _WIN32
tm parse_date(const char* date_str, const char* format) {
    tm result = {};
    strptime(date_str, format, &result);
    return result;
}

// 使用
tm t = parse_date("2024-11-15", "%Y-%m-%d");
time_t timestamp = mktime(&t);
#endif

// C++20：std::chrono::parse
#include <sstream>

auto parse_datetime(const std::string& str) {
    std::istringstream ss{str};
    sys_seconds tp;
    ss >> parse("%Y-%m-%d %H:%M:%S", tp);
    return tp;
}
```

---

## 5. 常见应用场景

### 5.1 计算程序运行时间？

```cpp
class Timer {
    using Clock = std::chrono::steady_clock;
    Clock::time_point start;

public:
    Timer() : start(Clock::now()) {}

    void reset() {
        start = Clock::now();
    }

    double elapsed_seconds() const {
        auto end = Clock::now();
        auto duration = std::chrono::duration<double>(end - start);
        return duration.count();
    }

    long long elapsed_ms() const {
        auto end = Clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
};

// 使用
Timer timer;
// ... 执行代码 ...
std::cout << "耗时: " << timer.elapsed_ms() << " ms\n";
```

### 5.2 定时器（周期性任务）？

```cpp
void periodic_task() {
    using namespace std::chrono;

    auto next = steady_clock::now();

    while (true) {
        // 执行任务
        std::cout << "执行任务\n";

        // 计算下次执行时间
        next += seconds(5);

        // 睡眠到下次执行时间
        std::this_thread::sleep_until(next);
    }
}
```

### 5.3 超时检测？

```cpp
bool wait_with_timeout(std::function<bool()> condition, milliseconds timeout) {
    auto deadline = steady_clock::now() + timeout;

    while (steady_clock::now() < deadline) {
        if (condition()) {
            return true;  // 条件满足
        }
        std::this_thread::sleep_for(milliseconds(10));
    }

    return false;  // 超时
}

// 使用
bool success = wait_with_timeout(
    [] { return check_condition(); },
    3000ms
);
```

### 5.4 日期范围检查？

```cpp
bool is_in_range(const tm& date, const tm& start, const tm& end) {
    time_t t = mktime(const_cast<tm*>(&date));
    time_t t_start = mktime(const_cast<tm*>(&start));
    time_t t_end = mktime(const_cast<tm*>(&end));

    return t >= t_start && t <= t_end;
}
```

---

## 6. 性能考虑

### 6.1 获取时间的开销？

| 方法 | 精度 | 开销 |
|------|------|------|
| `time()` | 秒 | 很低 |
| `gettimeofday()` | 微秒 | 低 |
| `clock_gettime(CLOCK_REALTIME)` | 纳秒 | 低 |
| `clock_gettime(CLOCK_MONOTONIC)` | 纳秒 | 低 |
| `steady_clock::now()` | 纳秒 | 低 |
| `high_resolution_clock::now()` | 纳秒 | 中 |

**建议**：
- 计时用 `steady_clock`（不受系统时间调整影响）
- 获取日期时间用 `system_clock`
- 避免在循环中频繁获取时间

### 6.2 避免频繁获取时间？

```cpp
// ❌ 慢：每次迭代都获取时间
for (int i = 0; i < 1000000; i++) {
    auto now = steady_clock::now();
    // 使用 now...
}

// ✅ 快：批量检查
auto last_check = steady_clock::now();
for (int i = 0; i < 1000000; i++) {
    if (i % 1000 == 0) {  // 每 1000 次检查一次
        auto now = steady_clock::now();
        // ...
    }
}
```

---

## 7. 跨平台注意事项

### 7.1 time_t 的范围？

```cpp
// 32 位系统：time_t 可能是 32 位（2038 年问题）
// 64 位系统：time_t 通常是 64 位

#include <limits>

std::cout << "time_t 最大值: "
          << std::numeric_limits<time_t>::max() << "\n";

// 2038 年问题：32 位 time_t 在 2038-01-19 03:14:07 UTC 溢出
```

### 7.2 时区处理？

```cpp
// 设置时区（不推荐，会影响整个进程）
#ifndef _WIN32
    setenv("TZ", "America/New_York", 1);
    tzset();
#else
    _putenv_s("TZ", "EST5EDT");
    _tzset();
#endif

// 推荐：使用 C++20 的时区库
```

---

## 总结

### 时间 API 选择指南

| 需求 | 推荐方案 |
|------|----------|
| 获取当前日期时间 | `system_clock::now()` |
| 性能计时 | `steady_clock::now()` |
| 格式化输出 | `strftime` 或 C++20 `std::format` |
| 时间间隔 | `std::chrono::duration` |
| 睡眠 | `std::this_thread::sleep_for` |
| 日期计算 | C++20 `year_month_day` |
| 时区转换 | C++20 `zoned_time` |

### 关键记忆点

1. **C 风格**：`time()`、`localtime()`、`strftime()`
2. **C++11 chrono**：`duration`、`time_point`、`clock`
3. **三种时钟**：`system_clock`（日期时间）、`steady_clock`（计时）、`high_resolution_clock`（高精度）
4. **C++14 字面量**：`10s`、`500ms`、`1min`、`2h`
5. **C++20 日历**：`year_month_day`、`weekday`
6. **C++20 时区**：`zoned_time`、`locate_zone()`
7. **线程安全**：使用 `localtime_r` 或 `localtime_s`
8. **2038 问题**：32 位 `time_t` 溢出

---

## 参考资源

- [cppreference: chrono](https://en.cppreference.com/w/cpp/chrono)
- [C++20 Calendar and Time Zone](https://en.cppreference.com/w/cpp/chrono#Calendar)
- [Howard Hinnant's date library](https://github.com/HowardHinnant/date)（C++20 时区库的基础）
