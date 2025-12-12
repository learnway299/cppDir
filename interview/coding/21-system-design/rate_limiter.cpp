/**
 * @file rate_limiter.cpp
 * @brief 限流器实现 - 面试题
 */
#include <chrono>
#include <mutex>
#include <queue>

// 题目1: 固定窗口限流
class FixedWindowRateLimiter {
public:
    FixedWindowRateLimiter(int limit, int windowSizeMs) {}
    bool allow() { return false; }
};

// 题目2: 滑动窗口限流
class SlidingWindowRateLimiter {
public:
    SlidingWindowRateLimiter(int limit, int windowSizeMs) {}
    bool allow() { return false; }
};

// 题目3: 令牌桶限流
class TokenBucketRateLimiter {
public:
    TokenBucketRateLimiter(int capacity, int refillRate) {}
    bool allow() { return false; }
};

// 题目4: 漏桶限流
class LeakyBucketRateLimiter {
public:
    LeakyBucketRateLimiter(int capacity, int leakRate) {}
    bool allow() { return false; }
};

int main() { return 0; }
