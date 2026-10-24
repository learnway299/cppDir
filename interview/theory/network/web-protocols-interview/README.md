# Web 协议面试专项

HTTP / HTTPS / WebSocket 面试向深入考察 — 概念清晰、原理到位、贴近实战。

> 基础知识通读：[fundamentals/05-application-layer.md](../fundamentals/05-application-layer.md)

---

## 主题列表 (8 个主题)

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 01 | [01-http-fundamentals.md](01-http-fundamentals.md) | HTTP 核心概念 | 报文结构、方法幂等性、状态码、无状态 |
| 02 | [02-http-versions.md](02-http-versions.md) | HTTP 版本演进 | 1.0/1.1/2/3 特性、队头阻塞、QUIC |
| 03 | [03-http-advanced.md](03-http-advanced.md) | HTTP 进阶机制 | 缓存、Keep-Alive、Cookie/Session、CORS |
| 04 | [04-https-tls.md](04-https-tls.md) | HTTPS 与 TLS | 握手流程、证书链、TLS 1.2 vs 1.3 |
| 05 | [05-websocket-protocol.md](05-websocket-protocol.md) | WebSocket 协议 | 握手升级、帧格式、Ping/Pong、WSS |
| 06 | [06-comparison-scenarios.md](06-comparison-scenarios.md) | 对比与选型 | 短轮询/SSE/WebSocket 对比、场景决策 |
| 07 | [07-interview-qa.md](07-interview-qa.md) | 面试题汇总 | 高频 Q&A + 追问链，考前速刷 |
| 08 | [08-network-troubleshooting.md](08-network-troubleshooting.md) | 用户环境排查实战 | 分层排查、DNS/代理/证书/WS、速查表 |

---

## 学习路径

1. **01 → 03**：HTTP 从概念到进阶，建立完整 HTTP 知识链
2. **04**：HTTPS/TLS 安全层
3. **05 → 06**：WebSocket 协议 + 实时通信选型
4. **07**：考前集中刷题
5. **08**：用户侧网络问题分层排查（客服/现场/面试实战）

---

## 面试高频考点

- GET vs POST（幂等性、参数位置、缓存、安全）
- HTTP/1.1 队头阻塞 vs HTTP/2 多路复用 vs HTTP/3 QUIC
- HTTPS 握手全过程（TLS 1.2 / 1.3）
- 强缓存 vs 协商缓存
- WebSocket 与 HTTP 的关系（Upgrade 101）
- WebSocket 帧格式、Ping/Pong 心跳
- 短轮询 / 长轮询 / SSE / WebSocket 如何选型
- 用户连不上时如何分层排查（DNS / 代理 / 证书 / WS Upgrade）

---

## 文件列表

```
web-protocols-interview/
├── README.md
├── 01-http-fundamentals.md
├── 02-http-versions.md
├── 03-http-advanced.md
├── 04-https-tls.md
├── 05-websocket-protocol.md
├── 06-comparison-scenarios.md
├── 07-interview-qa.md
└── 08-network-troubleshooting.md
```
