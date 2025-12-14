/**
 * @file type_erasure_impl.cpp
 * @brief 类型擦除实现 - 面试者实现文件
 */

#include "type_erasure_impl.h"
#include <iostream>

namespace TypeErasureTemplateImpl {

// ==================== Function 实现 ====================

template <typename R, typename... Args>
template <typename F, typename>
Function<R(Args...)>::Function(F&& f) {
    // TODO: 创建 CallableImpl 实例
}

template <typename R, typename... Args>
Function<R(Args...)>::Function(const Function& other) {
    // TODO: 克隆 callable_
}

template <typename R, typename... Args>
Function<R(Args...)>& Function<R(Args...)>::operator=(const Function& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename R, typename... Args>
R Function<R(Args...)>::operator()(Args... args) const {
    // TODO: 调用 callable_->invoke()
    if (!callable_) {
        throw std::bad_function_call();
    }
    return R();
}

template <typename R, typename... Args>
Function<R(Args...)>::operator bool() const noexcept {
    return callable_ != nullptr;
}

// ==================== Any 实现 ====================

template <typename T, typename>
Any::Any(T&& value) {
    // TODO: 创建 Holder 实例
}

Any::Any(const Any& other) {
    // TODO: 克隆 holder_
}

Any& Any::operator=(const Any& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

const std::type_info& Any::type() const noexcept {
    return holder_ ? holder_->type() : typeid(void);
}

bool Any::has_value() const noexcept {
    return holder_ != nullptr;
}

void Any::reset() noexcept {
    holder_.reset();
}

template <typename T>
T* Any::get_if() noexcept {
    // TODO: 类型检查后返回指针
    return nullptr;
}

template <typename T>
const T* Any::get_if() const noexcept {
    // TODO: 类型检查后返回指针
    return nullptr;
}

template <typename T>
T& Any::get() {
    T* ptr = get_if<T>();
    if (!ptr) {
        throw std::bad_cast();
    }
    return *ptr;
}

template <typename T>
const T& Any::get() const {
    const T* ptr = get_if<T>();
    if (!ptr) {
        throw std::bad_cast();
    }
    return *ptr;
}

// ==================== Drawable 实现 ====================

template <typename T>
Drawable::Drawable(T x) {
    // TODO: 创建 Model 实例
}

Drawable::Drawable(const Drawable& other) {
    // TODO: 克隆 self_
}

Drawable& Drawable::operator=(const Drawable& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

void Drawable::draw() const {
    // TODO: 调用 self_->draw()
}

// ==================== 测试辅助类型实现 ====================

void Circle::draw() const {
    std::cout << "Drawing Circle (r=" << radius << ")\n";
}

void Square::draw() const {
    std::cout << "Drawing Square (s=" << side << ")\n";
}

int add(int a, int b) {
    return a + b;
}

// 显式模板实例化
template class Function<int(int, int)>;
template class Function<int(int)>;
template Any::Any(int&&);
template Any::Any(std::string&&);
template int& Any::get<int>();
template std::string& Any::get<std::string>();
template Drawable::Drawable(Circle);
template Drawable::Drawable(Square);

} // namespace TypeErasureTemplateImpl
