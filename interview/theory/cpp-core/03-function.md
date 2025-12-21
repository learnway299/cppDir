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

## 6. 函数指针 vs 指针函数

这是两个完全不同的概念，面试中经常被问到：

| 概念 | 含义 | 示例 |
|-----|------|------|
| **函数指针** | 指向函数的指针 | `int (*p)(int)` |
| **指针函数** | 返回指针的函数 | `int* func(int)` |

### 6.1 函数指针（Pointer to Function）

```cpp
// 声明：返回类型 (*指针名)(参数列表)
int (*funcPtr)(int, int);

int add(int a, int b) { return a + b; }

funcPtr = add;               // 赋值
int result = funcPtr(1, 2);  // 调用
```

### 6.2 指针函数（Function Returning Pointer）

```cpp
// 声明：返回类型* 函数名(参数列表)
int* getPointer(int index) {
    static int arr[] = {10, 20, 30};
    return &arr[index];
}

int* p = getPointer(1);  // p 指向 arr[1]
```

### 6.3 如何区分？看 `*` 的位置

```cpp
int  (*p)(int);    // 函数指针：*p 先结合，p 是指针，指向 int(int) 函数
int*  f(int);      // 指针函数：f(int) 先结合，f 是函数，返回 int*
```

### 6.4 复杂声明技巧

```cpp
// 返回函数指针的函数（指针函数 + 函数指针）
int (*getOperation(char op))(int, int) {
    if (op == '+') return add;
    if (op == '-') return subtract;
    return nullptr;
}

// 用 using 简化
using BinaryOp = int (*)(int, int);
BinaryOp getOperation(char op);  // 更清晰
```

---

## 7. 函数对象 vs Lambda

### 7.1 本质：Lambda 就是编译器生成的函数对象

```cpp
// 手写函数对象
struct Adder {
    int value;
    Adder(int v) : value(v) {}
    int operator()(int x) const { return x + value; }
};

// Lambda 表达式
int value = 10;
auto adder = [value](int x) { return x + value; };

// 编译器将 lambda 转换为类似这样的匿名类：
// struct __lambda_1 {
//     int value;  // 捕获的变量成为成员
//     __lambda_1(int v) : value(v) {}
//     int operator()(int x) const { return x + value; }
// };
```

### 7.2 对比表

| 特性 | 函数对象 (Functor) | Lambda |
|-----|-------------------|--------|
| **定义方式** | 显式定义类 + `operator()` | `[](){}` 语法 |
| **类型** | 类名 | 唯一匿名类型 |
| **状态存储** | 成员变量 | 捕获列表 |
| **可复用性** | 可多次实例化 | 通常一次性使用 |
| **可读性** | 复杂逻辑更清晰 | 简短逻辑更简洁 |
| **模板友好** | 是 | 是（C++14 泛型 lambda） |

### 7.3 代码对比

```cpp
#include <algorithm>
#include <vector>

std::vector<int> nums = {1, 2, 3, 4, 5};

// 方式1：函数对象
struct GreaterThan {
    int threshold;
    GreaterThan(int t) : threshold(t) {}
    bool operator()(int x) const { return x > threshold; }
};
auto it1 = std::find_if(nums.begin(), nums.end(), GreaterThan(3));

// 方式2：Lambda（更简洁）
int threshold = 3;
auto it2 = std::find_if(nums.begin(), nums.end(),
                        [threshold](int x) { return x > threshold; });
```

### 7.4 何时用函数对象？

1. **复杂逻辑**：多个方法、需要继承
2. **需要复用**：多处使用相同的可调用对象
3. **需要类型名**：模板参数、类型别名

```cpp
// 复杂函数对象示例
struct Comparator {
    bool ascending;
    Comparator(bool asc = true) : ascending(asc) {}

    bool operator()(int a, int b) const {
        return ascending ? (a < b) : (a > b);
    }

    void toggle() { ascending = !ascending; }
};

std::set<int, Comparator> mySet(Comparator(false));  // 需要类型名
```

---

## 8. std::function 的本质

### 8.1 定义

**std::function 是类型擦除 (Type Erasure) 的可调用对象包装器**

```cpp
#include <functional>

// std::function 可以存储任意可调用对象
std::function<int(int, int)> func;

func = [](int a, int b) { return a + b; };           // lambda
func = std::plus<int>();                              // 函数对象
func = add;                                           // 普通函数
func = std::bind(&MyClass::method, &obj, _1, _2);    // 成员函数
```

