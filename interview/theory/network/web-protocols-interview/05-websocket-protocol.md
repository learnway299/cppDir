# 05 - WebSocket 协议

## 核心概念

**WebSocket** 是应用层协议（RFC 6455），提供**全双工、长连接**通信。握手通过 HTTP Upgrade 完成，之后切换为独立的帧协议。

```
为什么需要 WebSocket？

HTTP 请求-响应模型的局限：
  客户端 ── 请求 ──► 服务器
  客户端 ◄── 响应 ── 服务器
  服务器无法主动推送 → 只能轮询

WebSocket：
  客户端 ◄────────────► 服务器
         双向实时通信
```

---

## 原理深入

### 握手升级

WebSocket 连接从 HTTP 开始，通过 **101 Switching Protocols** 升级：

**客户端请求**：

```
GET /chat HTTP/1.1
Host: server.example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
Sec-WebSocket-Protocol: chat, superchat
Origin: http://example.com
```

**服务器响应**：

```
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
Sec-WebSocket-Protocol: chat
```

`Sec-WebSocket-Accept` 计算方式：

```
Base64(SHA1(Sec-WebSocket-Key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"))
```

握手完成后，**同一 TCP 连接**上的后续通信不再使用 HTTP，而是 WebSocket 帧。

### 帧格式

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|   (7)       |             (16/64)           |
|N|V|V|V|       |S|             |                               |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Masking-key (if MASK=1)   |                               |
+-------------------------------+ - - - - - - - - - - - - - - - +
|                          Payload Data                          |
+---------------------------------------------------------------+
```

**关键字段**：

| 字段 | 说明 |
|------|------|
| FIN | 1 = 最后一帧，0 = 还有后续帧 |
| Opcode | 0=Continuation, 1=Text, 2=Binary, 8=Close, 9=Ping, 10=Pong |
| MASK | 客户端→服务器必须掩码 (MASK=1)，服务器→客户端不掩码 |
| Payload len | 数据长度，126/127 表示后续扩展长度字段 |

**Opcode 含义**：

| Opcode | 类型 | 用途 |
|--------|------|------|
| 0x1 | Text | UTF-8 文本 |
| 0x2 | Binary | 二进制数据 |
| 0x8 | Close | 关闭连接（可带状态码和原因） |
| 0x9 | Ping | 心跳探测 |
| 0xA | Pong | 心跳响应 |

### 心跳机制

WebSocket 协议内置 Ping/Pong：

```
客户端                    服务器
  │ ──── Ping ──────────► │
  │ ◄──── Pong ────────── │
  │                       │
  │ （超时未收到 Pong → 判定连接断开 → 重连）
```

与应用层心跳的区别：
- **Ping/Pong**：协议层，帧开销极小（2-10 字节）
- **应用层心跳**：自定义 JSON 如 `{"type":"ping"}`，开销大但可携带业务信息

### WSS

**WSS = WebSocket + TLS**，类似 HTTPS。握手流程：

```
1. TCP 连接
2. TLS 握手（同 HTTPS）
3. HTTP Upgrade 101（在加密通道内）
4. WebSocket 帧通信（加密）
```

生产环境**必须使用 WSS**，ws:// 明文易被窃听和劫持。

### 断线重连

工程实践中的重连策略：

```
连接断开
    │
    ▼
等待 1s → 重连失败
    │
    ▼
等待 2s → 重连失败
    │
    ▼
等待 4s → 重连失败
    │
    ▼
等待 8s ... （指数退避，上限 30-60s）
    │
    ▼
重连成功 → 重置退避计数，恢复订阅/同步状态
```

---

## 常见面试题

**Q1：WebSocket 和 HTTP 是什么关系？**

WebSocket 不是 HTTP 的对立面，而是**基于 HTTP 握手升级**的独立协议。连接建立用 HTTP（Upgrade: websocket → 101），建立后切换为 WebSocket 帧协议，不再走 HTTP 请求-响应。

**Q2：WebSocket 通信过程中还会用到 HTTP 吗？**

不会。101 响应之后，该 TCP 连接上只有 WebSocket 帧。但页面加载、REST API 调用等仍走 HTTP — 它们是不同连接。

**Q3：WebSocket 如何穿过 Nginx 代理？**

Nginx 配置：

```nginx
location /ws {
    proxy_pass http://backend;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_read_timeout 3600s;
}
```

关键：`Upgrade` 和 `Connection` 头必须转发，否则代理会把 101 升级破坏。

**Q4：WebSocket 为什么需要 Mask？**

RFC 6455 要求客户端发送的帧必须 Mask（异或随机 key），防止恶意脚本通过 WebSocket 构造非法 TCP 包攻击中间设备（Cache Poisoning）。服务器→客户端不需要 Mask。

**Q5：WebSocket 和 Socket 一样吗？**

不一样。**Socket** 是 BSD 接口（API），WebSocket 是应用层协议。WebSocket 底层仍使用 TCP Socket，但在 Socket 之上定义了帧格式和握手规范。

---

## 实战要点

- 生产环境用 WSS，配置合理的 Ping 间隔（30-60s）和超时
- 服务端需处理 Close 帧优雅关闭，避免 CLOSE_WAIT 堆积
- 大消息用 Binary 帧 + 分片（FIN=0 续帧）
- 多节点部署需 Sticky Session 或 Redis Pub/Sub 广播

---

## 下一步

- 通信方案选型：[06-comparison-scenarios.md](06-comparison-scenarios.md)
- 面试题汇总：[07-interview-qa.md](07-interview-qa.md)
