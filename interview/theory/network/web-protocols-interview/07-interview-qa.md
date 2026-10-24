# 07 - 面试题汇总

考前速刷 — 按主题分组，每题含追问链。

---

## HTTP 基础

**Q1：GET 和 POST 的区别？**

| 维度 | GET | POST |
|------|-----|------|
| 参数位置 | URL Query | Request Body |
| 幂等性 | 幂等 | 非幂等 |
| 缓存 | 可缓存 | 默认不缓存 |
| 长度 | 受 URL 限制 | 无限制 |
| 安全 | 参数暴露在 URL | 相对隐蔽 |

> **追问**：POST 比 GET 安全吗？
> **答**：不一定。两者在 HTTP 下都是明文，安全取决于 HTTPS。POST 只是参数不在 URL 和历史记录中。

> **追问**：GET 可以有 Body 吗？
> **答**：HTTP 规范未禁止，但语义上 GET 不应有副作用，大多数服务器/框架忽略 GET Body。不推荐。

---

**Q2：HTTP 是无状态协议，如何实现登录态？**

三种方案：Cookie+Session（服务端存状态）、JWT（客户端存 Token）、Cookie 存 Token（HttpOnly 防 XSS）。

> **追问**：Session 和 JWT 怎么选？
> **答**：Session 适合单体应用，注销简单；JWT 适合分布式/微服务，无状态易扩展，但注销需黑名单机制。

---

**Q3：401 和 403 的区别？**

- 401：未认证 — "请先登录"
- 403：已认证但无权限 — "你没有访问权限"

---

**Q4：常见 HTTP 状态码？**

- 200 成功、201 创建成功、204 无内容
- 301 永久重定向、302 临时重定向、304 协商缓存命中
- 400 请求错误、401 未认证、403 无权限、404 不存在
- 500 服务器错误、502 网关错误、503 服务不可用

---

## HTTP 版本

**Q5：HTTP/1.1 的队头阻塞是什么？**

同一 TCP 连接上，响应必须按请求顺序返回。大资源阻塞后续小资源。

> **追问**：HTTP/2 怎么解决的？
> **答**：二进制分帧 + 多路复用，多个 Stream 的帧可交错传输，应用层队头阻塞消除。

> **追问**：HTTP/2 还有队头阻塞吗？
> **答**：有，在 TCP 层。一个 TCP 段丢失，所有 Stream 等待重传。HTTP/3 用 QUIC 解决。

---

**Q6：HTTP/2 的关键特性？**

多路复用、二进制分帧、HPACK 头部压缩、Server Push、Stream 优先级。必须基于 TLS。

---

**Q7：HTTP/3 为什么用 QUIC？**

QUIC 基于 UDP，每个 Stream 独立可靠传输，丢包只阻塞单个 Stream。内置 TLS 1.3，支持 0-RTT 和连接迁移。

---

## HTTP 进阶

**Q8：强缓存和协商缓存的区别？**

- 强缓存：不发请求，直接用本地（Cache-Control: max-age / Expires）
- 协商缓存：发请求验证，304 则用本地（ETag / Last-Modified）

> **追问**：优先级？
> **答**：强缓存 > 协商缓存；ETag > Last-Modified。

---

**Q9：Cookie 的 HttpOnly 和 SameSite 作用？**

- HttpOnly：禁止 JS 访问，防 XSS 窃取 Cookie
- SameSite=Strict：禁止跨站发送 Cookie，防 CSRF

---

**Q10：什么是 CORS 预检请求？**

非简单请求（PUT/自定义头/application/json）先发送 OPTIONS 请求，服务器返回允许的方法和头，通过后浏览器才发真实请求。

---

## HTTPS / TLS

**Q11：HTTPS 的握手过程？**

TLS 1.2：ClientHello → ServerHello + 证书 → 验证证书 → 密钥交换 → 加密通信（2-RTT）。
TLS 1.3：ClientHello(含 key_share) → ServerHello + 证书 + Finished → Finished（1-RTT）。

> **追问**：为什么用混合加密？
> **答**：非对称加密安全但慢，只用于交换对称密钥；对称加密快，用于后续数据传输。

---

