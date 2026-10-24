# Computer Network Knowledge

计算机网络知识 - 面试必备的 10 个主题。

> 返回上级：[network/ 总览](../README.md)

---

## 知识体系

### 基础层 (Basic) - 4 个主题

网络协议栈核心概念，必须掌握。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [网络模型](01-network-model.md) | OSI 七层、TCP/IP 四层、各层协议与设备 |
| 02 | [链路层](02-link-layer.md) | 以太网、MAC 地址、ARP、交换机、VLAN |
| 03 | [网络层](03-network-layer.md) | IP 协议、子网划分、路由、ICMP、NAT、IPv6 |
| 04 | [传输层](04-transport-layer.md) | TCP 三次握手/四次挥手、流量/拥塞控制、UDP |

### 进阶层 (Intermediate) - 2 个主题

应用层协议和编程。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 05 | [应用层](05-application-layer.md) | HTTP/1.1/2/3、HTTPS、DNS、WebSocket |
| 06 | [网络编程](06-socket-programming.md) | Socket API、IO 多路复用、高并发模型 |

### 深度层 (Advanced) - 4 个主题

网络安全、性能优化和综合实战。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 07 | [网络安全](07-security.md) | TLS/SSL、证书、加密、常见攻击与防御 |
| 08 | [网络性能](08-performance.md) | 延迟、带宽、TCP 调优、CDN、负载均衡 |
| 09 | [浏览器导航](09-browser-navigation.md) | URL 到页面显示全过程、渲染原理、性能优化 |
| 10 | [心跳机制](10-heartbeat-mechanism.md) | TCP/HTTP/应用层心跳对比、NAT 穿透、移动端优化 |

---

## 面试重点

### 必问话题

1. **TCP/IP 协议栈**
   - OSI 七层与 TCP/IP 四层对比
   - 各层功能与协议

2. **TCP 三次握手/四次挥手**
   - 完整流程与状态变化
   - 为什么是三次/四次
   - TIME_WAIT 状态

3. **HTTP/HTTPS**
   - HTTP 方法与状态码
   - HTTP/1.1 vs HTTP/2 vs HTTP/3
   - HTTPS 握手过程
   - 深入考察见 [web-protocols-interview/](../web-protocols-interview/)

4. **Socket 编程**
   - TCP/UDP Socket 流程
   - select/poll/epoll 区别
   - Reactor 模式

5. **浏览器导航全过程**
   - 从 URL 到页面显示
   - DNS 解析、TCP/TLS 握手
   - 浏览器渲染原理

6. **心跳机制**
   - TCP Keep-Alive vs HTTP Keep-Alive 区别
   - 应用层心跳设计
   - 三层心跳各自作用与必要性

### 加分项

1. TCP 拥塞控制算法（BBR）
2. HTTP/2 多路复用与 HPACK
3. TLS 1.3 vs TLS 1.2
4. CDN 原理
5. 负载均衡算法
6. 常见网络攻击与防御
7. 浏览器渲染性能优化
8. NAT 穿透与连接保活
9. 移动端智能心跳优化

---

## 文件列表

```
fundamentals/
├── README.md                  # 本文件
├── 01-network-model.md        # 网络模型
├── 02-link-layer.md           # 链路层
├── 03-network-layer.md        # 网络层
├── 04-transport-layer.md      # 传输层（含深入原理）
├── 05-application-layer.md    # 应用层（含 HTTP/TLS 详解）
├── 06-socket-programming.md   # 网络编程
├── 07-security.md             # 网络安全
├── 08-performance.md          # 网络性能
├── 09-browser-navigation.md   # 浏览器导航全过程
└── 10-heartbeat-mechanism.md  # 心跳机制
```
