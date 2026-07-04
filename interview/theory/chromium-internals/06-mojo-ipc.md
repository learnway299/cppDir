# 06 - Mojo IPC 机制

## 简介

Mojo 是 Chromium 的进程间通信（IPC）框架，用于 Browser、Renderer、GPU、Utility 等进程之间的通信。它取代了旧的 Chrome IPC 系统，提供类型安全、高性能的跨进程调用能力。

---

## 1. 为什么需要 Mojo？

### 1.1 旧 IPC 的问题

```
旧 Chrome IPC（基于 IPC::Channel）：
├── 手动序列化/反序列化（容易出错）
├── 所有消息走单一通道（瓶颈）
├── 难以扩展新消息类型
├── 不支持共享内存等高级特性
└── 缺乏类型安全
```

### 1.2 Mojo 的优势

```
Mojo：
├── IDL 定义接口（.mojom 文件），自动生成序列化代码
├── 类型安全：编译期检查参数类型
├── 多通道：每个接口有独立的消息管道
├── 支持共享内存（Shared Buffer）
├── 支持平台句柄传递（文件描述符、socket 等）
├── 支持关联接口（Associated Interface，保证消息顺序）
└── 跨语言：支持 C++、Java、JavaScript 绑定
```

---

## 2. Mojo 核心概念

### 2.1 消息管道 (Message Pipe)

```
消息管道是 Mojo 的基础通信原语：

  进程 A                    进程 B
┌─────────┐              ┌─────────┐
│ Handle 0 │←── 管道 ───→│ Handle 1 │
└─────────┘              └─────────┘

特点：
├── 双向通信（全双工）
├── 有序消息传递（FIFO）
├── 支持传递句柄（嵌套管道、共享内存、平台句柄）
├── 跨进程传输（底层通过操作系统 IPC 机制）
└── 进程内也可使用（作为统一的通信抽象）

底层实现：
├── 同进程：直接内存访问
├── 跨进程（同一节点）：共享内存 + eventfd/pipe 通知
└── 跨进程（不同节点）：Unix socket / Windows named pipe
```

### 2.2 Mojom 接口定义

```
// 文件：example.mojom

module example.mojom;

// 定义接口
interface PageLoader {
    // 方法定义：参数类型 → 返回值类型
    Load(string url) => (bool success, string content);

    // 无返回值的方法（单向消息）
    SetTitle(string title);

    // 传递复杂类型
    Configure(PageConfig config);
};

// 结构体
struct PageConfig {
    string user_agent;
    bool enable_javascript;
    int32 timeout_ms;
};

// 枚举
enum LoadError {
    kNone,
    kNetworkError,
    kTimeout,
    kNotFound,
};
```

### 2.3 自动生成的 C++ 代码

```cpp
// mojom 编译器生成的代码（简化）

// 接口定义
class PageLoader {
public:
    virtual ~PageLoader() = default;

    // 异步方法（带回调）
    using LoadCallback = base::OnceCallback<void(bool success, const std::string& content)>;
    virtual void Load(const std::string& url, LoadCallback callback) = 0;

    // 单向方法
    virtual void SetTitle(const std::string& title) = 0;
};

// 远程调用端（调用方）
mojo::Remote<PageLoader> remote;
remote->Load("https://example.com",
    base::BindOnce([](bool success, const std::string& content) {
        if (success) {
            // 处理内容
        }
    }));

// 接收端（实现方）
class PageLoaderImpl : public PageLoader {
    void Load(const std::string& url, LoadCallback callback) override {
        // 实际加载逻辑
        std::move(callback).Run(true, "<html>...</html>");
    }

    void SetTitle(const std::string& title) override {
        // 设置标题
    }
};
```

---

## 3. Mojo 绑定模型

### 3.1 Remote 与 Receiver

```
                 进程 A                           进程 B
          ┌──────────────────┐            ┌──────────────────┐
          │                  │            │                  │
          │  Remote<Foo>     │── 管道 ──→ │  Receiver<Foo>   │
          │  （调用方）       │            │  （实现方）       │
          │                  │            │                  │
          │  remote->Bar()   │            │  FooImpl::Bar()  │
          │                  │            │                  │
          └──────────────────┘            └──────────────────┘

Remote<T>:
  持有消息管道的发送端
  调用接口方法时将参数序列化后发送到管道

Receiver<T>:
  持有消息管道的接收端
  绑定到一个接口实现对象
  收到消息后反序列化并调用对应方法

PendingRemote<T> / PendingReceiver<T>:
  未绑定的管道端点
  可以跨线程/跨进程传递
  传递后再绑定到具体的 Remote/Receiver
```

### 3.2 使用示例

```cpp
// 创建管道
mojo::PendingRemote<mojom::PageLoader> pending_remote;
mojo::PendingReceiver<mojom::PageLoader> pending_receiver =
    pending_remote.InitWithNewPipeAndPassReceiver();

// 在 Browser 进程：绑定 Remote（调用方）
mojo::Remote<mojom::PageLoader> remote;
remote.Bind(std::move(pending_remote));

// 将 PendingReceiver 通过已有通道发送给 Renderer 进程
// ...

// 在 Renderer 进程：绑定 Receiver（实现方）
PageLoaderImpl impl;
mojo::Receiver<mojom::PageLoader> receiver(&impl);
receiver.Bind(std::move(pending_receiver));

// 现在 Browser 可以调用 Renderer 的实现
remote->Load("https://example.com", callback);
```

