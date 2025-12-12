# 03 - 函数 (Function)

## 简介

本文档涵盖 C++ 函数相关的核心概念，包括函数重载、默认参数、函数指针和 lambda 表达式。这些是 C++ 面试中的高频考点。

---

## 1. 函数重载 (Function Overloading)

### 1.1 什么是函数重载？

**问题描述**：函数重载允许在同一作用域内定义多个同名函数，但参数列表必须不同（参数个数、类型或顺序）。编译器根据调用时的实参类型和个数来选择最匹配的函数。

**代码示例**：
```cpp
#include <iostream>
using namespace std;

// 重载：参数个数不同
void print(int a) {
    cout << "int: " << a << endl;
}

void print(int a, int b) {
    cout << "int, int: " << a << ", " << b << endl;
}

// 重载：参数类型不同
void print(double a) {
    cout << "double: " << a << endl;
}

void print(const char* s) {
    cout << "string: " << s << endl;
}

int main() {
    print(10);           // 调用 print(int)
    print(10, 20);       // 调用 print(int, int)
    print(3.14);         // 调用 print(double)
    print("hello");      // 调用 print(const char*)
    return 0;
}
```

**关键要点**：
- 函数重载是编译期多态（静态多态）
- 仅返回类型不同不能构成重载
- 重载决议（overload resolution）遵循最佳匹配原则
- const 修饰符可以构成重载（如 `void f(int&)` vs `void f(const int&)`）

**常见陷阱**：
```cpp
// 错误示例：仅返回类型不同
int getValue();
double getValue();  // 编译错误

// 易混淆：默认参数与重载
void func(int a, int b = 0);
void func(int a);  // 编译错误：调用 func(10) 产生歧义

// 隐式类型转换导致的歧义
void foo(int);
void foo(double);
foo('a');  // OK：char 隐式转换为 int
```

---

### 1.2 重载决议规则

**问题描述**：当有多个重载函数时，编译器如何选择最佳匹配？

**匹配优先级**（从高到低）：
1. **精确匹配**：参数类型完全相同
2. **提升匹配**：整型提升（char → int）、float → double
3. **标准转换匹配**：int → double、派生类指针 → 基类指针
4. **用户定义转换**：通过构造函数或转换运算符
5. **省略号匹配**：`void func(...)`

**代码示例**：
```cpp
void test(int);           // #1
void test(double);        // #2
void test(char);          // #3

test(10);      // 精确匹配 #1
test(3.14);    // 精确匹配 #2
test('a');     // 精确匹配 #3
test(3.14f);   // 提升匹配 #2 (float → double)

// 二义性示例
void ambiguous(int, double);
void ambiguous(double, int);
ambiguous(1, 2);  // 编译错误：两个函数都需要一次标准转换
```

---

## 2. 默认参数 (Default Arguments)

### 2.1 基本用法

**问题描述**：默认参数允许函数调用时省略某些参数，编译器会使用预设的默认值。

**代码示例**：
```cpp
#include <iostream>
using namespace std;

// 默认参数必须从右向左连续
void createWindow(int width = 800, int height = 600, bool fullscreen = false) {
    cout << "Width: " << width
         << ", Height: " << height
         << ", Fullscreen: " << fullscreen << endl;
}

int main() {
    createWindow();                  // 使用所有默认值
    createWindow(1024);              // width=1024, 其余默认
    createWindow(1920, 1080);        // 指定前两个参数
    createWindow(1920, 1080, true);  // 指定所有参数
    return 0;
}
```

**关键要点**：
- 默认参数只能从右向左连续设置
- 默认参数只能在声明或定义中指定一次（通常在头文件声明中）
- 默认参数在编译时展开，不是运行时特性

**常见陷阱**：
```cpp
// 错误示例：默认参数不连续
void func(int a = 10, int b, int c = 20);  // 编译错误

// 错误示例：重复定义默认参数
// header.h
void foo(int a = 10);

// source.cpp
void foo(int a = 20) {  // 编译错误
    // ...
}

// 正确做法：只在声明中指定
// header.h
void foo(int a = 10);

// source.cpp
void foo(int a) {  // 定义时不重复默认值
    // ...
}
```

---

### 2.2 默认参数的求值时机

**问题描述**：默认参数表达式何时求值？

**代码示例**：
```cpp
int global = 100;

void func(int x = global) {
    cout << x << endl;
}

int main() {
    func();        // 输出 100
    global = 200;
    func();        // 输出 200（默认参数在调用时求值）
    return 0;
}
```

