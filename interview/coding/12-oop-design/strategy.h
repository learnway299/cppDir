/**
 * @file strategy.h
 * @brief 策略模式实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 经典策略模式（支付系统）
 * 2. 函数对象策略（排序）
 * 3. 模板策略（日志格式化）
 * 4. 策略与工厂结合（压缩）
 */

#ifndef STRATEGY_H
#define STRATEGY_H

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace StrategyImpl {

// ==================== 题目1: 经典策略模式 ====================

/**
 * 题目1: 实现经典策略模式
 *
 * 场景：支付系统，支持多种支付方式
 * 运行时多态，通过虚函数实现
 *
 * 示例：
 *   PaymentContext context;
 *   context.setStrategy(std::make_unique<CreditCardPayment>("1234567890"));
 *   context.checkout(99.99);
 */
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual double pay(double amount) = 0;
    virtual std::string getName() const = 0;
};

class CreditCardPayment : public PaymentStrategy {
public:
    explicit CreditCardPayment(const std::string& cardNumber);
    double pay(double amount) override;
    std::string getName() const override;
    double getPaidAmount() const;

private:
    std::string cardNumber_;
    double paidAmount_ = 0;
};

class PayPalPayment : public PaymentStrategy {
public:
    explicit PayPalPayment(const std::string& email);
    double pay(double amount) override;
    std::string getName() const override;
    double getPaidAmount() const;

private:
    std::string email_;
    double paidAmount_ = 0;
};

class WeChatPayment : public PaymentStrategy {
public:
    double pay(double amount) override;
    std::string getName() const override;
    double getPaidAmount() const;

private:
    double paidAmount_ = 0;
};

class PaymentContext {
public:
    void setStrategy(std::unique_ptr<PaymentStrategy> strategy);
    double checkout(double amount);
    std::string getStrategyName() const;

private:
    std::unique_ptr<PaymentStrategy> strategy_;
};

// ==================== 题目2: 函数对象策略 ====================

/**
 * 题目2: 使用函数对象实现策略（现代 C++ 风格）
 *
 * 场景：排序策略，支持 lambda 表达式
 *
 * 示例：
 *   Sorter<int> sorter;
 *   sorter.setStrategy([](int a, int b) { return a > b; });  // 降序
 *   sorter.sort(data);
 */
template <typename T>
class Sorter {
public:
    using CompareStrategy = std::function<bool(const T&, const T&)>;

    Sorter();
    void setStrategy(CompareStrategy strategy);
    void sort(std::vector<T>& data);

private:
    CompareStrategy compare_;
};

template <typename T>
struct SortStrategies {
    static bool ascending(const T& a, const T& b);
    static bool descending(const T& a, const T& b);
};

// ==================== 题目3: 模板策略 ====================

/**
 * 题目3: 使用模板实现策略（编译期多态）
 *
 * 场景：日志格式化策略
 * 编译期确定策略，零运行时开销
 *
 * 示例：
 *   Logger<JsonFormatter> logger;
 *   logger.info("message");  // 输出 JSON 格式
 */
struct PlainTextFormatter {
    static std::string format(const std::string& level, const std::string& message);
};

struct JsonFormatter {
    static std::string format(const std::string& level, const std::string& message);
};

struct XmlFormatter {
    static std::string format(const std::string& level, const std::string& message);
};

template <typename Formatter = PlainTextFormatter>
class Logger {
public:
    std::string info(const std::string& message);
    std::string warn(const std::string& message);
    std::string error(const std::string& message);
};

// ==================== 题目4: 策略与工厂结合 ====================

/**
 * 题目4: 策略模式与工厂模式结合
 *
 * 场景：根据配置动态选择压缩策略
 *
 * 示例：
 *   Compressor compressor;
 *   compressor.setStrategy("rle");  // 使用工厂创建策略
 *   auto compressed = compressor.compress(data);
 */
class CompressionStrategy {
public:
    virtual ~CompressionStrategy() = default;
    virtual std::vector<char> compress(const std::vector<char>& data) = 0;
    virtual std::vector<char> decompress(const std::vector<char>& data) = 0;
    virtual std::string getName() const = 0;
};

class NoCompression : public CompressionStrategy {
public:
    std::vector<char> compress(const std::vector<char>& data) override;
    std::vector<char> decompress(const std::vector<char>& data) override;
    std::string getName() const override;
};

class RunLengthEncoding : public CompressionStrategy {
public:
    std::vector<char> compress(const std::vector<char>& data) override;
    std::vector<char> decompress(const std::vector<char>& data) override;
    std::string getName() const override;
};

class CompressionFactory {
public:
    static std::unique_ptr<CompressionStrategy> create(const std::string& type);
};

class Compressor {
public:
    void setStrategy(std::unique_ptr<CompressionStrategy> strategy);
    void setStrategy(const std::string& type);
    std::vector<char> compress(const std::vector<char>& data);
    std::vector<char> decompress(const std::vector<char>& data);
    std::string getStrategyName() const;

private:
    std::unique_ptr<CompressionStrategy> strategy_;
};

// ==================== 测试函数 ====================

void runTests();

} // namespace StrategyImpl

#endif // STRATEGY_H
