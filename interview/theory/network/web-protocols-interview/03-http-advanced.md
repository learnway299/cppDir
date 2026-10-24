# 03 - HTTP 进阶机制

## 核心概念

HTTP 进阶机制解决三类工程问题：**减少重复传输（缓存）**、**维持连接与会话（Keep-Alive / Cookie）**、**跨域访问（CORS）**。

---

## 原理深入

### 缓存机制

浏览器缓存分两层：

```
请求资源
    │
    ▼
┌─────────────┐  命中   ┌──────────┐
│  强缓存      │ ──────► │ 直接使用  │（不发请求）
│ Cache-Control│         └──────────┘
│ Expires      │
└──────┬──────┘
       │ 未命中/过期
       ▼
┌─────────────┐  304    ┌──────────┐
│  协商缓存    │ ──────► │ 使用本地  │（发请求但不传 body）
│ ETag        │         │ 缓存      │
│ Last-Modified│         └──────────┘
└──────┬──────┘
       │ 200
       ▼
   重新下载
```

**强缓存**（不发请求）：

| 响应头 | 说明 |
|--------|------|
| `Cache-Control: max-age=3600` | 3600 秒内直接使用缓存 |
| `Cache-Control: no-cache` | 可以缓存，但每次使用前必须协商 |
| `Cache-Control: no-store` | 不缓存任何内容 |
| `Expires: Wed, 21 Oct 2026 07:28:00 GMT` | 绝对过期时间（HTTP/1.0，已被 max-age 取代） |

**协商缓存**（发请求验证）：

| 响应头 | 请求头 | 说明 |
|--------|--------|------|
| `ETag: "abc123"` | `If-None-Match: "abc123"` | 内容哈希对比，精确 |
| `Last-Modified: ...` | `If-Modified-Since: ...` | 修改时间对比，秒级精度 |

优先级：强缓存 > 协商缓存；ETag > Last-Modified。

### Keep-Alive 辨析

| 概念 | 层级 | 作用 | 默认 |
|------|------|------|------|
| HTTP Keep-Alive | 应用层 | 一个 TCP 连接发多个 HTTP 请求 | HTTP/1.1 默认开启 |
| TCP Keep-Alive | 传输层 | 探测空闲连接是否存活 | 默认关闭（Linux 7200s） |

```
HTTP Keep-Alive:
  GET /a → 200 → GET /b → 200 → GET /c → 200
  └──────── 同一 TCP 连接 ────────┘

TCP Keep-Alive:
  连接空闲 7200s → 发探测包 → 无响应则关闭
```

> 详见 [fundamentals/10-heartbeat-mechanism.md](../fundamentals/10-heartbeat-mechanism.md)

### Cookie / Session / JWT

**Cookie + Session**：

```
1. 用户登录 → 服务器验证 → 创建 Session 存服务端
2. 响应 Set-Cookie: sessionId=abc123
3. 后续请求自动带 Cookie: sessionId=abc123
4. 服务器查 Session 识别用户
```

**JWT (JSON Web Token)**：

```
1. 登录成功 → 服务器签发 JWT（Header.Payload.Signature）
2. 客户端存储 Token（localStorage / Cookie）
3. 后续请求 Authorization: Bearer <token>
4. 服务器验证签名，从 Payload 读取用户信息（无需查库）
```

| 维度 | Session | JWT |
|------|---------|-----|
| 存储 | 服务端 | 客户端 |
| 扩展性 | 需共享 Session（Redis） | 无状态，易水平扩展 |
| 注销 | 直接删除 Session | 需黑名单或短过期时间 |
| 安全 | Session ID 泄露风险低 | Token 泄露 = 身份泄露 |
| 体积 | Cookie 只存 ID | Token 较大 |

**Cookie 安全属性**：

| 属性 | 作用 |
|------|------|
| HttpOnly | 禁止 JS 访问，防 XSS 窃取 |
| Secure | 仅 HTTPS 传输 |
| SameSite=Strict/Lax | 防 CSRF 跨站请求 |
| Domain / Path | 控制 Cookie 作用域 |

### CORS 跨域

浏览器同源策略：协议 + 域名 + 端口 三者相同才同源。

**简单请求**（GET/POST/HEAD + 简单头）：直接发送，服务器返回 `Access-Control-Allow-Origin`。

**预检请求 (Preflight)**：非简单请求（如 PUT、自定义头、Content-Type: application/json）先发送 OPTIONS：

```
浏览器                          服务器
  │ ── OPTIONS /api ──────────► │
  │    Origin: http://a.com     │
  │    Access-Control-Request-Method: PUT
  │ ◄── 204 ─────────────────── │
  │    Access-Control-Allow-Origin: http://a.com
  │    Access-Control-Allow-Methods: PUT
  │ ── PUT /api ──────────────► │  （预检通过后发真实请求）
```

---

## 常见面试题

**Q1：强缓存和协商缓存的区别？**

- 强缓存：不发请求，直接用本地副本（Cache-Control / Expires）
- 协商缓存：发请求问服务器"资源变了吗？"，没变返回 304 用本地副本（ETag / Last-Modified）

**Q2：刷新页面对缓存的影响？**

| 操作 | 效果 |
|------|------|
| 普通刷新 (F5) | 跳过强缓存，走协商缓存 |
| 强制刷新 (Ctrl+F5) | 跳过所有缓存，带 `Cache-Control: no-cache` |
| 地址栏回车 | 走正常缓存策略 |

**Q3：Token 放 localStorage 还是 Cookie？**

- **localStorage**：方便 JS 读取，但 XSS 可窃取
- **HttpOnly Cookie**：JS 无法访问，防 XSS，但需防 CSRF（SameSite）
- **推荐**：敏感 Token 放 HttpOnly Cookie + SameSite=Strict

**Q4：如何解决跨域？**

1. CORS（服务端设置响应头）
2. 反向代理（Nginx 同源转发）
3. JSONP（仅 GET，已过时）
4. postMessage（iframe 跨域通信）

---

## 实战要点

- 静态资源（JS/CSS/图片）设长 max-age + 文件名 hash，实现 immutable 缓存
- API 接口设 `Cache-Control: no-store` 防缓存敏感数据
- Session 分布式部署必须用 Redis 等共享存储
- JWT 过期时间不宜过长，配合 Refresh Token 机制

---

## 下一步

- HTTPS/TLS：[04-https-tls.md](04-https-tls.md)
- WebSocket：[05-websocket-protocol.md](05-websocket-protocol.md)