**关键要点**：
- 默认参数表达式在每次函数调用时求值（而非定义时）
- 默认参数可以使用之前声明的参数

```cpp
// 默认参数引用前面的参数
int multiply(int a, int b = a * 2) {  // b 的默认值依赖 a
    return a * b;
}

multiply(5);     // 等价于 multiply(5, 10)
multiply(3, 4);  // 直接指定
```

---

## 3. 函数指针 (Function Pointer)

### 3.1 基本用法

**问题描述**：函数指针是指向函数的指针变量，可以用于回调、策略模式等场景。

**代码示例**：
```cpp
#include <iostream>
using namespace std;

// 普通函数
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

int main() {
    // 声明函数指针：返回类型 (*指针名)(参数列表)
    int (*operation)(int, int);

    operation = add;
    cout << operation(10, 5) << endl;  // 15

    operation = subtract;
    cout << operation(10, 5) << endl;  // 5

    operation = &multiply;  // 取地址符可选
    cout << (*operation)(10, 5) << endl;  // 50，解引用可选

    return 0;
}
```

**关键要点**：
- 函数名会自动转换为函数指针（可选使用 `&`）
- 调用函数指针时可以直接用 `ptr(args)` 或 `(*ptr)(args)`
- 函数指针类型必须与目标函数签名完全匹配

---

### 3.2 typedef 和 using 简化声明

**代码示例**：
```cpp
// 复杂的函数指针声明
int (*callback)(int, int);

// 使用 typedef 简化
typedef int (*BinaryOp)(int, int);
BinaryOp op1 = add;
BinaryOp op2 = subtract;

// C++11: 使用 using（更清晰）
using BinaryOp = int (*)(int, int);
BinaryOp op3 = multiply;

// 函数指针数组
BinaryOp operations[] = {add, subtract, multiply};
for (int i = 0; i < 3; ++i) {
    cout << operations[i](10, 5) << endl;
}
```

---

### 3.3 函数指针作为参数（回调函数）

**代码示例**：
```cpp
#include <iostream>
#include <vector>
using namespace std;

// 回调函数：对数组每个元素执行操作
void forEach(vector<int>& arr, void (*callback)(int&)) {
    for (auto& elem : arr) {
        callback(elem);
    }
}

// 具体的回调实现
void doubleValue(int& x) { x *= 2; }
void printValue(int& x) { cout << x << " "; }

int main() {
    vector<int> nums = {1, 2, 3, 4, 5};

    forEach(nums, doubleValue);  // 每个元素 *2
    forEach(nums, printValue);   // 输出：2 4 6 8 10

    return 0;
}
```

**常见陷阱**：
```cpp
// 成员函数指针与普通函数指针不兼容
class MyClass {
public:
    void memberFunc(int x) {}
    static void staticFunc(int x) {}
};

void (*ptr1)(int) = &MyClass::staticFunc;  // OK：静态成员函数
void (*ptr2)(int) = &MyClass::memberFunc;  // 编译错误：非静态成员函数

// 成员函数指针需要特殊语法
void (MyClass::*ptr3)(int) = &MyClass::memberFunc;  // OK
MyClass obj;
(obj.*ptr3)(10);  // 调用成员函数指针
```

---

### 3.4 回调机制深度解析

**问题描述**：回调的本质是什么？什么函数可以作为回调？成员函数能否作为回调？

#### 3.4.1 回调的本质

**回调本质**：回调是一种控制反转（Inversion of Control）机制，将函数作为参数传递，让被调用方在适当时机调用传入的函数。

**核心概念**：
- **同步回调**：在函数调用过程中立即执行（如 STL 算法）
- **异步回调**：在未来某个时刻执行（如事件处理、定时器）
- **回调的生命周期管理**：确保回调对象在被调用时仍然有效

**代码示例**：
```cpp
#include <iostream>
#include <functional>
#include <vector>
using namespace std;

// 同步回调：立即执行
void processData(int data, function<void(int)> callback) {
    // 处理数据...
    callback(data * 2);  // 立即调用回调
}

// 异步回调：模拟延迟执行
class Timer {
    function<void()> callback_;
public:
    void setCallback(function<void()> cb) {
        callback_ = cb;
    }

    void trigger() {
        if (callback_) {
            callback_();  // 在未来某个时刻执行
        }
    }
};

int main() {
    // 同步回调
    processData(10, [](int result) {
        cout << "Result: " << result << endl;  // 20
    });

    // 异步回调
    Timer timer;
    timer.setCallback([]() {
        cout << "Timer triggered!" << endl;
    });
    timer.trigger();

    return 0;
}
```

