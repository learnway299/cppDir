/**
 * @file strategy_solution.cpp
 * @brief 策略模式实现 - 解答
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <unordered_map>

/**
 * 题目1：经典策略模式
 *
 * 运行时多态，通过虚函数实现
 */
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;
    virtual std::string getName() const = 0;
};

class CreditCardPayment : public PaymentStrategy {
public:
    CreditCardPayment(const std::string& cardNumber)
        : cardNumber_(cardNumber) {}

    void pay(double amount) override {
        std::cout << "Paid $" << amount << " with Credit Card "
                  << maskCardNumber() << "\n";
    }

    std::string getName() const override { return "CreditCard"; }

private:
    std::string maskCardNumber() const {
        if (cardNumber_.length() < 4) return "****";
        return "****-****-****-" + cardNumber_.substr(cardNumber_.length() - 4);
    }

    std::string cardNumber_;
};

class PayPalPayment : public PaymentStrategy {
public:
    PayPalPayment(const std::string& email) : email_(email) {}

    void pay(double amount) override {
        std::cout << "Paid $" << amount << " with PayPal (" << email_ << ")\n";
    }

    std::string getName() const override { return "PayPal"; }

private:
    std::string email_;
};

class WeChatPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        std::cout << "Paid $" << amount << " with WeChat Pay\n";
    }

    std::string getName() const override { return "WeChat"; }
};

class PaymentContext {
public:
    void setStrategy(std::unique_ptr<PaymentStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    void checkout(double amount) {
        if (!strategy_) {
            std::cout << "Error: No payment method selected\n";
            return;
        }
        std::cout << "Processing payment via " << strategy_->getName() << "...\n";
        strategy_->pay(amount);
    }

private:
    std::unique_ptr<PaymentStrategy> strategy_;
};

/**
 * 题目2：函数对象策略（现代 C++ 风格）
 *
 * 更灵活，支持 lambda
 */
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

// 预定义排序策略
template <typename T>
struct SortStrategies {
    static bool ascending(const T& a, const T& b) { return a < b; }
    static bool descending(const T& a, const T& b) { return a > b; }
};

/**
 * 题目3：模板策略（编译期多态）
 *
 * 零开销抽象，适合性能关键场景
 */
struct PlainTextFormatter {
    static std::string format(const std::string& level,
                              const std::string& message) {
        return "[" + level + "] " + message;
    }
};

struct JsonFormatter {
    static std::string format(const std::string& level,
                              const std::string& message) {
        std::ostringstream oss;
        oss << "{\"level\":\"" << level << "\",\"message\":\"" << message << "\"}";
        return oss.str();
    }
};

struct XmlFormatter {
    static std::string format(const std::string& level,
                              const std::string& message) {
        std::ostringstream oss;
        oss << "<log><level>" << level << "</level>"
            << "<message>" << message << "</message></log>";
        return oss.str();
    }
};

template <typename Formatter = PlainTextFormatter>
class Logger {
public:
    void info(const std::string& message) {
        std::cout << Formatter::format("INFO", message) << "\n";
    }

    void warn(const std::string& message) {
        std::cout << Formatter::format("WARN", message) << "\n";
    }

    void error(const std::string& message) {
        std::cout << Formatter::format("ERROR", message) << "\n";
    }
};

/**
 * 题目4：策略模式与工厂结合
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

// 压缩上下文
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
        std::cout << "Compressing with " << strategy_->getName() << "...\n";
        return strategy_->compress(data);
    }

    std::vector<char> decompress(const std::vector<char>& data) {
        if (!strategy_) {
            strategy_ = std::make_unique<NoCompression>();
        }
        return strategy_->decompress(data);
    }

private:
    std::unique_ptr<CompressionStrategy> strategy_;
};

int main() {
    std::cout << "=== 经典策略模式：支付系统 ===\n";
    {
        PaymentContext context;

        context.setStrategy(std::make_unique<CreditCardPayment>("1234567890123456"));
        context.checkout(99.99);

        context.setStrategy(std::make_unique<PayPalPayment>("user@example.com"));
        context.checkout(49.99);

        context.setStrategy(std::make_unique<WeChatPayment>());
        context.checkout(29.99);
    }

    std::cout << "\n=== 函数策略：排序 ===\n";
    {
        Sorter<int> sorter;
        std::vector<int> data = {5, 2, 8, 1, 9, 3};

        // 升序
        sorter.setStrategy(SortStrategies<int>::ascending);
        sorter.sort(data);
        std::cout << "Ascending: ";
        for (int x : data) std::cout << x << " ";
        std::cout << "\n";

        // 降序
        sorter.setStrategy(SortStrategies<int>::descending);
        sorter.sort(data);
        std::cout << "Descending: ";
        for (int x : data) std::cout << x << " ";
        std::cout << "\n";

        // Lambda 策略
        sorter.setStrategy([](int a, int b) {
            return (a % 2) < (b % 2);  // 偶数在前
        });
        data = {5, 2, 8, 1, 9, 3};
        sorter.sort(data);
        std::cout << "Even first: ";
        for (int x : data) std::cout << x << " ";
        std::cout << "\n";
    }

    std::cout << "\n=== 模板策略：日志格式化 ===\n";
    {
        Logger<PlainTextFormatter> plainLogger;
        plainLogger.info("Application started");
        plainLogger.error("Something went wrong");

        std::cout << "\n";

        Logger<JsonFormatter> jsonLogger;
        jsonLogger.info("Application started");
        jsonLogger.error("Something went wrong");

        std::cout << "\n";

        Logger<XmlFormatter> xmlLogger;
        xmlLogger.info("Application started");
    }

    std::cout << "\n=== 策略工厂：压缩 ===\n";
    {
        Compressor compressor;

        std::vector<char> data = {'A', 'A', 'A', 'A', 'B', 'B', 'C', 'C', 'C'};
        std::cout << "Original size: " << data.size() << "\n";

        compressor.setStrategy("rle");
        auto compressed = compressor.compress(data);
        std::cout << "Compressed size: " << compressed.size() << "\n";

        auto decompressed = compressor.decompress(compressed);
        std::cout << "Decompressed: ";
        for (char c : decompressed) std::cout << c;
        std::cout << " (size: " << decompressed.size() << ")\n";
    }

    return 0;
}