### 8.2 内部实现原理（简化版）

```cpp
template<typename R, typename... Args>
class function<R(Args...)> {
    // 基类：定义虚函数接口
    struct CallableBase {
        virtual R invoke(Args...) = 0;
        virtual ~CallableBase() = default;
    };

    // 派生类模板：存储具体的可调用对象
    template<typename F>
    struct CallableImpl : CallableBase {
        F func_;
        CallableImpl(F f) : func_(std::move(f)) {}
        R invoke(Args... args) override {
            return func_(std::forward<Args>(args)...);
        }
    };

    std::unique_ptr<CallableBase> callable_;  // 多态指针

public:
    template<typename F>
    function(F f) : callable_(new CallableImpl<F>(std::move(f))) {}

    R operator()(Args... args) {
        return callable_->invoke(std::forward<Args>(args)...);
    }
};
```

### 8.3 代价

| 特性 | 说明 |
|-----|------|
| 堆分配 | 通常需要动态内存分配（小对象优化可避免） |
| 虚函数调用 | 每次调用有虚表查找开销 |
| 无法内联 | 编译器无法优化掉调用开销 |

### 8.4 何时使用 std::function？

```cpp
// ✅ 需要存储不同类型的可调用对象
class EventSystem {
    std::map<std::string, std::function<void()>> handlers_;
public:
    void on(const std::string& event, std::function<void()> handler) {
        handlers_[event] = handler;
    }
};

// ✅ 作为函数参数，接受任意可调用对象
void process(std::function<int(int)> transformer);

// ❌ 不需要类型擦除时，用 auto 或模板更高效
template<typename F>
void processEfficient(F transformer);  // 可内联，零开销
```

**结论**：`std::function` 用多态 + 类型擦除实现"存储任意可调用对象"，代价是运行时开销。优先使用 `auto` 或模板，需要类型擦除时才用 `std::function`。

---

## 9. 闭包 (Closure)

### 9.1 定义

**闭包 = 函数 + 捕获的外部环境（变量）**

闭包是一个函数，它能够"记住"并访问其定义时所在作用域的变量，即使该函数在其原始作用域之外执行。

### 9.2 C++ 中的闭包：带捕获的 Lambda

```cpp
#include <iostream>
#include <functional>

std::function<int()> makeCounter() {
    int count = 0;  // 局部变量

    // Lambda 捕获了 count，形成闭包
    return [count]() mutable {
        return ++count;
    };
}

int main() {
    auto counter1 = makeCounter();
    auto counter2 = makeCounter();

    std::cout << counter1() << std::endl;  // 1
    std::cout << counter1() << std::endl;  // 2
    std::cout << counter1() << std::endl;  // 3

    std::cout << counter2() << std::endl;  // 1（独立的闭包）

    return 0;
}
```

**关键点**：
- `makeCounter()` 返回后，局部变量 `count` 本应销毁
- 但 Lambda 捕获了 `count` 的副本，延续了它的"生命"
- 每次调用 `makeCounter()` 都创建独立的闭包

### 9.3 闭包的本质（编译器视角）

Lambda 闭包被编译器转换为**匿名函数对象**：

```cpp
// 你写的 Lambda
int x = 10;
auto lambda = [x](int y) { return x + y; };

// 编译器生成的等价代码
struct __ClosureType {
    int x;  // 捕获的变量成为成员

    __ClosureType(int x_) : x(x_) {}

    int operator()(int y) const {
        return x + y;
    }
};

__ClosureType lambda(x);  // 构造时捕获 x
```

### 9.4 闭包 vs 普通函数 vs Lambda

| 特性 | 普通函数 | 无捕获 Lambda | 有捕获 Lambda（闭包） |
|-----|---------|--------------|-------------------|
| 访问外部变量 | ❌ 不能 | ❌ 不能 | ✅ 可以 |
| 携带状态 | ❌ 无 | ❌ 无 | ✅ 有 |
| 转换为函数指针 | ✅ 本身就是 | ✅ 可以 | ❌ 不能 |
| 本质 | 代码地址 | 空类 + operator() | 类 + 成员变量 + operator() |

```cpp
// 普通函数：无法访问外部变量
int multiplier = 10;
int multiply(int x) {
    // return x * multiplier;  // 只能访问全局变量
    return x * 2;
}

// 无捕获 Lambda：等价于普通函数
auto lambda1 = [](int x) { return x * 2; };
int (*ptr)(int) = lambda1;  // ✅ 可转换为函数指针

// 有捕获 Lambda（闭包）：携带状态
int factor = 10;
auto lambda2 = [factor](int x) { return x * factor; };
// int (*ptr2)(int) = lambda2;  // ❌ 编译错误
std::function<int(int)> func = lambda2;  // ✅ 需要 std::function
```

