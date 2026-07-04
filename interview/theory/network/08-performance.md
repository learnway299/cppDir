# 08 - 网络性能 (Network Performance)

## 简介

网络性能优化是提高系统响应速度和吞吐量的关键。

---

## 1. 性能指标

| 指标 | 定义 | 优化目标 |
|------|------|----------|
| **延迟** | 数据传输时间 | 降低 |
| **带宽** | 最大传输速率 | 充分利用 |
| **吞吐量** | 实际传输速率 | 提高 |
| **丢包率** | 丢失数据包比例 | 降低 |

---

## 2. TCP 调优

### 2.1 常用参数

```bash
# 查看/设置 TCP 参数
sysctl -a | grep tcp

# 发送/接收缓冲区
net.core.rmem_max = 16777216
net.core.wmem_max = 16777216

# TCP 窗口扩大
net.ipv4.tcp_window_scaling = 1

# TIME_WAIT 重用
net.ipv4.tcp_tw_reuse = 1

# 快速打开
net.ipv4.tcp_fastopen = 3
```

### 2.2 拥塞控制算法

```bash
# 查看当前算法
sysctl net.ipv4.tcp_congestion_control
# 设置为 BBR
sysctl -w net.ipv4.tcp_congestion_control=bbr
```

---

## 3. CDN

**CDN (Content Delivery Network)**：内容分发网络。

```
用户请求流程：

用户 ──► DNS ──► 就近 CDN 节点 ──► 缓存命中 ──► 返回
                      │
                      └──► 缓存未命中 ──► 回源 ──► 缓存
```

**CDN 优势**：
- 就近访问，降低延迟
- 分担源站压力
- 提高可用性

---

## 4. 负载均衡

### 4.1 负载均衡算法

| 算法 | 特点 |
|------|------|
| 轮询 | 简单，无状态 |
| 加权轮询 | 考虑服务器性能 |
| IP 哈希 | 同一 IP 到同一服务器 |
| 最少连接 | 选择连接数最少的 |
| 一致性哈希 | 最小化服务器变动影响 |

### 4.2 负载均衡层次

| 层次 | 技术 | 特点 |
|------|------|------|
| DNS | DNS 轮询 | 简单，缓存问题 |
| L4 | LVS, F5 | 高性能 |
| L7 | Nginx, HAProxy | 灵活，内容感知 |

---

## 5. 连接池

```cpp
// 连接池优势
// 1. 复用连接，减少握手开销
// 2. 限制并发连接数
// 3. 提高响应速度

class ConnectionPool {
    std::queue<Connection*> pool;
    std::mutex mtx;
    size_t max_size;

public:
    Connection* acquire() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!pool.empty()) {
            auto conn = pool.front();
            pool.pop();
            return conn;
        }
        return createNew();
    }

    void release(Connection* conn) {
        std::lock_guard<std::mutex> lock(mtx);
        if (pool.size() < max_size) {
            pool.push(conn);
        } else {
            delete conn;
        }
    }
};
```

---

## 6. 面试常见问题

**Q1：如何优化 TCP 性能？**
- 调整缓冲区大小
- 使用 BBR 拥塞控制
- 开启 TCP Fast Open
- 复用连接

**Q2：CDN 如何加速？**
- 就近返回缓存内容
- 减少到源站的请求

**Q3：负载均衡算法如何选择？**
- 无状态服务：轮询
- 有状态服务：IP 哈希、一致性哈希
- 性能差异大：加权轮询

**Q4：如何降低网络延迟？**
- 减少 RTT（使用 CDN）
- 减少数据量（压缩）
- 复用连接（Keep-Alive）
- 并行请求
