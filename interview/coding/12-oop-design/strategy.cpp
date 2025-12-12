/**
 * @file strategy.cpp
 * @brief 策略模式实现 - 面试题
 *
 * 定义一系列算法，把它们封装起来，并且使它们可以互相替换
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

/**
 * 题目1：实现经典策略模式
 *
 * 场景：支付系统，支持多种支付方式
 */

// TODO: 定义策略接口
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;
    virtual std::string getName() const = 0;
};

// TODO: 实现具体策略
class CreditCardPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
    }
    std::string getName() const override { return "CreditCard"; }
};

class PayPalPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
    }
    std::string getName() const override { return "PayPal"; }
};

class WeChatPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
    }
    std::string getName() const override { return "WeChat"; }
};

// TODO: 实现上下文
class PaymentContext {
public:
    void setStrategy(std::unique_ptr<PaymentStrategy> strategy) {
    }

    void checkout(double amount) {
    }

private:
    std::unique_ptr<PaymentStrategy> strategy_;
};

/**
 * 题目2：使用函数对象实现策略（现代 C++ 风格）
 *
 * 场景：排序策略
 */

// TODO: 使用 std::function 实现策略模式
template <typename T>
class Sorter {
public:
    using CompareStrategy = std::function<bool(const T&, const T&)>;

    void setStrategy(CompareStrategy strategy) {
    }

    void sort(std::vector<T>& data) {
    }

private:
    CompareStrategy compare_;
};

/**
 * 题目3：使用模板实现策略（编译期多态）
 *
 * 场景：日志格式化策略
 */

// TODO: 定义格式化策略
struct PlainTextFormatter {
    static std::string format(const std::string& message) {
        return message;
    }
};

struct JsonFormatter {
    static std::string format(const std::string& message) {
        return "";  // TODO: 返回 JSON 格式
    }
};

// TODO: 使用策略模板参数
template <typename Formatter = PlainTextFormatter>
class Logger {
public:
    void log(const std::string& message) {
    }
};

/**
 * 题目4：策略模式与工厂模式结合
 *
 * 场景：根据配置动态选择压缩策略
 */
class CompressionStrategy {
public:
    virtual ~CompressionStrategy() = default;
    virtual std::vector<char> compress(const std::vector<char>& data) = 0;
    virtual std::vector<char> decompress(const std::vector<char>& data) = 0;
};

// TODO: 实现具体压缩策略（ZIP、GZIP、LZ4）

// TODO: 实现策略工厂
class CompressionFactory {
public:
    static std::unique_ptr<CompressionStrategy> create(const std::string& type) {
        return nullptr;
    }
};

int main() {
    return 0;
}