**Q12：HTTPS 一定安全吗？**

不一定。证书伪造、混合内容、降级攻击、服务端漏洞、恶意 CA 等情况下仍不安全。

---

**Q13：什么是前向安全 (Forward Secrecy)？**

即使服务器私钥泄露，历史通信也无法被解密。ECDHE 密钥交换提供前向安全，RSA 密钥交换不提供。

---

**Q14：对称加密和非对称加密的区别？**

| | 对称 | 非对称 |
|---|------|--------|
| 密钥 | 同一密钥 | 公钥/私钥对 |
| 速度 | 快 | 慢 |
| 用途 | 数据加密 | 密钥交换、签名 |

---

## WebSocket

**Q15：WebSocket 和 HTTP 是什么关系？**

WebSocket 基于 HTTP Upgrade 握手（101 Switching Protocols），之后切换为独立帧协议。不是两种并列的网络框架，而是同一 TCP 连接上的协议切换。

---

**Q16：WebSocket 握手过程？**

客户端发 HTTP GET + Upgrade: websocket + Sec-WebSocket-Key → 服务器返回 101 + Sec-WebSocket-Accept（Key 的 SHA1+Base64） → 切换为 WebSocket 帧通信。

---

**Q17：WebSocket 帧格式关键字段？**

FIN（是否最后一帧）、Opcode（1=Text, 2=Binary, 8=Close, 9=Ping, 10=Pong）、MASK（客户端必须掩码）、Payload（数据）。

---

**Q18：WebSocket 和 Socket 的区别？**

Socket 是操作系统 API（BSD Socket），WebSocket 是应用层协议。WebSocket 底层使用 TCP Socket，但定义了帧格式和握手规范。

---

**Q19：WebSocket 如何穿过 Nginx？**

转发 Upgrade 和 Connection 头，设置 proxy_http_version 1.1，增大 proxy_read_timeout。

---

## 综合对比

**Q20：短轮询、长轮询、SSE、WebSocket 怎么选？**

| 场景 | 方案 |
|------|------|
| 只需服务器推送 | SSE |
| 双向实时通信 | WebSocket |
| 简单原型/低频 | 长轮询 |
| 非实时 CRUD | REST (HTTP) |

---

**Q21：WebSocket 多节点如何广播？**

Redis Pub/Sub 或 Kafka：任一节点收到消息 → 发布到频道 → 所有节点订阅 → 推给各自客户端。

---

**Q22：如何实现聊天室？**

HTTP 登录鉴权 → WebSocket 连接携带 Token → 消息广播（单节点内存 / 多节点 Redis Pub/Sub） → 断线指数退避重连 → HTTP API 拉取离线消息。

---

**Q23：TCP Keep-Alive 和 HTTP Keep-Alive 的区别？**

- HTTP Keep-Alive：应用层，一个 TCP 连接发多个 HTTP 请求
- TCP Keep-Alive：传输层，探测空闲连接是否存活
- 二者独立，互不影响

---

**Q24：从 URL 输入到页面显示，涉及哪些网络协议？**

DNS 解析 → TCP 三次握手 → TLS 握手（HTTPS）→ HTTP 请求/响应 → 解析 HTML → 加载 CSS/JS/图片（更多 HTTP 请求）→ 渲染。

> 详见 [fundamentals/09-browser-navigation.md](../fundamentals/09-browser-navigation.md)

---

## 速记清单

面试前 5 分钟过一遍：

- [ ] GET vs POST（幂等、缓存、参数位置）
- [ ] HTTP/1.1 队头阻塞 → HTTP/2 多路复用 → HTTP/3 QUIC
- [ ] 强缓存 vs 协商缓存（Cache-Control / ETag）
- [ ] HTTPS 握手（TLS 1.2: 2-RTT, 1.3: 1-RTT）
- [ ] 对称 vs 非对称加密，为什么混用
- [ ] WebSocket = HTTP Upgrade 101 + 帧协议
- [ ] WebSocket 帧：Opcode、Mask、Ping/Pong
- [ ] 短轮询 / 长轮询 / SSE / WebSocket 选型
- [ ] 401 vs 403
- [ ] Cookie HttpOnly / SameSite