---

#### 3.4.2 什么函数可以作为回调？

C++ 中，**所有可调用对象（Callable）** 都可以作为回调：

| 类型 | 示例 | 特点 | 适用场景 |
|------|------|------|----------|
| **普通函数** | `void func(int)` | 无状态、简单 | C 风格 API |
| **静态成员函数** | `MyClass::staticFunc` | 无状态、类作用域 | 类相关的全局回调 |
| **函数指针** | `void (*ptr)(int)` | 兼容 C | 底层接口、插件系统 |
| **函数对象** | `struct F { void operator()() }` | 有状态、可内联 | 复杂逻辑、性能关键 |
| **Lambda** | `[](int x) { ... }` | 现代、简洁 | 首选方案 |
| **std::function** | `std::function<void(int)>` | 类型擦除 | 需要存储不同类型回调 |
| **std::bind** | `std::bind(&Class::method, obj, _1)` | 绑定参数 | 适配参数不匹配 |
| **成员函数指针** | `void (C::*)(int)` | 需要对象实例 | 面向对象设计 |

**代码示例**：
```cpp
#include <iostream>
#include <functional>
using namespace std;

// 回调接收者：接受任意可调用对象
void executeCallback(function<void(int)> callback, int value) {
    callback(value);
}

// 1. 普通函数
void normalFunc(int x) {
    cout << "Normal function: " << x << endl;
}

// 2. 函数对象
struct Functor {
    void operator()(int x) const {
        cout << "Functor: " << x << endl;
    }
};

class MyClass {
public:
    // 3. 静态成员函数
    static void staticMethod(int x) {
        cout << "Static method: " << x << endl;
    }

    // 4. 非静态成员函数
    void memberMethod(int x) {
        cout << "Member method: " << x << endl;
    }
};

int main() {
    // 1. 普通函数
    executeCallback(normalFunc, 1);

    // 2. 函数对象
    executeCallback(Functor(), 2);

    // 3. Lambda
    executeCallback([](int x) {
        cout << "Lambda: " << x << endl;
    }, 3);

    // 4. 静态成员函数
    executeCallback(&MyClass::staticMethod, 4);

    // 5. 成员函数（需要绑定对象）
    MyClass obj;
    executeCallback(bind(&MyClass::memberMethod, &obj, placeholders::_1), 5);

    // 6. 成员函数（Lambda 包装）
    executeCallback([&obj](int x) {
        obj.memberMethod(x);
    }, 6);

    return 0;
}
```

---

#### 3.4.3 成员函数作为回调的完整解决方案

**核心问题**：非静态成员函数需要 `this` 指针，无法直接转换为普通函数指针。

**解决方案对比**：

```cpp
#include <iostream>
#include <functional>
using namespace std;

class EventHandler {
    int handlerId_;
public:
    EventHandler(int id) : handlerId_(id) {}

    void onEvent(int eventCode) {
        cout << "Handler " << handlerId_ << " received event " << eventCode << endl;
    }

    static void staticHandler(int eventCode) {
        cout << "Static handler received event " << eventCode << endl;
    }
};

// 事件系统
class EventSystem {
    function<void(int)> callback_;
public:
    void registerCallback(function<void(int)> cb) {
        callback_ = cb;
    }

    void triggerEvent(int code) {
        if (callback_) callback_(code);
    }
};

int main() {
    EventSystem eventSys;
    EventHandler handler(123);

    // ❌ 方案1：直接使用成员函数指针（编译错误）
    // eventSys.registerCallback(&EventHandler::onEvent);  // 错误！

    // ✅ 方案2：使用 std::bind
    eventSys.registerCallback(
        bind(&EventHandler::onEvent, &handler, placeholders::_1)
    );
    eventSys.triggerEvent(1001);  // Handler 123 received event 1001

    // ✅ 方案3：使用 Lambda（推荐）
    eventSys.registerCallback([&handler](int code) {
        handler.onEvent(code);
    });
    eventSys.triggerEvent(1002);

    // ✅ 方案4：使用静态成员函数
    eventSys.registerCallback(&EventHandler::staticHandler);
    eventSys.triggerEvent(1003);

    // ✅ 方案5：成员函数指针 + 对象指针（需要自定义封装）
    auto memberCallback = [](EventHandler* obj, int code) {
        obj->onEvent(code);
    };
    eventSys.registerCallback([&handler, memberCallback](int code) {
        memberCallback(&handler, code);
    });
    eventSys.triggerEvent(1004);

    return 0;
}
```

