/**
 * @file factory_solution.cpp
 * @brief 工厂模式实现 - 解答
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>

// ============ 产品基类 ============
class Product {
public:
    virtual ~Product() = default;
    virtual void use() = 0;
    virtual std::string getName() const = 0;
};

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

class ProductC : public Product {
public:
    void use() override {
        std::cout << "Using ProductC\n";
    }
    std::string getName() const override { return "ProductC"; }
};

/**
 * 题目1：简单工厂
 *
 * 优点：创建逻辑集中
 * 缺点：新增产品需要修改工厂类（违反开闭原则）
 */
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

    // 字符串版本
    static std::unique_ptr<Product> create(const std::string& typeName) {
        if (typeName == "A") return std::make_unique<ProductA>();
        if (typeName == "B") return std::make_unique<ProductB>();
        if (typeName == "C") return std::make_unique<ProductC>();
        throw std::invalid_argument("Unknown product type: " + typeName);
    }
};

/**
 * 题目2：工厂方法模式
 *
 * 优点：遵循开闭原则，新增产品只需新增工厂类
 * 缺点：类数量增多
 */
class Factory {
public:
    virtual ~Factory() = default;
    virtual std::unique_ptr<Product> createProduct() = 0;

    // 模板方法：使用产品
    void doWork() {
        auto product = createProduct();
        std::cout << "Working with " << product->getName() << "\n";
        product->use();
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

/**
 * 题目3：抽象工厂模式
 *
 * 用于创建相关产品的家族
 */

// ---- 抽象产品 ----
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

// ---- Windows 风格产品 ----
class WindowsButton : public Button {
public:
    void render() override {
        std::cout << "[Windows Button] Rendering...\n";
    }
};

class WindowsTextBox : public TextBox {
public:
    void render() override {
        std::cout << "[Windows TextBox] Rendering...\n";
    }
};

// ---- Mac 风格产品 ----
class MacButton : public Button {
public:
    void render() override {
        std::cout << "[Mac Button] Rendering...\n";
    }
};

class MacTextBox : public TextBox {
public:
    void render() override {
        std::cout << "[Mac TextBox] Rendering...\n";
    }
};

// ---- 抽象工厂 ----
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

// 客户端代码
void createUI(GUIFactory& factory) {
    auto button = factory.createButton();
    auto textBox = factory.createTextBox();
    button->render();
    textBox->render();
}

/**
 * 题目4：注册式工厂
 *
 * 支持运行时动态注册新类型
 */
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

private:
    static std::unordered_map<std::string, Creator>& getRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }
};

// 自动注册辅助宏
#define REGISTER_TYPE(Base, Derived, TypeName) \
    namespace { \
        bool _registered_##Derived = []() { \
            RegisterFactory<Base>::registerType(TypeName, []() { \
                return std::make_unique<Derived>(); \
            }); \
            return true; \
        }(); \
    }

// 注册产品
REGISTER_TYPE(Product, ProductA, "ProductA")
REGISTER_TYPE(Product, ProductB, "ProductB")
REGISTER_TYPE(Product, ProductC, "ProductC")

/**
 * 扩展：带参数的工厂
 */
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

private:
    static std::unordered_map<std::string, Creator>& getRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }
};

int main() {
    std::cout << "=== 简单工厂 ===\n";
    {
        auto a = SimpleFactory::create(SimpleFactory::ProductType::A);
        auto b = SimpleFactory::create("B");
        a->use();
        b->use();
    }

    std::cout << "\n=== 工厂方法 ===\n";
    {
        std::unique_ptr<Factory> factory = std::make_unique<FactoryA>();
        factory->doWork();

        factory = std::make_unique<FactoryB>();
        factory->doWork();
    }

    std::cout << "\n=== 抽象工厂 ===\n";
    {
        std::cout << "Windows UI:\n";
        WindowsFactory winFactory;
        createUI(winFactory);

        std::cout << "\nMac UI:\n";
        MacFactory macFactory;
        createUI(macFactory);
    }

    std::cout << "\n=== 注册式工厂 ===\n";
    {
        std::cout << "Registered types: ";
        for (const auto& type : RegisterFactory<Product>::getRegisteredTypes()) {
            std::cout << type << " ";
        }
        std::cout << "\n";

        auto product = RegisterFactory<Product>::create("ProductA");
        product->use();

        product = RegisterFactory<Product>::create("ProductB");
        product->use();
    }

    return 0;
}
