/**
 * @file rate_limiter_solution.cpp
 * @brief 限流器实现 - 参考解答
 */

#include "rate_limiter.h"
#include <iostream>
#include <thread>
#include <algorithm>

namespace RateLimiterImpl {
namespace Solution {

// 固定窗口限流
class FixedWindowRateLimiter {
public:
    FixedWindowRateLimiter(int limit, int windowMs)
        : limit_(limit), windowMs_(windowMs), count_(0) {
        windowStart_ = Clock::now();
    }

    bool allow() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = Clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - windowStart_).count();

        if (elapsed >= windowMs_) {
            windowStart_ = now;
            count_ = 0;
        }

        if (count_ < limit_) {
            ++count_;
            return true;
        }
        return false;
    }

private:
    int limit_, windowMs_, count_;
    TimePoint windowStart_;
    std::mutex mutex_;
};

// 滑动窗口限流
class SlidingWindowRateLimiter {
public:
    SlidingWindowRateLimiter(int limit, int windowMs)
        : limit_(limit), windowMs_(windowMs) {}

    bool allow() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = Clock::now();

        while (!timestamps_.empty()) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - timestamps_.front()).count();
            if (elapsed >= windowMs_) {
                timestamps_.pop();
            } else {
                break;
            }
        }

        if (static_cast<int>(timestamps_.size()) < limit_) {
            timestamps_.push(now);
            return true;
        }
        return false;
    }

private:
    int limit_, windowMs_;
    std::queue<TimePoint> timestamps_;
    std::mutex mutex_;
};

// 令牌桶限流
class TokenBucketRateLimiter {
public:
    TokenBucketRateLimiter(int capacity, double refillPerSec)
        : capacity_(capacity), refillRate_(refillPerSec), tokens_(capacity) {
        lastRefill_ = Clock::now();
    }

    bool allow(int tokens = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        refill();

        if (tokens_ >= tokens) {
            tokens_ -= tokens;
            return true;
        }
        return false;
    }

private:
    void refill() {
        auto now = Clock::now();
        double elapsed = std::chrono::duration<double>(now - lastRefill_).count();
        tokens_ = std::min(static_cast<double>(capacity_), tokens_ + elapsed * refillRate_);
        lastRefill_ = now;
    }

    int capacity_;
    double refillRate_, tokens_;
    TimePoint lastRefill_;
    std::mutex mutex_;
};

// 漏桶限流
class LeakyBucketRateLimiter {
public:
    LeakyBucketRateLimiter(int capacity, double leakPerSec)
        : capacity_(capacity), leakRate_(leakPerSec), water_(0) {
        lastLeak_ = Clock::now();
    }

    bool allow() {
        std::lock_guard<std::mutex> lock(mutex_);
        leak();

        if (water_ < capacity_) {
            ++water_;
            return true;
        }
        return false;
    }

private:
    void leak() {
        auto now = Clock::now();
        double elapsed = std::chrono::duration<double>(now - lastLeak_).count();
        water_ = std::max(0.0, water_ - elapsed * leakRate_);
        lastLeak_ = now;
    }

    int capacity_;
    double leakRate_, water_;
    TimePoint lastLeak_;
    std::mutex mutex_;
};

void runSolutionTests() {
    std::cout << "=== Rate Limiter Solution ===" << std::endl;

    // 测试固定窗口限流
    std::cout << "\n--- Fixed Window (5 req/1000ms) ---" << std::endl;
    FixedWindowRateLimiter fw(5, 1000);
    for (int i = 0; i < 8; ++i) {
        std::cout << "Request " << i << ": " << (fw.allow() ? "OK" : "DENIED") << std::endl;
    }

    // 测试滑动窗口限流
    std::cout << "\n--- Sliding Window (5 req/1000ms) ---" << std::endl;
    SlidingWindowRateLimiter sw(5, 1000);
    for (int i = 0; i < 8; ++i) {
        std::cout << "Request " << i << ": " << (sw.allow() ? "OK" : "DENIED") << std::endl;
    }

    // 测试令牌桶限流
    std::cout << "\n--- Token Bucket (capacity=5, refill=2/s) ---" << std::endl;
    TokenBucketRateLimiter tb(5, 2.0);
    for (int i = 0; i < 8; ++i) {
        std::cout << "Request " << i << ": " << (tb.allow() ? "OK" : "DENIED") << std::endl;
    }
    std::cout << "Waiting 1s for refill..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (int i = 0; i < 3; ++i) {
        std::cout << "Request: " << (tb.allow() ? "OK" : "DENIED") << std::endl;
    }

    // 测试漏桶限流
    std::cout << "\n--- Leaky Bucket (capacity=5, leak=2/s) ---" << std::endl;
    LeakyBucketRateLimiter lb(5, 2.0);
    for (int i = 0; i < 8; ++i) {
        std::cout << "Request " << i << ": " << (lb.allow() ? "OK" : "DENIED") << std::endl;
    }

    std::cout << "\nAll rate limiter tests completed!" << std::endl;
}

} // namespace Solution

void runTests() {
    Solution::runSolutionTests();
}

} // namespace RateLimiterImpl
