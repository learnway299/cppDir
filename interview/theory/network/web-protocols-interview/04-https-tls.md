# 04 - HTTPS 与 TLS

## 核心概念

**HTTPS = HTTP + TLS**，在 HTTP 之下加一层 TLS 加密，解决三个安全问题：

| 威胁 | TLS 如何解决 |
|------|-------------|
| 窃听 | 对称加密（AES）加密通信内容 |
| 篡改 | MAC / AEAD 认证加密，篡改可被检测 |
| 冒充 | 数字证书 + CA 验证服务器身份 |

```
HTTP:   客户端 ──── 明文 ────► 服务器
HTTPS:  客户端 ──── TLS 加密 ────► 服务器
                  └── HTTP 内容被加密 ──┘
```

---

## 原理深入

### TLS 握手流程

**TLS 1.2 握手（2-RTT）**：

```
客户端                                    服务器
  │ ──── ClientHello ──────────────────► │
  │      支持的 TLS 版本、加密套件、随机数    │
  │ ◄──── ServerHello ─────────────────── │
  │       选定套件、服务器随机数              │
  │ ◄──── Certificate ─────────────────── │
  │       服务器证书链                       │
  │ ◄──── ServerKeyExchange (可选) ────── │
  │ ◄──── ServerHelloDone ─────────────── │
  │ ──── ClientKeyExchange ─────────────► │
  │      预主密钥（用证书公钥加密）           │
  │ ──── ChangeCipherSpec ──────────────► │
  │ ──── Finished ─────────────────────► │
  │ ◄──── ChangeCipherSpec ────────────── │
  │ ◄──── Finished ────────────────────── │
  │ ══════ 加密通信开始 ═══════════════ │
```

**TLS 1.3 握手（1-RTT）**：

```
客户端                                    服务器
  │ ──── ClientHello ──────────────────► │
  │      密钥共享 (key_share)              │
  │ ◄──── ServerHello ─────────────────── │
  │       密钥共享、Certificate、Finished   │
  │ ──── Finished ─────────────────────► │
  │ ══════ 加密通信开始 ═══════════════ │
```

TLS 1.3 改进：
- 移除 RSA 密钥交换，仅支持 ECDHE（前向安全）
- 0-RTT 模式：复用连接时客户端在第一个包就发加密数据
- 握手消息更少，延迟更低

### 证书链验证

```
浏览器验证流程：

1. 服务器发送证书链：
   server.crt → intermediate.crt → root.crt

2. 浏览器检查：
   ├── server.crt 是否由 intermediate.crt 签发？
   ├── intermediate.crt 是否由 root.crt 签发？
   ├── root.crt 是否在浏览器信任库中？
   ├── 证书是否过期？
   ├── 域名是否匹配 (CN / SAN)？
   └── 证书是否被吊销 (OCSP / CRL)？
```

**关键概念**：

| 概念 | 说明 |
|------|------|
| CA | 证书颁发机构，根证书预装在操作系统/浏览器 |
| SNI | Server Name Indication，一个 IP 多个 HTTPS 站点 |
| 前向安全 (PFS) | 即使私钥泄露，历史通信仍不可解密（ECDHE） |
| 加密套件 | 如 `TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384` |

### 混合加密体系

TLS 使用**非对称 + 对称**混合加密：

```
1. 非对称加密（RSA/ECDHE）：交换对称密钥（握手阶段）
2. 对称加密（AES-GCM/ChaCha20）：加密实际数据（通信阶段）

为什么？
- 非对称加密慢但安全 → 只用于密钥交换
- 对称加密快 → 用于大量数据传输
```

---

## TLS 1.2 vs 1.3 对比

| 特性 | TLS 1.2 | TLS 1.3 |
|------|---------|---------|
| 握手 RTT | 2-RTT | 1-RTT（0-RTT 复用） |
| 密钥交换 | RSA / ECDHE | 仅 ECDHE（强制 PFS） |
| 加密算法 | 多种（含弱算法） | 仅 AEAD（AES-GCM, ChaCha20） |
| 0-RTT | 不支持 | 支持（有重放攻击风险） |
| 降级攻击 | 可能被降级到 1.0 | 不支持旧版本，无法降级 |

---

## 常见面试题

**Q1：HTTPS 的握手过程？**

简述：客户端发 ClientHello → 服务器回 ServerHello + 证书 → 客户端验证证书 → 密钥交换生成对称密钥 → 双方切换加密通信。TLS 1.2 需 2-RTT，TLS 1.3 只需 1-RTT。

**Q2：HTTPS 一定安全吗？**

不一定。以下情况仍不安全：
- 证书被伪造（用户忽略证书警告）
- 混合内容（HTTPS 页面加载 HTTP 资源）
- 降级攻击（强制使用旧 TLS 版本，TLS 1.3 已解决）
- 服务端漏洞（Heartbleed 等）
- 客户端被植入恶意 CA 证书

**Q3：什么是中间人攻击 (MITM)？如何防御？**

攻击者在客户端和服务器之间截获通信。防御：
- HTTPS 证书验证（浏览器检查 CA 签名）
- 证书固定 (Certificate Pinning)
- HSTS 强制 HTTPS

**Q4：HTTP 和 HTTPS 可以共用同一个端口吗？**

不能。HTTP 默认 80，HTTPS 默认 443。但可以通过：
- 80 端口返回 301 重定向到 443
- TLS ALPN 扩展在同一端口协商协议（不常见）

**Q5：对称加密和非对称加密的区别？HTTPS 为什么混用？**

| | 对称加密 | 非对称加密 |
|---|---------|-----------|
| 密钥 | 加解密同一密钥 | 公钥加密、私钥解密 |
| 速度 | 快 | 慢（100-1000 倍差距） |
| 密钥分发 | 困难（如何安全传递密钥？） | 公钥可公开 |

HTTPS 用非对称加密安全交换对称密钥，后续通信用对称加密保证性能。

---

## 实战要点

- 生产环境禁用 TLS 1.0/1.1，优先 TLS 1.3
- 配置 HSTS 头：`Strict-Transport-Security: max-age=31536000`
- 证书自动续期：Let's Encrypt + certbot
- 排查 HTTPS 问题：openssl s_client、SSL Labs 测试

---

## 下一步

- WebSocket 协议：[05-websocket-protocol.md](05-websocket-protocol.md)
- 安全攻击防御：[fundamentals/07-security.md](../fundamentals/07-security.md)