### 9.5 闭包的经典应用

#### 工厂函数 / 柯里化

```cpp
// 创建乘法器
auto makeMultiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

auto double_it = makeMultiplier(2);
auto triple_it = makeMultiplier(3);

std::cout << double_it(5) << std::endl;  // 10
std::cout << triple_it(5) << std::endl;  // 15
```

#### 回调中保持状态

```cpp
void processAsync(std::function<void(int)> callback) {
    // 模拟异步处理
    callback(42);
}

void example() {
    std::string prefix = "Result: ";

    processAsync([prefix](int result) {
        std::cout << prefix << result << std::endl;  // "Result: 42"
    });
}
```

#### 延迟执行

```cpp
std::vector<std::function<void()>> tasks;

void scheduleTask(int id, const std::string& name) {
    tasks.push_back([id, name]() {
        std::cout << "Task " << id << ": " << name << std::endl;
    });
}

// 稍后执行
for (auto& task : tasks) {
    task();
}
```

### 9.6 闭包的陷阱：悬空引用

```cpp
std::function<int()> createBadClosure() {
    int local = 100;

    // ❌ 危险：按引用捕获局部变量
    return [&local]() {
        return local;  // local 已销毁，未定义行为！
    };
}

std::function<int()> createGoodClosure() {
    int local = 100;

    // ✅ 安全：按值捕获
    return [local]() {
        return local;
    };
}
```

**规则**：
- 闭包生命周期 > 被捕获变量 → **按值捕获**
- 闭包生命周期 ≤ 被捕获变量 → 按引用捕获可以

### 9.7 闭包总结

| 问题 | 答案 |
|-----|------|
| **闭包是什么** | 函数 + 它捕获的外部变量环境 |
| **C++ 如何实现** | 带捕获的 Lambda（编译为函数对象 + 成员变量） |
| **核心特征** | 能访问定义时作用域的变量，即使离开该作用域 |
| **与 Lambda 关系** | 有捕获的 Lambda = 闭包；无捕获的 Lambda ≠ 闭包 |

---

## 10. 调用约定 (Calling Convention)

### 10.1 什么是调用约定？

**问题描述**：调用约定定义了函数调用时的底层规则，包括参数如何传递、返回值如何获取、以及谁负责清理栈。

**核心问题**：
- 参数传递顺序（从左到右 or 从右到左）
- 参数传递方式（栈 or 寄存器）
- **栈清理责任**（调用者 or 被调用者）
- 名称修饰（Name Mangling）规则

### 10.2 常见调用约定对比

| 调用约定 | 参数传递顺序 | 栈清理者 | 特点 | 使用场景 |
|---------|-------------|---------|------|---------|
| **cdecl** | 右→左 | 调用者 (caller) | C/C++ 默认，支持变参 | C/C++ 函数 |
| **stdcall** | 右→左 | 被调用者 (callee) | 代码更紧凑 | Windows API |
| **fastcall** | 前2个用寄存器 | 被调用者 (callee) | 速度更快 | 性能关键代码 |
| **thiscall** | 右→左，this 用 ECX | 被调用者 (callee) | C++ 成员函数 | MSVC 成员函数 |
| **vectorcall** | 向量用 SIMD 寄存器 | 被调用者 (callee) | SIMD 优化 | 数学/图形库 |

### 10.3 cdecl — C/C++ 默认（调用者清理）

**代码示例**：
```cpp
// 显式声明 cdecl（通常可省略）
int __cdecl add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(10, 20);
    return 0;
}
```

**汇编伪代码**（x86）：
```asm
; 调用 add(10, 20)
push 20         ; 参数从右向左入栈
push 10
call add
add esp, 8      ; 调用者负责清理栈（2个int = 8字节）
```

**为什么调用者清理？**
- 支持 **变参函数**（如 `printf`）— 只有调用者知道传了多少参数
- 被调用者无法知道变参的实际数量

```cpp
// 变参函数必须使用 cdecl
int __cdecl myprintf(const char* fmt, ...) {
    // ...
}

myprintf("%d %d %d", 1, 2, 3);  // 调用者知道传了4个参数
```

