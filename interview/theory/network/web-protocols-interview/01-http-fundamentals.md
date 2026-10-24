# 01 - HTTP 核心概念

## 核心概念

**HTTP (HyperText Transfer Protocol)** 是应用层协议，基于 TCP，采用**请求-响应**模型，**无状态**。

```
客户端                              服务器
  │ ──── TCP 连接 ──────────────────► │
  │ ──── HTTP Request ──────────────► │
  │ ◄──── HTTP Response ───────────── │
  │ ──── (可复用连接发送更多请求) ──► │
```

三个关键特征：

| 特征 | 含义 | 影响 |
|------|------|------|
| 应用层 | 不关心数据如何传输，只管"请求什么资源" | 底层可以是 TCP（HTTP/1-2）或 QUIC/UDP（HTTP/3） |
| 请求-响应 | 客户端发起，服务器应答 | 默认半双工；服务器不能主动推送（HTTP/2 Server Push 除外） |
| 无状态 | 服务器不记住之前的请求 | 需要 Cookie/Session/Token 维持会话 |

---

## 原理深入

### 报文结构

**请求报文**：

```
GET /index.html HTTP/1.1          ← 请求行（方法 URI 版本）
Host: www.example.com             ← 请求头
User-Agent: Mozilla/5.0
Accept: text/html
                                  ← 空行（\r\n）
                                  ← 请求体（GET 通常为空）
```

**响应报文**：

```
HTTP/1.1 200 OK                   ← 状态行（版本 状态码 原因短语）
Content-Type: text/html
Content-Length: 1234
                                  ← 空行
<html>...</html>                  ← 响应体
```

### HTTP 方法

| 方法 | 用途 | 幂等 | 安全 | 请求体 |
|------|------|------|------|--------|
| GET | 获取资源 | 是 | 是 | 无 |
| POST | 提交/创建 | 否 | 否 | 有 |
| PUT | 全量更新 | 是 | 否 | 有 |
| PATCH | 部分更新 | 否 | 否 | 有 |
| DELETE | 删除 | 是 | 否 | 可选 |
| HEAD | 同 GET 但不返回 body | 是 | 是 | 无 |
| OPTIONS | 查询支持的方法（CORS 预检） | 是 | 是 | 无 |

**幂等性**：同一请求执行多次，效果与执行一次相同。POST 不幂等（重复提交会创建多条记录），PUT/DELETE 幂等。

**安全性**：不会修改服务器资源。GET/HEAD/OPTIONS 是安全的，POST/PUT/DELETE 不安全。

### 状态码

| 范围 | 类别 | 常见码 | 场景 |
|------|------|--------|------|
| 1xx | 信息 | 100 Continue | 客户端应继续发送 body |
| 2xx | 成功 | 200 OK, 201 Created, 204 No Content | 正常响应 / 创建成功 / 无 body |
| 3xx | 重定向 | 301 永久, 302 临时, 304 Not Modified | 资源迁移 / 缓存命中 |
| 4xx | 客户端错误 | 400 Bad Request, 401 Unauthorized, 403 Forbidden, 404 Not Found | 参数错误 / 未认证 / 无权限 / 不存在 |
| 5xx | 服务器错误 | 500 Internal, 502 Bad Gateway, 503 Service Unavailable, 504 Gateway Timeout | 内部错误 / 网关故障 / 过载 / 超时 |

**304 特别注意**：协商缓存命中时返回，body 为空，客户端使用本地缓存。

---

## 常见面试题

**Q1：GET 和 POST 的区别？**

| 维度 | GET | POST |
|------|-----|------|
| 参数位置 | URL Query String | Request Body |
| 长度限制 | 受 URL 长度限制（约 2KB~8KB） | 理论无限制 |
| 缓存 | 可被浏览器/CDN 缓存 | 默认不缓存 |
| 幂等性 | 幂等 | 非幂等 |
| 安全性 | 参数暴露在 URL 和浏览器历史 | 相对隐蔽 |
| 书签/刷新 | 可收藏、刷新安全 | 刷新可能重复提交 |

> **追问**：POST 一定比 GET 安全吗？
> **答**：不一定。POST 只是参数不在 URL 中，但仍是明文传输（除非 HTTPS）。安全取决于 HTTPS 而非方法选择。

**Q2：HTTP 是无状态协议，怎么实现登录态？**

三种主流方案：

1. **Cookie + Session**：服务器创建 Session 存用户数据，通过 Set-Cookie 返回 Session ID
2. **Token (JWT)**：服务器签发 Token，客户端每次请求在 Authorization 头携带
3. **Cookie 存 Token**：Token 写入 HttpOnly Cookie，兼顾安全与便捷

**Q3：401 和 403 的区别？**

- **401 Unauthorized**：未认证 — "你是谁？"（需要登录）
- **403 Forbidden**：已认证但无权限 — "我知道你是谁，但你不能访问"

---

## 实战要点

- RESTful API 设计：GET 查、POST 建、PUT 全量改、PATCH 部分改、DELETE 删
- GET 请求不要放敏感信息，即使用 POST body 也需 HTTPS
- 204 用于 DELETE 成功或 PUT 无返回内容的场景
- 301 vs 302：SEO 和缓存行为不同，永久迁移用 301

---

## 下一步

- 版本演进：[02-http-versions.md](02-http-versions.md)
- 进阶机制：[03-http-advanced.md](03-http-advanced.md)
