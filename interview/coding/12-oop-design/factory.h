/**
 * @file factory.h
 * @brief 工厂模式实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 简单工厂
 * 2. 工厂方法模式
 * 3. 抽象工厂模式
 * 4. 注册式工厂
 * 5. 带参数的工厂
 */

#ifndef FACTORY_H
#define FACTORY_H

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace FactoryImpl {

// ==================== 题目1: 简单工厂 ====================

/**
 * 题目1: 实现简单工厂
 *
 * 优点：创建逻辑集中管理
 * 缺点：新增产品需要修改工厂类（违反开闭原则）
 *
 * 示例：
 *   auto product = SimpleFactory::create(SimpleFactory::ProductType::A);
 *   product->use();
 */

// 产品基类
class Product {
public:
    virtual ~Product() = default;
    virtual void use() = 0;
    virtual std::string getName() const = 0;
};

class ProductA : public Product {
public:
    void use() override;
    std::string getName() const override;
private:
    bool used_ = false;
};

class ProductB : public Product {
public:
    void use() override;
    std::string getName() const override;
private:
    bool used_ = false;
};

class ProductC : public Product {
public:
    void use() override;
    std::string getName() const override;
private:
    bool used_ = false;
};

// 简单工厂
class SimpleFactory {
public:
    enum class ProductType { A, B, C };

    static std::unique_ptr<Product> create(ProductType type);
    static std::unique_ptr<Product> create(const std::string& typeName);
};

// ==================== 题目2: 工厂方法模式 ====================

/**
 * 题目2: 实现工厂方法模式
 *
 * 优点：遵循开闭原则，新增产品只需新增工厂类
 * 缺点：类数量增多
 *
 * 示例：
 *   std::unique_ptr<Factory> factory = std::make_unique<FactoryA>();
 *   auto product = factory->createProduct();
 */
class Factory {
public:
    virtual ~Factory() = default;
    virtual std::unique_ptr<Product> createProduct() = 0;

    // 模板方法
    std::string doWork();
};

class FactoryA : public Factory {
public:
    std::unique_ptr<Product> createProduct() override;
};

class FactoryB : public Factory {
public:
    std::unique_ptr<Product> createProduct() override;
};

class FactoryC : public Factory {
public:
    std::unique_ptr<Product> createProduct() override;
};

// ==================== 题目3: 抽象工厂模式 ====================

/**
 * 题目3: 实现抽象工厂模式
 *
 * 用于创建相关产品的家族（如 Windows/Mac 风格的 UI 组件）
 *
 * 示例：
 *   std::unique_ptr<GUIFactory> factory = std::make_unique<WindowsFactory>();
 *   auto button = factory->createButton();
 *   auto textbox = factory->createTextBox();
 */
class Button {
public:
    virtual ~Button() = default;
    virtual std::string render() = 0;
};

class TextBox {
public:
    virtual ~TextBox() = default;
    virtual std::string render() = 0;
};

class WindowsButton : public Button {
public:
    std::string render() override;
};

class WindowsTextBox : public TextBox {
public:
    std::string render() override;
};

class MacButton : public Button {
public:
    std::string render() override;
};

class MacTextBox : public TextBox {
public:
    std::string render() override;
};

class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<TextBox> createTextBox() = 0;
};

class WindowsFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override;
    std::unique_ptr<TextBox> createTextBox() override;
};

class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override;
    std::unique_ptr<TextBox> createTextBox() override;
};

// ==================== 题目4: 注册式工厂 ====================

/**
 * 题目4: 实现注册式工厂
 *
 * 支持运行时动态注册新类型
 *
 * 示例：
 *   RegisterFactory<Product>::registerType("ProductA", []() {
 *       return std::make_unique<ProductA>();
 *   });
 *   auto product = RegisterFactory<Product>::create("ProductA");
 */
template <typename Base>
class RegisterFactory {
public:
    using Creator = std::function<std::unique_ptr<Base>()>;

    static void registerType(const std::string& typeName, Creator creator);
    static std::unique_ptr<Base> create(const std::string& typeName);
    static bool hasType(const std::string& typeName);
    static std::vector<std::string> getRegisteredTypes();
    static void clear();

private:
    static std::unordered_map<std::string, Creator>& getRegistry();
};

// ==================== 题目5: 带参数的工厂 ====================

/**
 * 题目5: 实现带参数的工厂
 *
 * 创建时可以传递参数
 */
template <typename Base, typename... Args>
class ParameterizedFactory {
public:
    using Creator = std::function<std::unique_ptr<Base>(Args...)>;

    static void registerType(const std::string& typeName, Creator creator);
    static std::unique_ptr<Base> create(const std::string& typeName, Args... args);
    static void clear();

private:
    static std::unordered_map<std::string, Creator>& getRegistry();
};

// ==================== 测试函数声明 ====================

void testFactory();          // 测试面试者实现
void testFactorySolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testFactory();
    testFactorySolution();
}

} // namespace FactoryImpl

#endif // FACTORY_H