**成员函数指针的底层机制**：
```cpp
#include <iostream>
using namespace std;

class MyClass {
    int value_ = 100;
public:
    void print(int x) {
        cout << "value=" << value_ << ", x=" << x << endl;
    }
};

int main() {
    MyClass obj;

    // 成员函数指针的声明和使用
    void (MyClass::*pMethod)(int) = &MyClass::print;

    // 调用方式1：.*
    (obj.*pMethod)(42);  // value=100, x=42

    // 调用方式2：->*
    MyClass* pObj = &obj;
    (pObj->*pMethod)(99);  // value=100, x=99

    // 使用 std::mem_fn 简化调用
    auto memFunc = mem_fn(&MyClass::print);
    memFunc(obj, 123);  // value=100, x=123

    return 0;
}
```

**工程实践：回调的所有权管理**：
```cpp
#include <iostream>
#include <functional>
#include <memory>
using namespace std;

class Button {
    function<void()> onClick_;
public:
    void setOnClick(function<void()> callback) {
        onClick_ = callback;
    }

    void click() {
        if (onClick_) onClick_();
    }
};

class Window {
    int windowId_;
public:
    Window(int id) : windowId_(id) {
        cout << "Window " << windowId_ << " created" << endl;
    }

    ~Window() {
        cout << "Window " << windowId_ << " destroyed" << endl;
    }

    void onButtonClick() {
        cout << "Window " << windowId_ << " button clicked" << endl;
    }
};

int main() {
    Button btn;

    // ❌ 危险：捕获的引用可能悬空
    {
        Window win(1);
        btn.setOnClick([&win]() {
            win.onButtonClick();
        });
    }  // win 已销毁
    // btn.click();  // 未定义行为！悬空引用

    // ✅ 方案1：使用 shared_ptr 管理生命周期
    auto win2 = make_shared<Window>(2);
    btn.setOnClick([win2]() {  // 按值捕获 shared_ptr
        win2->onButtonClick();
    });
    win2.reset();  // Window 2 仍然存活，因为 lambda 持有引用计数
    btn.click();   // OK

    // ✅ 方案2：使用 weak_ptr 避免循环引用
    auto win3 = make_shared<Window>(3);
    btn.setOnClick([weakWin = weak_ptr<Window>(win3)]() {
        if (auto win = weakWin.lock()) {
            win->onButtonClick();
        } else {
            cout << "Window already destroyed" << endl;
        }
    });
    win3.reset();  // Window 3 立即销毁
    btn.click();   // 输出：Window already destroyed

    return 0;
}
```

**关键要点**：
- 普通函数、静态成员函数、无捕获 Lambda 可直接作为回调
- 成员函数需要绑定对象实例（`std::bind` 或 Lambda 包装）
- 使用 `std::function` 统一不同类型的回调
- 注意回调的生命周期：避免悬空引用、使用智能指针管理
- Lambda 是现代 C++ 的首选方案：简洁、类型安全、易于管理捕获

---

## 4. Lambda 表达式 (Lambda Expression)

### 4.1 基本语法

**问题描述**：Lambda 是 C++11 引入的匿名函数对象，用于简化函数指针和函数对象的使用。

**基本语法**：
```
[capture](parameters) -> return_type { body }
```

