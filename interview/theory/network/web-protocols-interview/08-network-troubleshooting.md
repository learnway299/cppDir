# 08 - 用户环境网络排查实战

## 核心概念

用户报「连不上 / 很慢 / 证书报错 / 聊天断线」时，优先按协议栈**自下而上分层定位**，避免一上来改代码或重装客户端。

```
应用层问题（业务逻辑、Token 过期）
    ↑
HTTP / WebSocket（状态码、Upgrade、CORS）
    ↑
TLS（证书、握手失败）
    ↑
TCP（端口不通、超时、RST）
    ↑
DNS（解析失败、解析到错误 IP）
    ↑
链路 / 本地网络（无网、网关错、代理/VPN）
```

**总原则**：

1. **先复现，再定论** — 同一账号、同一网络、同一时间段能否稳定复现
2. **先本机，后远端** — 本机 DNS / 代理 / 防火墙 → 公司网 → 运营商 → 服务端
3. **先对比，后深入** — 浏览器能开、客户端不能 → 重点查代理/证书/自定义网络栈
4. **留证据** — 错误码、时间戳、截图、`curl -v` 输出、客户端日志

---

## 1. 信息收集清单

向用户（或面试场景中的「客户」）先问清：

| 维度 | 要问什么 |
|------|---------|
| 现象 | 完全失败 / 偶发 / 慢？错误文案或截图？ |
| 范围 | 仅某功能？仅某账号？仅某地区？ |
| 环境 | 家用 WiFi / 公司网 / 手机热点？有线还是无线？ |
| 代理 | 是否开了系统代理、VPN、安全软件？ |
| 对比 | 浏览器能否打开同一地址？手机能否？换网络能否？ |
| 时间 | 何时开始？是否与升级/换网同时发生？ |
| 日志 | 客户端日志、错误码、请求 ID |

---

## 2. 连通性与基础检查

### 本机网络是否正常

**Windows**：

```bat
ipconfig /all
ping 8.8.8.8
ping www.baidu.com
tracert example.com
pathping example.com
```

**Linux / macOS**：

```bash
ip addr        # 或 ifconfig
ping -c 4 8.8.8.8
ping -c 4 example.com
traceroute example.com
```

| 现象 | 可能原因 | 下一步 |
|------|---------|--------|
| ping 公网 IP 失败 | 无网、网关错、防火墙拦 ICMP | 查网关、换热点、看杀软 |
| ping IP 通，ping 域名失败 | DNS 问题 | 见 §3 |
| tracert 在某跳中断 | 中间路由/运营商/公司出口 | 换网络对比 |
| 仅 WiFi 不行、有线/热点可以 | WiFi 隔离、路由器问题 | 换 AP / 重启路由 |

### 端口是否可达

```bat
:: Windows
Test-NetConnection example.com -Port 443

:: 或 telnet example.com 443
```

```bash
# Linux / macOS
nc -vz example.com 443
curl -vI https://example.com
```

TCP 都建不上：查防火墙、安全组、公司出口是否拦 443/自定义端口。

---

## 3. DNS 问题

### 常见表现

- 「找不到服务器 / DNS_PROBE_FINISHED_NXDOMAIN」
- 打开了错误站点、证书域名不匹配（解析到劫持 IP）
- 内网域名在外网解析失败（或反之）

### 排查命令

**Windows**：

```bat
nslookup example.com
nslookup example.com 8.8.8.8
ipconfig /displaydns
ipconfig /flushdns
```

**Linux / macOS**：

```bash
dig example.com
dig @8.8.8.8 example.com
cat /etc/hosts
sudo dscacheutil -flushcache   # macOS
```

### 常见原因与处理

| 原因 | 如何确认 | 处理 |
|------|---------|------|
| hosts 被改 | 看 `C:\Windows\System32\drivers\etc\hosts` | 删掉异常映射 |
| DNS 缓存脏 | flushdns 后恢复 | 清缓存；查是否恶意软件改 DNS |
| 运营商 DNS 污染 | 换 8.8.8.8 / 1.1.1.1 后正常 | 改 DNS 或用 DoH |
| 内网私有域名 | 公网 DNS 无记录 | 连 VPN / 用公司 DNS |
| 本地安全软件劫持 DNS | 关杀软/对比后正常 | 加白名单或换软件 |

---

## 4. 代理 / VPN / 防火墙

客户端「浏览器能上、App 不能」时，**优先查代理**。

### 检查点