### 10.4 stdcall — Windows API（被调用者清理）

**代码示例**：
```cpp
// Windows API 风格
int __stdcall WinAdd(int a, int b) {
    return a + b;
}
```

**汇编伪代码**（x86）：
```asm
; 调用 WinAdd(10, 20)
push 20
push 10
call WinAdd     ; 函数内部使用 ret 8 清理栈
                ; 调用者不需要 add esp, 8
```

**优点**：
- 每个调用点少一条 `add esp, N` 指令
- 生成的代码更紧凑

**缺点**：
- **不支持变参函数**（被调用者不知道参数数量）

### 10.5 fastcall — 寄存器传参

**代码示例**：
```cpp
// 前两个参数通过寄存器传递（ECX, EDX）
int __fastcall fastAdd(int a, int b) {
    return a + b;
}
```

**特点**：
- 前 2 个整数参数用 ECX、EDX 传递
- 其余参数从右向左入栈
- 被调用者清理栈

### 10.6 thiscall — C++ 成员函数

**问题描述**：C++ 非静态成员函数如何传递 `this` 指针？

**MSVC 实现**：
```cpp
class MyClass {
public:
    int getValue(int x) {  // 隐含 __thiscall
        return value_ + x;
    }
private:
    int value_ = 10;
};
```

**调用约定**：
- `this` 指针通过 **ECX 寄存器**传递
- 其他参数从右向左入栈
- 被调用者清理栈

**GCC/Clang 实现**：
- `this` 作为第一个隐藏参数入栈
- 使用 cdecl 规则

### 10.7 x64 调用约定

在 **64位架构**下，调用约定更加统一：

#### Windows x64 (Microsoft x64)

| 参数位置 | 整数/指针 | 浮点数 |
|---------|----------|--------|
| 第1个 | RCX | XMM0 |
| 第2个 | RDX | XMM1 |
| 第3个 | R8 | XMM2 |
| 第4个 | R9 | XMM3 |
| 第5个及以后 | 栈 | 栈 |

- **Shadow Space**：调用者必须分配 32 字节的栈空间（即使参数用寄存器传递）
- **栈清理**：**调用者**负责

#### System V AMD64 (Linux/macOS)

| 参数位置 | 整数/指针 | 浮点数 |
|---------|----------|--------|
| 第1-6个 | RDI, RSI, RDX, RCX, R8, R9 | XMM0-XMM7 |
| 第7个及以后 | 栈 | 栈 |

- 无 Shadow Space
- **栈清理**：**调用者**负责

**代码示例**：
```cpp
// x64 下调用约定差异
long func(long a, long b, long c, long d, long e, long f, long g);

// Windows x64: a=RCX, b=RDX, c=R8, d=R9, e/f/g=栈
// Linux x64:   a=RDI, b=RSI, c=RDX, d=RCX, e=R8, f=R9, g=栈
```

### 10.8 调用约定导致的问题

#### 问题1：跨模块调用约定不匹配

```cpp
// DLL 中声明（stdcall）
extern "C" __declspec(dllexport) int __stdcall DllFunc(int a, int b);

// 调用方错误使用（cdecl）
typedef int (*FuncPtr)(int, int);  // 默认 cdecl
FuncPtr ptr = (FuncPtr)GetProcAddress(hDll, "DllFunc");
ptr(1, 2);  // 栈不平衡！崩溃或数据损坏
```

**解决方案**：
```cpp
// 正确声明
typedef int (__stdcall *FuncPtr)(int, int);
```

#### 问题2：C++ 与 C 混合编程

```cpp
// C 头文件
#ifdef __cplusplus
extern "C" {
#endif

void c_function(int x);  // 使用 C 链接

#ifdef __cplusplus
}
#endif
```

### 10.9 面试常见问题

#### Q1: C/C++ 默认谁清理栈？为什么？

**答**：**调用者**清理（cdecl）。因为需要支持变参函数（如 `printf`），只有调用者知道传递了多少参数。

#### Q2: stdcall 和 cdecl 的主要区别？

**答**：
| 特性 | cdecl | stdcall |
|-----|-------|---------|
| 栈清理 | 调用者 | 被调用者 |
| 变参支持 | ✅ 支持 | ❌ 不支持 |
| 代码大小 | 稍大 | 更紧凑 |
| 使用场景 | C/C++ 默认 | Windows API |

#### Q3: x64 和 x86 调用约定有何不同？

