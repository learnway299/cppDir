/**
 * @file factory.cpp
 * @brief 工厂模式实现 - 面试题
 *
 * 包含：简单工厂、工厂方法、抽象工厂
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

/**
 * 题目1：实现简单工厂
 *
 * 要求：
 * 1. 根据类型创建不同的产品
 * 2. 使用智能指针管理生命周期
 */

// 产品基类
class Product {
public:
    virtual ~Product() = default;
    virtual void use() = 0;
    virtual std::string getName() const = 0;
};

// 具体产品
class ProductA : public Product {
public:
    void use() override {
        std::cout << "Using ProductA\n";
    }
    std::string getName() const override { return "ProductA"; }
};

class ProductB : public Product {
public:
    void use() override {
        std::cout << "Using ProductB\n";
    }
    std::string getName() const override { return "ProductB"; }
};

// TODO: 实现简单工厂
class SimpleFactory {
public:
    enum class ProductType { A, B };

    static std::unique_ptr<Product> create(ProductType type) {
        // TODO: 实现工厂方法
        return nullptr;
    }
};

/**
 * 题目2：实现工厂方法模式
 *
 * 要求：
 * 1. 定义创建对象的接口
 * 2. 让子类决定实例化哪个类
 */

// TODO: 实现工厂基类
class Factory {
public:
    virtual ~Factory() = default;
    virtual std::unique_ptr<Product> createProduct() = 0;
};

// TODO: 实现具体工厂
class FactoryA : public Factory {
public:
    std::unique_ptr<Product> createProduct() override {
        return nullptr;
    }
};

class FactoryB : public Factory {
public:
    std::unique_ptr<Product> createProduct() override {
        return nullptr;
    }
};

/**
 * 题目3：实现抽象工厂模式
 *
 * 要求：
 * 1. 创建相关产品的家族
 * 2. 不指定具体类
 */

// 抽象产品
class Button {
public:
    virtual ~Button() = default;
    virtual void render() = 0;
};

class TextBox {
public:
    virtual ~TextBox() = default;
    virtual void render() = 0;
};

// TODO: 实现具体产品（Windows 风格和 Mac 风格）

// TODO: 实现抽象工厂
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<TextBox> createTextBox() = 0;
};

/**
 * 题目4：实现注册式工厂
 *
 * 要求：
 * 1. 支持运行时注册新产品类型
 * 2. 使用字符串标识产品类型
 */
template <typename Base>
class RegisterFactory {
public:
    using Creator = std::function<std::unique_ptr<Base>()>;

    // TODO: 注册创建函数
    static void registerType(const std::string& typeName, Creator creator) {
    }

    // TODO: 创建产品
    static std::unique_ptr<Base> create(const std::string& typeName) {
        return nullptr;
    }

private:
    // TODO: 定义注册表
};

// 自动注册辅助宏
#define REGISTER_TYPE(Base, Derived, TypeName) \
    static bool _registered_##Derived = []() { \
        RegisterFactory<Base>::registerType(TypeName, []() { \
            return std::make_unique<Derived>(); \
        }); \
        return true; \
    }()

int main() {
    return 0;
}
