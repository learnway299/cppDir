/**
 * @file rate_limiter.h
 * @brief 限流器实现 - 面试题
 *
 * 题目：实现四种常见的限流算法
 *
 * 1. 固定窗口限流 (Fixed Window)
 *    - 在固定时间窗口内限制请求数量
 *    - 优点：实现简单
 *    - 缺点：窗口边界可能出现突发流量
 *
 * 2. 滑动窗口限流 (Sliding Window)
 *    - 动态滑动的时间窗口，记录每个请求的时间戳
 *    - 优点：更平滑，避免窗口边界问题
 *    - 缺点：需要存储所有时间戳，内存开销大
 *
 * 3. 令牌桶限流 (Token Bucket)
 *    - 固定速率产生令牌，请求消耗令牌
 *    - 优点：允许一定程度的突发流量
 *    - 缺点：需要定期补充令牌
 *
 * 4. 漏桶限流 (Leaky Bucket)
 *    - 请求进入桶中，固定速率流出
 *    - 优点：流量整形，输出速率恒定
 *    - 缺点：不支持突发流量
 *
 * 时间复杂度：
 * - FixedWindow: O(1)
 * - SlidingWindow: O(N)，N 为窗口内的请求数
 * - TokenBucket: O(1)
 * - LeakyBucket: O(1)
 */

#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <chrono>
#include <mutex>
#include <queue>

namespace RateLimiterImpl {

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

// 固定窗口限流
class FixedWindowRateLimiter {
public:
    FixedWindowRateLimiter(int limit, int windowMs);
    bool allow();

private:
    int limit_;
    int windowMs_;
    int count_;
    TimePoint windowStart_;
    std::mutex mutex_;
};

// 滑动窗口限流
class SlidingWindowRateLimiter {
public:
    SlidingWindowRateLimiter(int limit, int windowMs);
    bool allow();

private:
    int limit_;
    int windowMs_;
    std::queue<TimePoint> timestamps_;
    std::mutex mutex_;
};

// 令牌桶限流
class TokenBucketRateLimiter {
public:
    TokenBucketRateLimiter(int capacity, double refillPerSec);
    bool allow(int tokens = 1);

private:
    void refill();

    int capacity_;
    double refillRate_;
    double tokens_;
    TimePoint lastRefill_;
    std::mutex mutex_;
};

// 漏桶限流
class LeakyBucketRateLimiter {
public:
    LeakyBucketRateLimiter(int capacity, double leakPerSec);
    bool allow();

private:
    void leak();

    int capacity_;
    double leakRate_;
    double water_;
    TimePoint lastLeak_;
    std::mutex mutex_;
};

void runTests();

} // namespace RateLimiterImpl

#endif // RATE_LIMITER_H
