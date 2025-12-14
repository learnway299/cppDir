/**
 * @file strategy_solution.cpp
 * @brief 策略模式实现 - 参考答案
 */

#include "strategy.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace StrategyImpl {

namespace Solution {

// ==================== 经典策略模式 ====================

class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual double pay(double amount) = 0;
    virtual std::string getName() const = 0;
};

class CreditCardPayment : public PaymentStrategy {
public:
    explicit CreditCardPayment(const std::string& cardNumber)
        : cardNumber_(cardNumber) {}

    double pay(double amount) override {
        paidAmount_ = amount;
        return amount;
    }

    std::string getName() const override { return "CreditCard"; }
    double getPaidAmount() const { return paidAmount_; }

private:
    std::string cardNumber_;
    double paidAmount_ = 0;
};

class PayPalPayment : public PaymentStrategy {
public:
    explicit PayPalPayment(const std::string& email) : email_(email) {}

    double pay(double amount) override {
        paidAmount_ = amount;
        return amount;
    }

    std::string getName() const override { return "PayPal"; }
    double getPaidAmount() const { return paidAmount_; }

private:
    std::string email_;
    double paidAmount_ = 0;
};

class WeChatPayment : public PaymentStrategy {
public:
    double pay(double amount) override {
        paidAmount_ = amount;
        return amount;
    }

    std::string getName() const override { return "WeChat"; }
    double getPaidAmount() const { return paidAmount_; }

private:
    double paidAmount_ = 0;
};

class PaymentContext {
public:
    void setStrategy(std::unique_ptr<PaymentStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    double checkout(double amount) {
        if (!strategy_) {
            return 0;
        }
        return strategy_->pay(amount);
    }

    std::string getStrategyName() const {
        return strategy_ ? strategy_->getName() : "";
    }

private:
    std::unique_ptr<PaymentStrategy> strategy_;
};

// ==================== 函数对象策略 ====================

template <typename T>
class Sorter {
public:
    using CompareStrategy = std::function<bool(const T&, const T&)>;

    Sorter() : compare_([](const T& a, const T& b) { return a < b; }) {}

    void setStrategy(CompareStrategy strategy) {
        compare_ = std::move(strategy);
    }

    void sort(std::vector<T>& data) {
        std::sort(data.begin(), data.end(), compare_);
    }

private:
    CompareStrategy compare_;
};

template <typename T>
struct SortStrategies {
    static bool ascending(const T& a, const T& b) { return a < b; }
    static bool descending(const T& a, const T& b) { return a > b; }
};

// ==================== 模板策略 ====================

struct PlainTextFormatter {
    static std::string format(const std::string& level, const std::string& message) {
        return "[" + level + "] " + message;
    }
};

struct JsonFormatter {
    static std::string format(const std::string& level, const std::string& message) {
        std::ostringstream oss;
        oss << "{\"level\":\"" << level << "\",\"message\":\"" << message << "\"}";
        return oss.str();
    }
};

struct XmlFormatter {
    static std::string format(const std::string& level, const std::string& message) {
        std::ostringstream oss;
        oss << "<log><level>" << level << "</level>"
            << "<message>" << message << "</message></log>";
        return oss.str();
    }
};

template <typename Formatter = PlainTextFormatter>
class Logger {
public:
    std::string info(const std::string& message) {
        return Formatter::format("INFO", message);
    }

    std::string warn(const std::string& message) {
        return Formatter::format("WARN", message);
    }

    std::string error(const std::string& message) {
        return Formatter::format("ERROR", message);
    }
};

// ==================== 策略与工厂结合 ====================

class CompressionStrategy {
public:
    virtual ~CompressionStrategy() = default;
    virtual std::vector<char> compress(const std::vector<char>& data) = 0;
    virtual std::vector<char> decompress(const std::vector<char>& data) = 0;
    virtual std::string getName() const = 0;
};

class NoCompression : public CompressionStrategy {
public:
    std::vector<char> compress(const std::vector<char>& data) override {
        return data;
    }

    std::vector<char> decompress(const std::vector<char>& data) override {
        return data;
    }

    std::string getName() const override { return "None"; }
};

class RunLengthEncoding : public CompressionStrategy {
public:
    std::vector<char> compress(const std::vector<char>& data) override {
        if (data.empty()) return {};

        std::vector<char> result;
        char current = data[0];
        int count = 1;

        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i] == current && count < 255) {
                ++count;
            } else {
                result.push_back(static_cast<char>(count));
                result.push_back(current);
                current = data[i];
                count = 1;
            }
        }
        result.push_back(static_cast<char>(count));
        result.push_back(current);

        return result;
    }

    std::vector<char> decompress(const std::vector<char>& data) override {
        std::vector<char> result;
        for (size_t i = 0; i + 1 < data.size(); i += 2) {
            int count = static_cast<unsigned char>(data[i]);
            char ch = data[i + 1];
            result.insert(result.end(), count, ch);
        }
        return result;
    }

    std::string getName() const override { return "RLE"; }
};