---

## 4. 高级特性

### 4.1 关联接口 (Associated Interface)

```
问题：两个独立的 Mojo 接口，消息顺序不保证

  Remote<A> ──管道1──→ Receiver<A>
  Remote<B> ──管道2──→ Receiver<B>

  A.Method1() 和 B.Method2() 的到达顺序不确定！

关联接口：共享同一个消息管道，保证顺序

  AssociatedRemote<A> ─┐
                       ├── 共享管道 ──→ 按序到达
  AssociatedRemote<B> ─┘

使用场景：
  Frame 的导航消息必须与文档消息保持顺序
  否则可能出现"旧页面收到新页面的消息"
```

### 4.2 共享内存 (Shared Buffer)

```
大量数据传输时，避免序列化/拷贝开销

// 创建共享内存
mojo::ScopedSharedBufferHandle buffer =
    mojo::SharedBufferHandle::Create(1024 * 1024);  // 1MB

// 映射到进程地址空间
mojo::ScopedSharedBufferMapping mapping = buffer->Map(1024 * 1024);
memcpy(mapping.get(), data, size);

// 通过 Mojo 接口发送句柄给另一个进程
remote->SendData(std::move(buffer));

// 另一个进程映射同一块内存，零拷贝访问
```

### 4.3 数据管道 (Data Pipe)

```
用于流式数据传输（如网络响应体）

  生产者                                    消费者
┌──────────┐                           ┌──────────┐
│ producer │── DataPipe（环形缓冲）──→ │ consumer │
│ handle   │                           │ handle   │
└──────────┘                           └──────────┘

特点：
├── 环形缓冲区（固定大小，如 512KB）
├── 无需序列化，直接写入/读取字节
├── 支持异步通知（有数据可读/有空间可写）
└── 网络响应体就是通过 Data Pipe 从 Network Service 传给 Renderer
```

---

## 5. Chromium 中的 Mojo 使用

```
Browser ↔ Renderer 通信的典型接口：

mojom::FrameHost（Browser 端实现）
├── Renderer 调用 Browser 的方法
├── 如：请求导航、请求权限、创建新窗口
└── Renderer 通过 Remote<FrameHost> 调用

mojom::Frame（Renderer 端实现）
├── Browser 调用 Renderer 的方法
├── 如：提交导航、执行 JS、设置页面状态
└── Browser 通过 Remote<Frame> 调用

mojom::NetworkService（Network Service 实现）
├── 处理所有网络请求
├── Browser 通过 Remote<URLLoaderFactory> 创建请求
└── 响应通过 Data Pipe 流式传输

mojom::GpuService（GPU 进程实现）
├── 处理 GPU 命令
├── 创建 GPU 通道
└── 管理共享纹理
```

---

## 6. 与旧 IPC 的对比

| 维度 | 旧 Chrome IPC | Mojo |
|------|-------------|------|
| 接口定义 | C++ 宏手动定义 | .mojom IDL 自动生成 |
| 类型安全 | 弱（手动序列化） | 强（编译期检查） |
| 通道模型 | 单一 Channel | 多独立管道 |
| 共享内存 | 不直接支持 | 原生支持 |
| 流式传输 | 不支持 | Data Pipe |
| 跨语言 | 仅 C++ | C++ / Java / JS |
| 性能 | 中等 | 更好（减少拷贝） |

---

## 7. 面试常见问题

**Q1：Mojo 是什么？为什么 Chromium 需要它？**

> Mojo 是 Chromium 的 IPC 框架，用于多进程之间的通信。它用 IDL（.mojom）定义接口，自动生成类型安全的序列化/反序列化代码。相比旧的 Chrome IPC，Mojo 提供多通道、共享内存、流式传输、跨语言等能力，更安全高效。

**Q2：Remote 和 Receiver 分别是什么？**

> Remote 是调用方，持有消息管道的发送端，调用接口方法时将消息发送到管道。Receiver 是实现方，持有接收端，绑定到一个实现了接口的对象。收到消息后反序列化参数并调用对应方法。

**Q3：Mojo 消息管道的底层实现？**

> 同进程内是直接方法调用（无序列化开销）。跨进程通过操作系统的 IPC 机制（Linux: Unix domain socket + eventfd，Windows: named pipe）。消息在用户态序列化后写入管道，接收端读取后反序列化。大块数据可用共享内存避免拷贝。

**Q4：什么是关联接口？什么时候需要？**

> 关联接口（Associated Interface）共享同一个消息管道，保证消息有序。当两个接口的消息之间有因果关系时需要使用。例如导航消息和文档消息必须有序，否则可能出现旧页面收到新导航的消息。

**Q5：网络响应体是如何从 Network Service 传到 Renderer 的？**

> 通过 Mojo Data Pipe。Network Service 创建一个 Data Pipe，将 producer handle 保留用于写入网络数据，将 consumer handle 通过 Mojo 接口传给 Renderer。Renderer 从 consumer handle 流式读取数据。这避免了将整个响应体通过 IPC 消息传输的内存开销。
