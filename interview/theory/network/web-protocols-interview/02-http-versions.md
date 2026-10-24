# 02 - HTTP 版本演进

## 核心概念

HTTP 从 1.0 到 3 的演进，核心驱动力是：**减少延迟、提高并发、增强安全**。

```
HTTP/1.0  →  HTTP/1.1  →  HTTP/2  →  HTTP/3
 短连接      持久连接      多路复用     QUIC(UDP)
             管道化        二进制分帧    0-RTT
             队头阻塞      HPACK压缩     无队头阻塞
```

---

## 原理深入

### HTTP/1.0 → HTTP/1.1

**1.0 问题**：每个请求建立新 TCP 连接，三次握手开销大。

**1.1 改进**：

| 特性 | 说明 |
|------|------|
| 持久连接 | 默认 Keep-Alive，一个 TCP 连接发多个请求 |
| 管道化 (Pipelining) | 不等响应就发下一个请求（实际很少用） |
| 分块传输 | `Transfer-Encoding: chunked`，流式响应 |
| Host 头 | 支持虚拟主机（同一 IP 多个域名） |
| 范围请求 | `Range` 头支持断点续传 |

**1.1 遗留问题 — 队头阻塞 (Head-of-Line Blocking)**：

```
同一 TCP 连接上的请求必须按序响应：

请求：  GET /big.jpg  →  GET /small.css  →  GET /tiny.js
响应：  [====big.jpg====] [css] [js]
         ↑ small.css 和 tiny.js 被 big.jpg 阻塞
```

### HTTP/2

2015 年发布，核心改进：

**二进制分帧 (Binary Framing)**：所有通信拆分为帧 (Frame)，属于某个流 (Stream)。

```
TCP 连接
├── Stream 1: GET /page.html
│   ├── HEADERS frame
│   └── DATA frame
├── Stream 3: GET /style.css
│   ├── HEADERS frame
│   └── DATA frame
└── Stream 5: GET /script.js
    ├── HEADERS frame
    └── DATA frame
```

**多路复用 (Multiplexing)**：多个 Stream 交错发送，一个 Stream 阻塞不影响其他。

| 特性 | 说明 |
|------|------|
| 多路复用 | 一个 TCP 连接并行传输多个请求/响应 |
| HPACK | 头部压缩，静态表 + 动态表，减少冗余 |
| Server Push | 服务器主动推送资源（实际使用较少） |
| 流优先级 | 客户端可设置 Stream 优先级 |
| 必须 TLS | 主流浏览器仅支持 h2 over TLS |

**HTTP/2 仍有的队头阻塞**：TCP 层。一个包丢失，整个 TCP 连接所有 Stream 等待重传。

### HTTP/3

2022 年标准化，基于 **QUIC** 协议（UDP 之上）：

| 特性 | 说明 |
|------|------|
| 传输层 | QUIC (UDP)，非 TCP |
| 连接建立 | 0-RTT / 1-RTT（复用连接时 0-RTT） |
| 队头阻塞 | 仅在单个 Stream 内，Stream 之间互不影响 |
| 连接迁移 | 切换网络（WiFi → 4G）不断连 |
| 加密 | 内置 TLS 1.3，不可降级 |

```
HTTP/2 队头阻塞（TCP 层）：
  Stream1 Stream2 Stream3 共用一个 TCP
  一个包丢失 → 所有 Stream 等待

HTTP/3 无 TCP 层阻塞（QUIC 层）：
  Stream1 Stream2 Stream3 各自独立
  Stream1 丢包 → 仅 Stream1 重传，Stream2/3 不受影响
```

---

## 版本对比表

| 特性 | HTTP/1.1 | HTTP/2 | HTTP/3 |
|------|----------|--------|--------|
| 传输 | TCP | TCP | QUIC (UDP) |
| 格式 | 文本 | 二进制帧 | 二进制帧 |
| 多路复用 | 无（管道化有限） | 有 | 有 |
| 头部压缩 | 无 | HPACK | QPACK |
| 队头阻塞 | 应用层有 | TCP 层有 | 仅 Stream 内 |
| 连接建立 | 1-RTT (TCP) + TLS | 1-RTT (TCP) + TLS | 0-RTT / 1-RTT |
| 加密 | 可选 | 实际必须 | 必须 |
| Server Push | 无 | 有 | 有 |

---

## 常见面试题

**Q1：HTTP/2 的多路复用是如何实现的？**

HTTP/2 在 TCP 之上引入二进制分帧层。每个请求/响应是一个 Stream，Stream 由多个 Frame 组成。多个 Stream 的 Frame 可以交错在同一个 TCP 连接上传输，接收端按 Stream ID 重组。这样一个小资源的 Frame 不必等待大资源传完。

**Q2：HTTP/2 解决了队头阻塞吗？**

部分解决。应用层（HTTP）的队头阻塞解决了 — 响应不必按请求顺序返回。但 TCP 层的队头阻塞仍在 — 一个 TCP 段丢失，所有 Stream 都要等待重传。HTTP/3 用 QUIC 彻底解决。

**Q3：为什么 HTTP/3 选择 UDP 而不是 TCP？**

TCP 的队头阻塞是协议设计决定的 — 保证有序交付。QUIC 在 UDP 之上自己实现可靠传输，每个 Stream 独立有序，丢包只阻塞对应 Stream。此外 QUIC 内置加密、支持连接迁移。

**Q4：HTTP/1.1 如何优化性能（不升级到 HTTP/2）？**

- 域名分片 (Domain Sharding)：多个域名绕过浏览器 6 连接限制
- 资源合并：CSS/JS 打包减少请求数
- 内联小资源：Base64 嵌入 HTML
- 雪碧图 (Sprite)：多张小图合并
- 开启 Keep-Alive 和 Gzip 压缩

---

## 实战要点

- 现代 Web 服务应优先支持 HTTP/2，CDN 节点考虑 HTTP/3
- HTTP/2 Server Push 因缓存冲突等问题，Chrome 已移除支持，慎用
- 排查性能时区分"连接数过多"(1.1) 和"TCP 丢包"(2) 两类瓶颈
- Nginx/Envoy 等反向代理可自动协商 h2/h3

---

## 下一步

- HTTPS 安全层：[04-https-tls.md](04-https-tls.md)
- 缓存与 Cookie：[03-http-advanced.md](03-http-advanced.md)
