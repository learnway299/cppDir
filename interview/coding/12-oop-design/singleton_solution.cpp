/**
 * @file singleton_solution.cpp
 * @brief 单例模式实现 - 解答
 */

#include <iostream>
#include <mutex>
#include <memory>
#include <atomic>

/**
 * 题目1：饿汉式单例
 *
 * 特点：
 * - 程序启动时即创建
 * - 天然线程安全
 * - 可能增加启动时间
 */
class EagerSingleton {
public:
    static EagerSingleton& getInstance() {
        return instance_;
    }

    void doSomething() {
        std::cout << "EagerSingleton working\n";
    }

    // 禁止拷贝和赋值
    EagerSingleton(const EagerSingleton&) = delete;
    EagerSingleton& operator=(const EagerSingleton&) = delete;

private:
    EagerSingleton() {
        std::cout << "EagerSingleton constructed\n";
    }

    static EagerSingleton instance_;
};

// 静态成员初始化
EagerSingleton EagerSingleton::instance_;

/**
 * 题目2：懒汉式单例（双重检查锁定）
 *
 * 特点：
 * - 延迟初始化
 * - 只在第一次调用时加锁
 * - 需要 memory_order 保证正确性
 */
class LazySingleton {
public:
    static LazySingleton* getInstance() {
        // 第一次检查（无锁）
        LazySingleton* tmp = instance_.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            // 第二次检查（有锁）
            tmp = instance_.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new LazySingleton();
                instance_.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

    void doSomething() {
        std::cout << "LazySingleton working\n";
    }

    LazySingleton(const LazySingleton&) = delete;
    LazySingleton& operator=(const LazySingleton&) = delete;

private:
    LazySingleton() {
        std::cout << "LazySingleton constructed\n";
    }

    static std::atomic<LazySingleton*> instance_;
    static std::mutex mutex_;
};

std::atomic<LazySingleton*> LazySingleton::instance_{nullptr};
std::mutex LazySingleton::mutex_;

/**
 * 题目3：Meyers 单例（C++11 推荐方式）
 *
 * 特点：
 * - C++11 保证局部静态变量初始化的线程安全
 * - 代码简洁
 * - 自动处理销毁顺序
 */
class MeyersSingleton {
public:
    static MeyersSingleton& getInstance() {
        static MeyersSingleton instance;
        return instance;
    }

    void doSomething() {
        std::cout << "MeyersSingleton working\n";
    }

    MeyersSingleton(const MeyersSingleton&) = delete;
    MeyersSingleton& operator=(const MeyersSingleton&) = delete;

private:
    MeyersSingleton() {
        std::cout << "MeyersSingleton constructed\n";
    }

    ~MeyersSingleton() {
        std::cout << "MeyersSingleton destroyed\n";
    }
};

/**
 * 题目4：可销毁单例
 *
 * 支持手动销毁和重新创建
 */
class DestructibleSingleton {
public:
    static DestructibleSingleton* getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_.reset(new DestructibleSingleton());
        }
        return instance_.get();
    }

    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        instance_.reset();
        std::cout << "DestructibleSingleton destroyed\n";
    }

    static bool exists() {
        std::lock_guard<std::mutex> lock(mutex_);
        return instance_ != nullptr;
    }

    void doSomething() {
        std::cout << "DestructibleSingleton working\n";
    }

    DestructibleSingleton(const DestructibleSingleton&) = delete;
    DestructibleSingleton& operator=(const DestructibleSingleton&) = delete;

private:
    DestructibleSingleton() {
        std::cout << "DestructibleSingleton constructed\n";
    }

    ~DestructibleSingleton() = default;

    static std::unique_ptr<DestructibleSingleton> instance_;
    static std::mutex mutex_;
};

std::unique_ptr<DestructibleSingleton> DestructibleSingleton::instance_;
std::mutex DestructibleSingleton::mutex_;

/**
 * 题目5：模板化单例（CRTP）
 *
 * 使用 CRTP 使任意类成为单例
 */
template <typename T>
class Singleton {
public:
    static T& getInstance() {
        static T instance;
        return instance;
    }

    // 禁止拷贝
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

// 使用示例
class Logger : public Singleton<Logger> {
    friend class Singleton<Logger>;  // 允许基类访问私有构造函数

public:
    void log(const std::string& message) {
        std::cout << "[LOG] " << message << "\n";
    }

private:
    Logger() {
        std::cout << "Logger constructed\n";
    }
};

class Config : public Singleton<Config> {
    friend class Singleton<Config>;

public:
    void loadConfig(const std::string& file) {
        configFile_ = file;
        std::cout << "[Config] Loaded: " << file << "\n";
    }

    std::string getConfigFile() const { return configFile_; }

private:
    Config() : configFile_("default.conf") {
        std::cout << "Config constructed\n";
    }

    std::string configFile_;
};

/**
 * 扩展：带参数的单例
 */
template <typename T>
class ParameterizedSingleton {
public:
    template <typename... Args>
    static T& getInstance(Args&&... args) {
        static T instance(std::forward<Args>(args)...);
        return instance;
    }

    ParameterizedSingleton(const ParameterizedSingleton&) = delete;
    ParameterizedSingleton& operator=(const ParameterizedSingleton&) = delete;

protected:
    ParameterizedSingleton() = default;
    virtual ~ParameterizedSingleton() = default;
};

class Database : public ParameterizedSingleton<Database> {
    friend class ParameterizedSingleton<Database>;

public:
    void query(const std::string& sql) {
        std::cout << "[DB:" << connectionString_ << "] " << sql << "\n";
    }

private:
    explicit Database(const std::string& connStr)
        : connectionString_(connStr) {
        std::cout << "Database connected to: " << connStr << "\n";
    }

    std::string connectionString_;
};

int main() {
    std::cout << "=== 测试饿汉式单例 ===\n";
    EagerSingleton::getInstance().doSomething();

    std::cout << "\n=== 测试懒汉式单例 ===\n";
    LazySingleton::getInstance()->doSomething();

    std::cout << "\n=== 测试 Meyers 单例 ===\n";
    MeyersSingleton::getInstance().doSomething();

    std::cout << "\n=== 测试可销毁单例 ===\n";
    DestructibleSingleton::getInstance()->doSomething();
    std::cout << "Exists: " << (DestructibleSingleton::exists() ? "yes" : "no") << "\n";
    DestructibleSingleton::destroyInstance();
    std::cout << "Exists: " << (DestructibleSingleton::exists() ? "yes" : "no") << "\n";
    DestructibleSingleton::getInstance()->doSomething();  // 重新创建

    std::cout << "\n=== 测试 CRTP 单例 ===\n";
    Logger::getInstance().log("Hello World");
    Config::getInstance().loadConfig("app.conf");

    std::cout << "\n=== 测试带参数单例 ===\n";
    Database::getInstance("mysql://localhost:3306/test").query("SELECT * FROM users");
    Database::getInstance("ignored").query("SELECT * FROM orders");  // 参数被忽略

    std::cout << "\n=== 程序结束 ===\n";
    return 0;
}