**代码示例**：
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    // 最简单的 lambda
    auto hello = []() { cout << "Hello Lambda!" << endl; };
    hello();

    // 带参数的 lambda
    auto add = [](int a, int b) { return a + b; };
    cout << add(3, 5) << endl;  // 8

    // 显式指定返回类型
    auto divide = [](int a, int b) -> double {
        return static_cast<double>(a) / b;
    };
    cout << divide(7, 2) << endl;  // 3.5

    // 配合 STL 算法使用
    vector<int> nums = {1, 2, 3, 4, 5};

    // for_each：打印每个元素
    for_each(nums.begin(), nums.end(), [](int n) {
        cout << n << " ";
    });
    cout << endl;

    // sort：降序排列
    sort(nums.begin(), nums.end(), [](int a, int b) {
        return a > b;
    });

    return 0;
}
```

**关键要点**：
- Lambda 本质是编译器生成的匿名函数对象（functor）
- 返回类型可以自动推导（单一 return 语句）
- Lambda 可以赋值给 `auto`、`std::function` 或函数指针（无捕获时）

---

### 4.2 捕获列表 (Capture)

**问题描述**：Lambda 如何访问外部变量？通过捕获列表指定。

**捕获方式**：
- `[]`：不捕获任何变量
- `[=]`：按值捕获所有外部变量
- `[&]`：按引用捕获所有外部变量
- `[x]`：按值捕获变量 x
- `[&x]`：按引用捕获变量 x
- `[=, &x]`：默认按值，x 按引用
- `[&, x]`：默认按引用，x 按值
- `[this]`：捕获当前对象指针（成员函数中）

**代码示例**：
```cpp
#include <iostream>
using namespace std;

int main() {
    int a = 10, b = 20;

    // 按值捕获
    auto lambda1 = [a, b]() {
        cout << a + b << endl;  // 30
        // a = 100;  // 编译错误：按值捕获的变量是 const
    };
    lambda1();

    // 按引用捕获
    auto lambda2 = [&a, &b]() {
        a = 100;  // OK：可以修改
        b = 200;
    };
    lambda2();
    cout << a << ", " << b << endl;  // 100, 200

    // 混合捕获
    int x = 1, y = 2;
    auto lambda3 = [=, &y]() {  // x 按值，y 按引用
        cout << x << endl;  // 1
        y = 99;             // 修改 y
    };
    lambda3();
    cout << y << endl;  // 99

    // mutable：允许修改按值捕获的变量副本
    int count = 0;
    auto counter = [count]() mutable {
        return ++count;  // 修改的是 lambda 内部的副本
    };
    cout << counter() << endl;  // 1
    cout << counter() << endl;  // 2
    cout << count << endl;      // 0（外部变量未改变）

    return 0;
}
```

**常见陷阱**：
```cpp
// 陷阱1：悬空引用
auto makeLambda() {
    int local = 42;
    return [&local]() { return local; };  // 危险！local 已销毁
}

// 陷阱2：按值捕获指针
int* ptr = new int(100);
auto lambda = [ptr]() { *ptr = 200; };  // 捕获的是指针值，可以修改指向的内容
lambda();  // *ptr 被修改为 200

// 陷阱3：捕获成员变量
class MyClass {
    int value = 10;
public:
    auto getLambda() {
        // 错误理解：[value] 捕获成员变量
        // 实际：捕获的是 this 指针，然后访问 this->value
        return [this]() { return value; };  // 正确写法
        // C++17: return [*this]() { return value; };  // 按值捕获整个对象
    }
};
```

---

### 4.3 Lambda 的类型和存储

**问题描述**：Lambda 表达式的类型是什么？如何存储？

**代码示例**：
```cpp
#include <iostream>
#include <functional>
using namespace std;

int main() {
    // 1. auto：最高效（编译期确定类型）
    auto lambda1 = [](int x) { return x * 2; };
    cout << lambda1(5) << endl;  // 10

    // 2. std::function：类型擦除（有开销）
    function<int(int)> lambda2 = [](int x) { return x * 2; };
    cout << lambda2(5) << endl;  // 10

    // 3. 函数指针（仅限无捕获的 lambda）
    int (*lambda3)(int) = [](int x) { return x * 2; };
    cout << lambda3(5) << endl;  // 10

    // 有捕获的 lambda 无法转换为函数指针
    int factor = 3;
    // int (*lambda4)(int) = [factor](int x) { return x * factor; };  // 编译错误
    function<int(int)> lambda5 = [factor](int x) { return x * factor; };  // OK

    return 0;
}
```

**关键要点**：
- 每个 lambda 表达式都有唯一的类型（即使代码相同）
- 无捕获的 lambda 可隐式转换为函数指针
- `std::function` 可存储任意可调用对象，但有额外开销
- 优先使用 `auto`，需要类型擦除时用 `std::function`

---

### 4.4 Lambda 高级特性

**代码示例**：
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    // 1. 泛型 lambda（C++14）
    auto generic = [](auto a, auto b) { return a + b; };
    cout << generic(1, 2) << endl;       // 3
    cout << generic(1.5, 2.5) << endl;   // 4.0

    // 2. 捕获表达式（C++14）
    auto ptr = make_unique<int>(42);
    auto lambda = [p = move(ptr)]() { return *p; };  // 移动捕获
    cout << lambda() << endl;  // 42

    // 3. constexpr lambda（C++17）
    constexpr auto square = [](int x) { return x * x; };
    static_assert(square(5) == 25);  // 编译期计算

    // 4. 模板 lambda（C++20）
    auto templated = []<typename T>(T a, T b) { return a + b; };
    cout << templated(1, 2) << endl;

    // 5. 递归 lambda（需要 std::function）
    function<int(int)> factorial = [&factorial](int n) -> int {
        return n <= 1 ? 1 : n * factorial(n - 1);
    };
    cout << factorial(5) << endl;  // 120

    return 0;
}
```

