/**
 * @file factory_solution.cpp
 * @brief 工厂模式实现 - 参考答案
 */

#include "factory.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

namespace FactoryImpl {

namespace Solution {

// ==================== Product 实现 ====================

class ProductA : public Product {
public:
    void use() override { used_ = true; }
    std::string getName() const override { return "ProductA"; }
    bool wasUsed() const { return used_; }
private:
    bool used_ = false;
};

class ProductB : public Product {
public:
    void use() override { used_ = true; }
    std::string getName() const override { return "ProductB"; }
    bool wasUsed() const { return used_; }
private:
    bool used_ = false;
};

class ProductC : public Product {
public:
    void use() override { used_ = true; }
    std::string getName() const override { return "ProductC"; }
    bool wasUsed() const { return used_; }
private:
    bool used_ = false;
};

// ==================== 简单工厂 ====================

class SimpleFactory {
public:
    enum class ProductType { A, B, C };

    static std::unique_ptr<Product> create(ProductType type) {
        switch (type) {
            case ProductType::A:
                return std::make_unique<ProductA>();
            case ProductType::B:
                return std::make_unique<ProductB>();
            case ProductType::C:
                return std::make_unique<ProductC>();
            default:
                throw std::invalid_argument("Unknown product type");
        }
    }

    static std::unique_ptr<Product> create(const std::string& typeName) {
        if (typeName == "A") return std::make_unique<ProductA>();
        if (typeName == "B") return std::make_unique<ProductB>();
        if (typeName == "C") return std::make_unique<ProductC>();
        throw std::invalid_argument("Unknown product type: " + typeName);
    }
};

// ==================== 工厂方法模式 ====================

class Factory {
public:
    virtual ~Factory() = default;
    virtual std::unique_ptr<Product> createProduct() = 0;

    std::string doWork() {
        auto product = createProduct();
        std::string name = product->getName();
        product->use();
        return name;
    }
};

class FactoryA : public Factory {
public:
    std::unique_ptr<Product> createProduct() override {
        return std::make_unique<ProductA>();
    }
};

class FactoryB : public Factory {
public:
    std::unique_ptr<Product> createProduct() override {
        return std::make_unique<ProductB>();
    }
};

class FactoryC : public Factory {
public:
    std::unique_ptr<Product> createProduct() override {
        return std::make_unique<ProductC>();
    }
};

// ==================== 抽象工厂模式 ====================

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
    std::string render() override { return "Windows Button"; }
};

class WindowsTextBox : public TextBox {
public:
    std::string render() override { return "Windows TextBox"; }
};

class MacButton : public Button {
public:
    std::string render() override { return "Mac Button"; }
};

class MacTextBox : public TextBox {
public:
    std::string render() override { return "Mac TextBox"; }
};

class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<TextBox> createTextBox() = 0;
};

class WindowsFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    std::unique_ptr<TextBox> createTextBox() override {
        return std::make_unique<WindowsTextBox>();
    }
};

class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
    std::unique_ptr<TextBox> createTextBox() override {
        return std::make_unique<MacTextBox>();
    }
};

// ==================== 注册式工厂 ====================

template <typename Base>
class RegisterFactory {
public:
    using Creator = std::function<std::unique_ptr<Base>()>;

    static void registerType(const std::string& typeName, Creator creator) {
        getRegistry()[typeName] = std::move(creator);
    }

    static std::unique_ptr<Base> create(const std::string& typeName) {
        auto& registry = getRegistry();
        auto it = registry.find(typeName);
        if (it == registry.end()) {
            throw std::invalid_argument("Unknown type: " + typeName);
        }
        return it->second();
    }

    static bool hasType(const std::string& typeName) {
        return getRegistry().count(typeName) > 0;
    }

    static std::vector<std::string> getRegisteredTypes() {
        std::vector<std::string> types;
        for (const auto& [name, _] : getRegistry()) {
            types.push_back(name);
        }
        return types;
    }