| 检查项 | Windows 位置 / 方法 |
|--------|---------------------|
| 系统代理 | 设置 → 网络和 Internet → 代理 |
| 环境变量 | `HTTP_PROXY` / `HTTPS_PROXY` / `NO_PROXY` |
| PAC 脚本 | 自动配置脚本 URL 是否指向失效地址 |
| VPN | 全局/分流模式；是否劫持 DNS |
| 杀软 / 防火墙 | 出站规则是否拦客户端进程 |
| 公司 MITM | 见 §5 证书 |

### 对比实验

1. 关代理 / 关 VPN，再试  
2. 换手机热点，再试  
3. 用 `curl` 分别直连与走代理：

```bash
curl -vI https://example.com
curl -vI -x http://127.0.0.1:7890 https://example.com
```

### 公司网常见坑

- 只允许 80/443，自定义端口被拦  
- 强制 HTTP 代理，未配置代理的原生 Socket 直连失败  
- SSL 解密（HTTPS 中间人），客户端未信任公司根证书  
- WebSocket Upgrade 被代理丢弃（见 §7）

---

## 5. TLS / 证书问题

### 常见报错

| 现象 / 错误 | 含义 |
|-------------|------|
| 证书过期 | `certificate has expired` |
| 域名不匹配 | hostname does not match CN/SAN |
| 不受信任的颁发者 | 缺中间证书 / 缺公司根 CA |
| 证书被吊销 | OCSP/CRL 校验失败 |
| 时钟错误 | 系统时间不准导致「未生效/已过期」 |
| SSL handshake failure | 协议版本/加密套件不兼容 |

### 排查步骤

1. **看系统时间** — 时间差过大直接导致校验失败  
2. **浏览器打开同一 URL** — 点锁图标看证书链  
3. **用 openssl 看握手细节**：

```bash
openssl s_client -connect example.com:443 -servername example.com -showcerts
```

关注：`Verify return code`、证书链是否完整、SAN 是否含域名。

4. **公司代理 MITM** — 浏览器因已装公司 CA 正常，客户端未导入根证书则失败 → 导入企业根 CA，或对客户端做证书 pinning 时预置企业证书策略。

> 证书原理详见 [04-https-tls.md](04-https-tls.md)

---

## 6. HTTP 层问题

### 用 curl 复现

```bash
# 看状态码与重定向
curl -vI https://example.com/api/health

# 带鉴权头
curl -v -H "Authorization: Bearer <token>" https://example.com/api/user

# 对比 HTTP/1.1 与 HTTP/2
curl --http1.1 -vI https://example.com
curl --http2 -vI https://example.com
```

### 常见现象

| 现象 | 可能原因 | 排查 |
|------|---------|------|
| 连接超时 | 网络不通、防火墙、错误 IP | ping / Test-NetConnection |
| 读超时 | 服务慢、中间设备掐长连接 | 看服务端 RT、调超时、查 LB |
| 401 / 403 | Token 过期、权限、时钟偏差 | 刷新登录、对时 |
| 404 | 路径/环境配错 | 对比官方文档与配置 |
| 502 / 504 | 网关后上游挂了或超时 | 查反向代理 / 上游健康检查 |
| 证书页能开、API 不行 | 域名/SNI/CDN 配置不一致 | curl 打具体 API Host |
| 仅浏览器 CORS 失败 | 跨域未放行 | 看预检 OPTIONS；App 无 CORS |
| 偶发旧数据 | 强缓存 / CDN 缓存 | 强制刷新、查 Cache-Control |

**注意**：浏览器有 CORS，原生客户端一般没有；「浏览器报 CORS、App 正常」通常不是同一问题。

---

## 7. WebSocket 专项排查

### 典型失败点

```
TCP 通 → TLS 通 → HTTP Upgrade 失败 → 连上后很快断开
```

| 阶段 | 现象 | 常见原因 |
|------|------|---------|
| 握手前 | 连不上 | DNS / 端口 / 代理 |
| Upgrade | 非 101，或一直 pending | 代理未转发 Upgrade；路径错；鉴权失败 |
| 握手后秒断 | Close / 异常断开 | 心跳缺失、NAT 超时、Token 校验失败 |
| 间歇断线 | 每隔 N 分钟断 | 中间设备空闲超时；需 Ping/Pong |

### Nginx 代理检查

服务端 / 网关需正确转发升级头：

```nginx
proxy_http_version 1.1;
proxy_set_header Upgrade $http_upgrade;
proxy_set_header Connection "upgrade";
proxy_read_timeout 3600s;
```

漏配时客户端常见：HTTP 200/400 而非 101，或连接立即关闭。