class CompressionFactory {
public:
    static std::unique_ptr<CompressionStrategy> create(const std::string& type) {
        if (type == "none" || type == "raw") {
            return std::make_unique<NoCompression>();
        }
        if (type == "rle") {
            return std::make_unique<RunLengthEncoding>();
        }
        throw std::invalid_argument("Unknown compression type: " + type);
    }
};

class Compressor {
public:
    void setStrategy(std::unique_ptr<CompressionStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    void setStrategy(const std::string& type) {
        strategy_ = CompressionFactory::create(type);
    }

    std::vector<char> compress(const std::vector<char>& data) {
        if (!strategy_) {
            strategy_ = std::make_unique<NoCompression>();
        }
        return strategy_->compress(data);
    }

    std::vector<char> decompress(const std::vector<char>& data) {
        if (!strategy_) {
            strategy_ = std::make_unique<NoCompression>();
        }
        return strategy_->decompress(data);
    }

    std::string getStrategyName() const {
        return strategy_ ? strategy_->getName() : "None";
    }

private:
    std::unique_ptr<CompressionStrategy> strategy_;
};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Strategy Tests ===" << std::endl;

    // 测试经典策略模式
    {
        Solution::PaymentContext context;

        context.setStrategy(std::make_unique<Solution::CreditCardPayment>("1234567890123456"));
        assert(context.getStrategyName() == "CreditCard");
        assert(context.checkout(99.99) == 99.99);

        context.setStrategy(std::make_unique<Solution::PayPalPayment>("user@example.com"));
        assert(context.getStrategyName() == "PayPal");
        assert(context.checkout(49.99) == 49.99);

        context.setStrategy(std::make_unique<Solution::WeChatPayment>());
        assert(context.getStrategyName() == "WeChat");
        assert(context.checkout(29.99) == 29.99);
    }
    std::cout << "  PaymentStrategy: PASSED" << std::endl;

    // 测试函数策略
    {
        Solution::Sorter<int> sorter;
        std::vector<int> data = {5, 2, 8, 1, 9, 3};

        // 升序
        sorter.setStrategy(Solution::SortStrategies<int>::ascending);
        sorter.sort(data);
        assert(data[0] == 1);
        assert(data[5] == 9);

        // 降序
        sorter.setStrategy(Solution::SortStrategies<int>::descending);
        sorter.sort(data);
        assert(data[0] == 9);
        assert(data[5] == 1);

        // Lambda 策略（偶数在前）
        sorter.setStrategy([](int a, int b) {
            return (a % 2) < (b % 2);
        });
        data = {5, 2, 8, 1, 9, 3};
        sorter.sort(data);
        assert(data[0] % 2 == 0);  // 第一个是偶数
    }
    std::cout << "  FunctionStrategy: PASSED" << std::endl;

    // 测试模板策略
    {
        Solution::Logger<Solution::PlainTextFormatter> plainLogger;
        assert(plainLogger.info("test") == "[INFO] test");
        assert(plainLogger.error("error") == "[ERROR] error");

        Solution::Logger<Solution::JsonFormatter> jsonLogger;
        assert(jsonLogger.info("test") == "{\"level\":\"INFO\",\"message\":\"test\"}");

        Solution::Logger<Solution::XmlFormatter> xmlLogger;
        std::string xml = xmlLogger.info("test");
        assert(xml.find("<level>INFO</level>") != std::string::npos);
        assert(xml.find("<message>test</message>") != std::string::npos);
    }
    std::cout << "  TemplateStrategy: PASSED" << std::endl;

    // 测试策略工厂
    {
        Solution::Compressor compressor;

        std::vector<char> data = {'A', 'A', 'A', 'A', 'B', 'B', 'C', 'C', 'C'};

        // RLE 压缩
        compressor.setStrategy("rle");
        assert(compressor.getStrategyName() == "RLE");

        auto compressed = compressor.compress(data);
        assert(compressed.size() < data.size());

        auto decompressed = compressor.decompress(compressed);
        assert(decompressed == data);

        // 无压缩
        compressor.setStrategy("none");
        assert(compressor.getStrategyName() == "None");
        auto noComp = compressor.compress(data);
        assert(noComp == data);
    }
    std::cout << "  CompressionStrategy: PASSED" << std::endl;
}

} // namespace StrategyImpl