    static void clear() {
        getRegistry().clear();
    }

private:
    static std::unordered_map<std::string, Creator>& getRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }
};

// ==================== 带参数的工厂 ====================

template <typename Base, typename... Args>
class ParameterizedFactory {
public:
    using Creator = std::function<std::unique_ptr<Base>(Args...)>;

    static void registerType(const std::string& typeName, Creator creator) {
        getRegistry()[typeName] = std::move(creator);
    }

    static std::unique_ptr<Base> create(const std::string& typeName, Args... args) {
        auto& registry = getRegistry();
        auto it = registry.find(typeName);
        if (it == registry.end()) {
            throw std::invalid_argument("Unknown type: " + typeName);
        }
        return it->second(std::forward<Args>(args)...);
    }

    static void clear() {
        getRegistry().clear();
    }

private:
    static std::unordered_map<std::string, Creator>& getRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }
};

} // namespace Solution

// ==================== 测试函数 ====================

void testFactorySolution() {
    std::cout << "=== Factory Tests (Solution) ===" << std::endl;

    // 测试简单工厂
    {
        auto a = Solution::SimpleFactory::create(Solution::SimpleFactory::ProductType::A);
        auto b = Solution::SimpleFactory::create("B");
        assert(a->getName() == "ProductA");
        assert(b->getName() == "ProductB");

        a->use();
        b->use();
    }
    std::cout << "  SimpleFactory: PASSED" << std::endl;

    // 测试工厂方法
    {
        std::unique_ptr<Solution::Factory> factory = std::make_unique<Solution::FactoryA>();
        assert(factory->doWork() == "ProductA");

        factory = std::make_unique<Solution::FactoryB>();
        assert(factory->doWork() == "ProductB");

        factory = std::make_unique<Solution::FactoryC>();
        assert(factory->doWork() == "ProductC");
    }
    std::cout << "  FactoryMethod: PASSED" << std::endl;

    // 测试抽象工厂
    {
        Solution::WindowsFactory winFactory;
        auto winBtn = winFactory.createButton();
        auto winTxt = winFactory.createTextBox();
        assert(winBtn->render() == "Windows Button");
        assert(winTxt->render() == "Windows TextBox");

        Solution::MacFactory macFactory;
        auto macBtn = macFactory.createButton();
        auto macTxt = macFactory.createTextBox();
        assert(macBtn->render() == "Mac Button");
        assert(macTxt->render() == "Mac TextBox");
    }
    std::cout << "  AbstractFactory: PASSED" << std::endl;

    // 测试注册式工厂
    {
        Solution::RegisterFactory<Product>::clear();

        Solution::RegisterFactory<Product>::registerType("ProductA", []() {
            return std::make_unique<Solution::ProductA>();
        });
        Solution::RegisterFactory<Product>::registerType("ProductB", []() {
            return std::make_unique<Solution::ProductB>();
        });

        assert(Solution::RegisterFactory<Product>::hasType("ProductA"));
        assert(Solution::RegisterFactory<Product>::hasType("ProductB"));
        assert(!Solution::RegisterFactory<Product>::hasType("ProductZ"));

        auto productA = Solution::RegisterFactory<Product>::create("ProductA");
        assert(productA->getName() == "ProductA");

        auto productB = Solution::RegisterFactory<Product>::create("ProductB");
        assert(productB->getName() == "ProductB");

        Solution::RegisterFactory<Product>::clear();
    }
    std::cout << "  RegisterFactory: PASSED" << std::endl;

    // 测试带参数的工厂
    {
        using IntFactory = Solution::ParameterizedFactory<Product, int>;
        IntFactory::clear();

        IntFactory::registerType("ProductA", [](int) {
            return std::make_unique<Solution::ProductA>();
        });

        auto product = IntFactory::create("ProductA", 42);
        assert(product->getName() == "ProductA");

        IntFactory::clear();
    }
    std::cout << "  ParameterizedFactory: PASSED" << std::endl;
}

} // namespace FactoryImpl