### 客户端侧排查

1. 浏览器 DevTools → Network → WS，看握手请求/响应头  
2. 确认用的是 `wss://`（生产）而非明文 `ws://`  
3. 抓客户端日志：Close code（如 1006 异常关闭、1008 策略违规）  
4. 对比：浏览器 WS 页面 vs 桌面客户端（证书/代理差异）

> 协议细节见 [05-websocket-protocol.md](05-websocket-protocol.md)；心跳与 NAT 见 [fundamentals/10-heartbeat-mechanism.md](../fundamentals/10-heartbeat-mechanism.md)

---

## 8. 常见问题速查表

| 用户描述 | 优先怀疑 | 快速验证 |
|----------|---------|---------|
| 完全打不开任何网页 | 本机网络 / 网关 | ping 8.8.8.8；换热点 |
| 网页能开，客户端不能 | 代理 / 证书 / 自定义端口 | 关代理；浏览器开同一 API |
| 提示证书错误 | 时间 / CA / MITM / 域名 | 对时；openssl s_client；锁图标 |
| DNS 错误 | hosts / DNS 污染 | nslookup；换公共 DNS；flushdns |
| 公司能连、家里不能（或反之） | VPN / 内网 DNS / 防火墙策略 | 对比两套网络配置 |
| 聊天经常自己掉线 | 心跳 / NAT / 代理空闲超时 | 抓断线周期；调 Ping 间隔 |
| WebSocket 连不上 | Upgrade 被拦 | 查 101；查 Nginx Upgrade 头 |
| 特别慢 | 丢包 / DNS 慢 / 跨国链路 | pathping；换 DNS；看 CDN 节点 |
| 仅某运营商不行 | 运营商劫持 / 路由差 | 换网络；tracert 对比 |
| 更新客户端后不行 | 新域名/证书 pinning/协议版本 | 看 changelog；对比旧版 |

---

## 9. 标准排查流程（可照着做）

```
1. 收集：现象截图 + 网络类型 + 是否代理/VPN + 能否复现
2. 对比：换热点？浏览器同一 URL？另一台设备？
3. 基础：ipconfig → ping IP → ping 域名 → 测 443 端口
4. DNS：nslookup / flushdns / 查 hosts
5. 代理：关闭系统代理与 VPN 再试
6. TLS：系统时间 + 浏览器证书 + openssl s_client
7. HTTP：curl -v 打具体 API，记录状态码与耗时
8. WS：确认 101、Upgrade 头、Close code、心跳配置
9. 上报：附上以上命令输出 + 客户端日志 + 时间点
```

---

## 10. 面试话术示例

**Q：用户说客户端连不上服务器，你怎么排查？**

按分层回答：先确认是否真的「网络问题」还是账号/业务问题；然后本机连通性 → DNS → 代理/防火墙 → TLS 证书 → HTTP 状态码 → 若是长连接再查 WebSocket Upgrade 与心跳；每一步用 ping/nslookup/curl/openssl 留下证据，并做换网络/关代理对比实验。

**Q：浏览器正常、客户端证书报错，最可能是什么？**

公司 HTTPS 解密代理：浏览器已信任企业根 CA，客户端未导入；或客户端做了 Certificate Pinning 却未包含企业证书；其次是系统时间不准、缺中间证书。

**Q：WebSocket 隔几分钟必断一次？**

高度怀疑中间 NAT/负载均衡/代理的空闲连接超时；用协议层 Ping/Pong 或应用层心跳保活，并核对 Nginx `proxy_read_timeout` 等超时配置。

---

## 实战要点

- 桌面客户端要显式处理系统代理（或提供「直连 / 跟随系统」开关）
- 日志里打印：最终解析 IP、是否走代理、TLS 错误码、HTTP 状态、WS Close code
- 不要只靠 ping：很多网络禁 ICMP，端口探测和 curl 更可靠
- 先让用户「关 VPN + 换手机热点」做二分，能排除大半环境问题

---

## 相关阅读

- [04-https-tls.md](04-https-tls.md) — 证书与握手
- [05-websocket-protocol.md](05-websocket-protocol.md) — Upgrade / 帧 / 心跳
- [06-comparison-scenarios.md](06-comparison-scenarios.md) — 实时方案选型
- [fundamentals/10-heartbeat-mechanism.md](../fundamentals/10-heartbeat-mechanism.md) — 三层心跳与 NAT
- [fundamentals/07-security.md](../fundamentals/07-security.md) — 网络安全概览
