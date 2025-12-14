/**
 * @file crtp.cpp
 * @brief CRTP (Curiously Recurring Template Pattern) - 面试者实现文件
 */

#include "crtp.h"
#include <sstream>

namespace CRTPImpl {

// ==================== Circle 实现 ====================

Circle::Circle(double radius) : radius_(radius) {}

std::string Circle::nameImpl() const {
    // TODO: 返回 "Circle"
    return "";
}

double Circle::areaImpl() const {
    // TODO: 返回 M_PI * radius_ * radius_
    return 0.0;
}

double Circle::getRadius() const {
    return radius_;
}

// ==================== Rectangle 实现 ====================

Rectangle::Rectangle(double width, double height) : width_(width), height_(height) {}

std::string Rectangle::nameImpl() const {
    // TODO: 返回 "Rectangle"
    return "";
}

double Rectangle::areaImpl() const {
    // TODO: 返回 width_ * height_
    return 0.0;
}

// ==================== Config 实现 ====================

Config::Config() : port_(8080), host_("localhost") {}

std::string Config::serializeImpl() const {
    // TODO: 返回 "host:port" 格式
    return "";
}

void Config::deserializeImpl(const std::string& data) {
    // TODO: 解析 "host:port" 格式
}

int Config::getPort() const {
    return port_;
}

const std::string& Config::getHost() const {
    return host_;
}

// ==================== ObjectCounter 实现 ====================

template <typename Derived>
ObjectCounter<Derived>::ObjectCounter() {
    // TODO: 增加计数
}

template <typename Derived>
ObjectCounter<Derived>::~ObjectCounter() {
    // TODO: 减少计数
}

template <typename Derived>
ObjectCounter<Derived>::ObjectCounter(const ObjectCounter&) {
    // TODO: 增加计数
}

template <typename Derived>
ObjectCounter<Derived>::ObjectCounter(ObjectCounter&&) noexcept {
    // TODO: 增加计数
}

// ==================== Entity 实现 ====================

Entity::Entity(int id) : id_(id) {}

int Entity::getId() const {
    return id_;
}

// ==================== Component 实现 ====================

Component::Component(const std::string& name) : name_(name) {}

const std::string& Component::getName() const {
    return name_;
}

// ==================== Person 实现 ====================

Person::Person(const std::string& name, int age) : name_(name), age_(age) {}

std::string Person::toString() const {
    // TODO: 返回 "Person(name, age)" 格式
    return "";
}

int Person::compareImpl(const Person& other) const {
    // TODO: 先比较年龄，再比较名字
    return 0;
}

size_t Person::hashImpl() const {
    // TODO: 返回 name 和 age 的组合哈希
    return 0;
}

// ==================== Vec 实现 ====================

Vec::Vec(size_t n) : data_(n) {}

Vec::Vec(std::initializer_list<double> list) : data_(list) {}

double Vec::operator[](size_t i) const {
    return data_[i];
}

double& Vec::operator[](size_t i) {
    return data_[i];
}

size_t Vec::size() const {
    return data_.size();
}

// 显式模板实例化
template class ObjectCounter<Entity>;
template class ObjectCounter<Component>;

} // namespace CRTPImpl