**答**：
- x86：多种约定并存（cdecl, stdcall, fastcall, thiscall）
- x64：基本统一，前 4-6 个参数用寄存器传递，调用者清理栈
- x64 更高效：减少栈操作，充分利用寄存器

#### Q4: 为什么成员函数指针不能直接转换为普通函数指针？

**答**：成员函数需要传递 `this` 指针，其调用约定（thiscall）与普通函数（cdecl）不同。成员函数指针实际上包含了函数地址和 `this` 偏移等信息。

#### Q5: 如何在 C++ 中指定调用约定？

**答**：
```cpp
// Windows 平台
int __cdecl func1(int x);      // C/C++ 默认
int __stdcall func2(int x);    // Windows API 风格
int __fastcall func3(int x);   // 寄存器传参

// 跨平台宏定义
#ifdef _WIN32
    #define CALLBACK __stdcall
#else
    #define CALLBACK
#endif
```

### 10.10 调用约定速查表

**x86 平台**：

| 约定 | 参数顺序 | 栈清理 | 寄存器参数 | 变参 |
|-----|---------|--------|-----------|------|
| cdecl | 右→左 | 调用者 | 无 | ✅ |
| stdcall | 右→左 | 被调用者 | 无 | ❌ |
| fastcall | 右→左 | 被调用者 | ECX, EDX | ❌ |
| thiscall | 右→左 | 被调用者 | ECX(this) | ❌ |

**x64 平台**：

| 平台 | 整数参数寄存器 | 浮点寄存器 | 栈清理 |
|-----|---------------|-----------|--------|
| Windows | RCX,RDX,R8,R9 | XMM0-3 | 调用者 |
| Linux/macOS | RDI,RSI,RDX,RCX,R8,R9 | XMM0-7 | 调用者 |

---

## 11. 程序执行顺序

### 11.1 main 是第一个/最后一个执行的函数吗？

**答案：都不是。**

```
程序启动
    │
    ├─→ 1. 全局/静态对象的构造函数（按定义顺序）
    │
    ├─→ 2. __attribute__((constructor)) 函数（GCC/Clang）
    │
    ├─→ 3. main() 函数
    │
    ├─→ 4. atexit() 注册的函数（逆序执行）
    │
    ├─→ 5. 全局/静态对象的析构函数（逆序）
    │
    ├─→ 6. __attribute__((destructor)) 函数（GCC/Clang）
    │
    └─→ 程序结束
```

### 11.2 代码验证

```cpp
#include <iostream>
#include <cstdlib>

// 1. 全局对象
class Global {
public:
    Global() { std::cout << "1. Global 构造\n"; }
    ~Global() { std::cout << "5. Global 析构\n"; }
};

Global g;  // 全局对象

// 2. GCC/Clang 特有的 constructor 属性
__attribute__((constructor))
void beforeMain() {
    std::cout << "2. __attribute__((constructor))\n";
}

// 6. GCC/Clang 特有的 destructor 属性
__attribute__((destructor))
void afterMain() {
    std::cout << "6. __attribute__((destructor))\n";
}

// 4. atexit 注册的函数
void atexitFunc1() { std::cout << "4a. atexit 函数1\n"; }
void atexitFunc2() { std::cout << "4b. atexit 函数2\n"; }

// 3. main 函数
int main() {
    std::cout << "3. main() 开始\n";

    std::atexit(atexitFunc1);  // 注册 atexit（逆序执行）
    std::atexit(atexitFunc2);

    std::cout << "3. main() 结束\n";
    return 0;
}
```

**输出**：
```
1. Global 构造
2. __attribute__((constructor))
3. main() 开始
3. main() 结束
4b. atexit 函数2
4a. atexit 函数1
5. Global 析构
6. __attribute__((destructor))
```

### 11.3 main 之前执行的内容

| 顺序 | 内容 | 说明 |
|-----|------|------|
| 1 | CRT 初始化 | C Runtime 初始化（设置堆、栈等） |
| 2 | 全局对象构造 | 按**定义顺序**（跨编译单元顺序未定义） |
| 3 | `__attribute__((constructor))` | GCC/Clang 扩展 |
| 4 | `DllMain` (Windows) | DLL 加载时 |

### 11.4 main 之后执行的内容

| 顺序 | 内容 | 说明 |
|-----|------|------|
| 1 | `atexit()` 注册的函数 | **逆序**执行（LIFO） |
| 2 | 局部静态对象析构 | 逆序 |
| 3 | 全局对象析构 | **逆序**（与构造相反） |
| 4 | `__attribute__((destructor))` | GCC/Clang 扩展 |
| 5 | CRT 清理 | 释放资源 |

