/**
 * @file type_erasure.cpp
 * @brief 类型擦除实现 - 面试者实现文件
 */

#include "type_erasure.h"

namespace TypeErasureImpl {

// ==================== Printable 实现 ====================

template <typename T>
Printable::Printable(T obj) {
    // TODO: 使用 Model<T> 创建 pImpl_
}

Printable::Printable(const Printable& other) {
    // TODO: 克隆 pImpl_
}

Printable::Printable(Printable&& other) noexcept = default;

Printable& Printable::operator=(const Printable& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

Printable& Printable::operator=(Printable&& other) noexcept = default;

std::string Printable::print() const {
    // TODO: 调用 pImpl_->print()
    return "";
}

// ==================== Document / Image 实现 ====================

std::string Document::print() const {
    // TODO: 返回 "Document: " + content
    return "";
}

std::string Image::print() const {
    // TODO: 返回 "Image: widthxheight"
    return "";
}

// ==================== Function 实现 ====================

template <typename R, typename... Args>
template <typename F>
Function<R(Args...)>::Function(F f) {
    // TODO: 使用 Model<F> 创建 pImpl_
}

template <typename R, typename... Args>
Function<R(Args...)>::Function(const Function& other) {
    // TODO: 克隆 pImpl_
}

template <typename R, typename... Args>
Function<R(Args...)>& Function<R(Args...)>::operator=(const Function& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename R, typename... Args>
R Function<R(Args...)>::operator()(Args... args) const {
    // TODO: 调用 pImpl_->invoke(args...)
    if (!pImpl_) {
        throw std::bad_function_call();
    }
    return R();
}

template <typename R, typename... Args>
Function<R(Args...)>::operator bool() const {
    return pImpl_ != nullptr;
}

// ==================== Any 实现 ====================

template <typename T>
Any::Any(T value) {
    // TODO: 使用 Model<decay_t<T>> 创建 pImpl_
}

Any::Any(const Any& other) {
    // TODO: 克隆 pImpl_
}

Any& Any::operator=(const Any& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
Any& Any::operator=(T value) {
    // TODO: 创建新的 Model<T>
    return *this;
}

bool Any::hasValue() const {
    return pImpl_ != nullptr;
}

const std::type_info& Any::type() const {
    return pImpl_ ? pImpl_->type() : typeid(void);
}

template <typename T>
T* Any::tryGet() {
    // TODO: 类型检查后返回指针
    return nullptr;
}

template <typename T>
T& Any::get() {
    T* ptr = tryGet<T>();
    if (!ptr) {
        throw std::bad_cast();
    }
    return *ptr;
}

template <typename T>
const T& Any::get() const {
    return const_cast<Any*>(this)->get<T>();
}

void Any::reset() {
    pImpl_.reset();
}

template <typename T>
T any_cast(Any& a) {
    return a.get<std::remove_reference_t<T>>();
}

template <typename T>
T any_cast(const Any& a) {
    return a.get<std::remove_reference_t<T>>();
}

// ==================== AnyWithSBO 实现 ====================

AnyWithSBO::AnyWithSBO() : vtable_(nullptr) {}

template <typename T>
AnyWithSBO::AnyWithSBO(T value) {
    // TODO: 根据对象大小决定栈存储还是堆存储
    // 小对象：placement new 在 buffer_ 中
    // 大对象：new T 存储指针
}

AnyWithSBO::~AnyWithSBO() {
    // TODO: 调用 vtable_->destroy
}

AnyWithSBO::AnyWithSBO(const AnyWithSBO& other) : vtable_(other.vtable_) {
    // TODO: 调用 vtable_->copy
}

AnyWithSBO::AnyWithSBO(AnyWithSBO&& other) noexcept : vtable_(other.vtable_) {
    // TODO: 调用 vtable_->move
}

AnyWithSBO& AnyWithSBO::operator=(const AnyWithSBO& other) {
    // TODO: 实现拷贝赋值（使用 copy-and-swap）
    return *this;
}

AnyWithSBO& AnyWithSBO::operator=(AnyWithSBO&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

void AnyWithSBO::swap(AnyWithSBO& other) noexcept {
    std::swap(vtable_, other.vtable_);
    std::swap(buffer_, other.buffer_);
}

bool AnyWithSBO::hasValue() const {
    return vtable_ != nullptr;
}

const std::type_info& AnyWithSBO::type() const {
    return vtable_ ? vtable_->type() : typeid(void);
}

template <typename T>
T* AnyWithSBO::tryGet() {
    // TODO: 根据 isSmall 决定如何获取指针
    return nullptr;
}

template <typename T, bool IsSmall>
const AnyWithSBO::VTable& AnyWithSBO::getVTable() {
    // TODO: 实现 VTable 静态实例
    static const VTable vtable = {
        []() -> const std::type_info& { return typeid(T); },
        [](char*) {},
        [](const char*, char*) {},
        [](char*, char*) {},
        IsSmall
    };
    return vtable;
}

// 显式模板实例化
template class Function<int(int, int)>;
template class Function<void()>;
template Any::Any(int);
template Any::Any(std::string);
template int& Any::get<int>();
template std::string& Any::get<std::string>();
template int any_cast<int>(Any&);
template std::string any_cast<std::string>(Any&);

} // namespace TypeErasureImpl