**实际应用场景**：
```cpp
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

struct Person {
    string name;
    int age;
};

int main() {
    vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };

    // 按年龄排序
    sort(people.begin(), people.end(), [](const Person& a, const Person& b) {
        return a.age < b.age;
    });

    // 查找年龄大于 30 的人
    auto it = find_if(people.begin(), people.end(), [](const Person& p) {
        return p.age > 30;
    });

    if (it != people.end()) {
        cout << it->name << " is older than 30" << endl;
    }

    // 统计年龄 >= 30 的人数
    int count = count_if(people.begin(), people.end(), [](const Person& p) {
        return p.age >= 30;
    });
    cout << "Count: " << count << endl;

    return 0;
}
```

---

## 5. 对比总结

| 特性 | 函数指针 | 函数对象 (Functor) | Lambda |
|------|----------|-------------------|--------|
| 语法复杂度 | 中等 | 高 | 低 |
| 可内联优化 | 否 | 是 | 是 |
| 可捕获状态 | 否 | 是 | 是 |
| 类型安全 | 弱 | 强 | 强 |
| 适用场景 | C 风格回调 | 复杂逻辑 | 短小回调 |

**代码示例对比**：
```cpp
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

// 1. 函数指针
bool greaterThan10_func(int x) { return x > 10; }

// 2. 函数对象
struct GreaterThan10 {
    bool operator()(int x) const { return x > 10; }
};

int main() {
    vector<int> nums = {5, 12, 8, 15, 3};

    // 使用函数指针
    auto it1 = find_if(nums.begin(), nums.end(), greaterThan10_func);

    // 使用函数对象
    auto it2 = find_if(nums.begin(), nums.end(), GreaterThan10());

    // 使用 lambda（最简洁）
    auto it3 = find_if(nums.begin(), nums.end(), [](int x) { return x > 10; });

    return 0;
}
```

---

## 6. 面试高频问题

### Q1: 函数重载的匹配规则是什么？
**答**：精确匹配 > 提升匹配 > 标准转换 > 用户定义转换 > 省略号匹配。

### Q2: 默认参数和重载的区别？
**答**：默认参数在调用时可省略，编译器填充默认值；重载是多个不同函数。默认参数可能与重载产生歧义。

### Q3: 函数指针和函数对象有何区别？
**答**：函数指针不能内联优化，不能携带状态；函数对象可以内联，可以有成员变量存储状态。

### Q4: Lambda 的本质是什么？
**答**：Lambda 是编译器生成的匿名函数对象（仿函数），捕获列表对应成员变量，函数体对应 `operator()`。

### Q5: Lambda 按值捕获和按引用捕获的区别？
**答**：按值捕获会复制变量（默认 const），按引用捕获直接引用外部变量（可修改）。按值捕获需注意生命周期，按引用需注意悬空引用。

### Q6: 为什么有捕获的 lambda 不能转换为函数指针？
**答**：函数指针只能指向无状态的函数，而有捕获的 lambda 包含状态（捕获的变量），需要用 `std::function` 存储。

---

## 7. 最佳实践

1. **优先使用 lambda**：现代 C++ 中，lambda 比函数指针更安全、更简洁
2. **避免悬空引用**：按引用捕获时确保 lambda 生命周期不超过被捕获变量
3. **谨慎使用 [&] 和 [=]**：显式捕获更清晰，避免意外捕获 this 指针
4. **mutable 慎用**：大多数情况下按引用捕获更合理
5. **函数重载要清晰**：避免过度重载导致二义性，优先使用不同函数名
6. **默认参数从右向左**：保持参数顺序的灵活性

---