### 11.5 静态初始化顺序问题 (Static Initialization Order Fiasco)

```cpp
// file1.cpp
class A {
public:
    A() { std::cout << "A 构造\n"; }
};
A a;

// file2.cpp
class B {
public:
    B() { std::cout << "B 构造\n"; }
};
B b;

// 问题：a 和 b 谁先构造？
// 答案：未定义！取决于链接顺序
```

**解决方案：Construct On First Use**

```cpp
// 使用函数内静态变量，保证初始化顺序
A& getA() {
    static A a;  // 首次调用时构造
    return a;
}

B& getB() {
    static B b;
    return b;
}
```

### 11.6 如何在 main 之前/之后执行代码

**main 之前**：
```cpp
// 方法1：全局对象构造函数
class BeforeMain {
public:
    BeforeMain() { /* 在 main 之前执行 */ }
} beforeMain;

// 方法2：GCC/Clang 扩展
__attribute__((constructor))
void init() { /* 在 main 之前执行 */ }
```

**main 之后**：
```cpp
// 方法1：atexit
std::atexit(cleanup);

// 方法2：全局对象析构函数
class AfterMain {
public:
    ~AfterMain() { /* 在 main 之后执行 */ }
} afterMain;

// 方法3：GCC/Clang 扩展
__attribute__((destructor))
void cleanup() { /* 在 main 之后执行 */ }
```

### 11.7 执行顺序速查表

| 问题 | 答案 |
|-----|------|
| **第一个执行的函数** | CRT 初始化 / 全局对象构造函数 |
| **main 的位置** | 中间（全局构造之后，全局析构之前） |
| **最后执行的函数** | 全局对象析构 / `__attribute__((destructor))` |
| **atexit 执行顺序** | 逆序（后注册先执行） |
| **跨编译单元全局对象顺序** | **未定义**（需要特殊处理） |
| **同编译单元全局对象顺序** | 按定义顺序构造，逆序析构 |

---

## 12. 面试高频问题

### Q1: 函数重载的匹配规则是什么？
**答**：精确匹配 > 提升匹配 > 标准转换 > 用户定义转换 > 省略号匹配。

### Q2: 默认参数和重载的区别？
**答**：默认参数在调用时可省略，编译器填充默认值；重载是多个不同函数。默认参数可能与重载产生歧义。

### Q7: main 是第一个执行的函数吗？
**答**：不是。全局/静态对象的构造函数在 main 之前执行。

### Q8: main 是最后一个执行的函数吗？
**答**：不是。`atexit` 注册的函数、全局/静态对象的析构函数在 main 之后执行。

### Q9: 全局对象的构造和析构顺序？
**答**：同一编译单元按定义顺序构造、逆序析构；跨编译单元顺序**未定义**（Static Initialization Order Fiasco）。

### Q3: 函数指针和函数对象有何区别？
**答**：函数指针不能内联优化，不能携带状态；函数对象可以内联，可以有成员变量存储状态。

### Q4: Lambda 的本质是什么？
**答**：Lambda 是编译器生成的匿名函数对象（仿函数），捕获列表对应成员变量，函数体对应 `operator()`。

### Q5: Lambda 按值捕获和按引用捕获的区别？
**答**：按值捕获会复制变量（默认 const），按引用捕获直接引用外部变量（可修改）。按值捕获需注意生命周期，按引用需注意悬空引用。

### Q6: 为什么有捕获的 lambda 不能转换为函数指针？
**答**：函数指针只能指向无状态的函数，而有捕获的 lambda 包含状态（捕获的变量），需要用 `std::function` 存储。

---

## 13. 最佳实践

1. **优先使用 lambda**：现代 C++ 中，lambda 比函数指针更安全、更简洁
2. **避免悬空引用**：按引用捕获时确保 lambda 生命周期不超过被捕获变量
3. **谨慎使用 [&] 和 [=]**：显式捕获更清晰，避免意外捕获 this 指针
4. **mutable 慎用**：大多数情况下按引用捕获更合理
5. **函数重载要清晰**：避免过度重载导致二义性，优先使用不同函数名
6. **默认参数从右向左**：保持参数顺序的灵活性
7. **std::function 谨慎使用**：有运行时开销，优先用 `auto` 或模板
8. **闭包注意生命周期**：返回闭包时按值捕获，避免悬空引用

---

