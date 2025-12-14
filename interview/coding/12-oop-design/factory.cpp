/**
 * @file factory.cpp
 * @brief 工厂模式实现 - 面试者实现文件
 */

#include "factory.h"
#include <stdexcept>
#include <iostream>

namespace FactoryImpl {

// ==================== Product 实现 ====================

void ProductA::use() {
    // TODO: 实现使用逻辑
}

std::string ProductA::getName() const {
    return "ProductA";
}

void ProductB::use() {
    // TODO: 实现使用逻辑
}

std::string ProductB::getName() const {
    return "ProductB";
}

void ProductC::use() {
    // TODO: 实现使用逻辑
}

std::string ProductC::getName() const {
    return "ProductC";
}

// ==================== SimpleFactory 实现 ====================

std::unique_ptr<Product> SimpleFactory::create(ProductType type) {
    // TODO: 根据类型创建产品
    // 提示：使用 switch 语句和 std::make_unique
    return nullptr;
}

std::unique_ptr<Product> SimpleFactory::create(const std::string& typeName) {
    // TODO: 根据名称创建产品
    return nullptr;
}

// ==================== Factory 实现 ====================

std::string Factory::doWork() {
    // TODO: 模板方法 - 创建产品并使用
    auto product = createProduct();
    std::string name = product->getName();
    product->use();
    return name;
}

std::unique_ptr<Product> FactoryA::createProduct() {
    // TODO: 创建 ProductA
    return nullptr;
}

std::unique_ptr<Product> FactoryB::createProduct() {
    // TODO: 创建 ProductB
    return nullptr;
}

std::unique_ptr<Product> FactoryC::createProduct() {
    // TODO: 创建 ProductC
    return nullptr;
}

// ==================== GUI 组件实现 ====================

std::string WindowsButton::render() {
    // TODO: 返回 "Windows Button"
    return "";
}

std::string WindowsTextBox::render() {
    // TODO: 返回 "Windows TextBox"
    return "";
}

std::string MacButton::render() {
    // TODO: 返回 "Mac Button"
    return "";
}

std::string MacTextBox::render() {
    // TODO: 返回 "Mac TextBox"
    return "";
}

// ==================== GUIFactory 实现 ====================

std::unique_ptr<Button> WindowsFactory::createButton() {
    // TODO: 创建 WindowsButton
    return nullptr;
}

std::unique_ptr<TextBox> WindowsFactory::createTextBox() {
    // TODO: 创建 WindowsTextBox
    return nullptr;
}

std::unique_ptr<Button> MacFactory::createButton() {
    // TODO: 创建 MacButton
    return nullptr;
}

std::unique_ptr<TextBox> MacFactory::createTextBox() {
    // TODO: 创建 MacTextBox
    return nullptr;
}

// ==================== RegisterFactory 模板实现 ====================

template <typename Base>
void RegisterFactory<Base>::registerType(const std::string& typeName, Creator creator) {
    // TODO: 注册类型到注册表
}

template <typename Base>
std::unique_ptr<Base> RegisterFactory<Base>::create(const std::string& typeName) {
    // TODO: 从注册表查找并创建
    return nullptr;
}

template <typename Base>
bool RegisterFactory<Base>::hasType(const std::string& typeName) {
    // TODO: 检查类型是否已注册
    return false;
}

template <typename Base>
std::vector<std::string> RegisterFactory<Base>::getRegisteredTypes() {
    // TODO: 返回所有已注册的类型名
    return {};
}

template <typename Base>
void RegisterFactory<Base>::clear() {
    // TODO: 清空注册表
}

template <typename Base>
std::unordered_map<std::string, typename RegisterFactory<Base>::Creator>&
RegisterFactory<Base>::getRegistry() {
    static std::unordered_map<std::string, Creator> registry;
    return registry;
}

// ==================== ParameterizedFactory 模板实现 ====================

template <typename Base, typename... Args>
void ParameterizedFactory<Base, Args...>::registerType(const std::string& typeName, Creator creator) {
    // TODO: 注册带参数的创建器
}

template <typename Base, typename... Args>
std::unique_ptr<Base> ParameterizedFactory<Base, Args...>::create(const std::string& typeName, Args... args) {
    // TODO: 从注册表查找并创建（传递参数）
    return nullptr;
}

template <typename Base, typename... Args>
void ParameterizedFactory<Base, Args...>::clear() {
    // TODO: 清空注册表
}

template <typename Base, typename... Args>
std::unordered_map<std::string, typename ParameterizedFactory<Base, Args...>::Creator>&
ParameterizedFactory<Base, Args...>::getRegistry() {
    static std::unordered_map<std::string, Creator> registry;
    return registry;
}

// 显式模板实例化
template class RegisterFactory<Product>;

// ==================== 测试函数 ====================

void testFactory() {
    std::cout << "=== Factory Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace FactoryImpl
