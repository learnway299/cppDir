# Computer Network Knowledge

计算机网络知识 — 涵盖协议栈基础与 Web 协议面试专项。

---

## 子模块

| 子模块 | 定位 | 主题数 |
|--------|------|--------|
| [fundamentals/](fundamentals/) | 计算机网络知识体系（协议栈、编程、安全、性能） | 10 |
| [web-protocols-interview/](web-protocols-interview/) | HTTP / HTTPS / WebSocket 面试专项深入 | 8 |

---

## 学习路径

1. **协议栈基础** — 从 [fundamentals/01-network-model.md](fundamentals/01-network-model.md) 开始，按 01 → 10 顺序通读
2. **Web 协议冲刺** — 通读 fundamentals/05 后，进入 [web-protocols-interview/](web-protocols-interview/) 做面试专项复习

---

## 目录结构

```
network/
├── README.md                          # 本文件 - 总索引
├── fundamentals/                      # 计算机网络基础（10 个主题）
│   ├── README.md
│   ├── 01-network-model.md
│   └── ... 10-heartbeat-mechanism.md
└── web-protocols-interview/           # Web 协议面试专项（8 个主题）
    ├── README.md
    ├── 01-http-fundamentals.md
    └── ... 08-network-troubleshooting.md
```

---

## 面试重点速查

| 主题 | 推荐模块 |
|------|---------|
| TCP/IP、三次握手、Socket 编程 | [fundamentals/](fundamentals/) |
| HTTP 方法、状态码、版本演进 | [web-protocols-interview/](web-protocols-interview/) |
| HTTPS/TLS 握手、证书 | [web-protocols-interview/04-https-tls.md](web-protocols-interview/04-https-tls.md) |
| WebSocket 协议、实时通信选型 | [web-protocols-interview/](web-protocols-interview/) |
| 用户环境网络排查 | [web-protocols-interview/08-network-troubleshooting.md](web-protocols-interview/08-network-troubleshooting.md) |
| 浏览器导航、心跳机制 | [fundamentals/09](fundamentals/09-browser-navigation.md)、[10](fundamentals/10-heartbeat-mechanism.md) |
