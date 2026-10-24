# 09 - 性能与内存

## 核心概念

CEF 性能问题多半是 **Chromium 多进程成本 + 页面本身 + 宿主误用**，不是单点 API。

---

## 内存

| 来源 | 说明 |
|------|------|
| 每 Renderer | 站点隔离下标签越多越高 |
| GPU / 缓存 | 磁盘 cache_path、HTTP 缓存 |
| 泄漏 | 宿主持有 `CefRefPtr` 循环引用、关不掉的 Browser |
| 页面泄漏 | SPA 事件监听、大图 |

治理：限制同时 Browser 数、及时关闭、任务管理器对照、定期升级修引擎泄漏。

---

## 流畅度

- 宿主 UI 线程勿同步等 Renderer  
- 少强制全页刷新；CSS/动画问题用 DevTools  
- 硬件加速开关按机器策略  
- `CefDoMessageLoopWork` 频率合理  

---

## 启动

- 冷启动：进程拉起 + 读 pak  
- 策略：延迟创建 Browser、预热、减少 command line 冲突  
- 子进程路径与主程序同目录减少查找失败重试  

---

## 常见面试题

**Q1：为何嵌入一个网页内存就几百 MB？**  
答：多进程 Chromium 基线高；再加页面与隔离。需对比 Chrome 同页，区分引擎成本与业务泄漏。

**Q2：如何优化？**  
答：少标签、关无用 Browser、磁盘缓存策略、升级、页面自身优化、避免 UI 线程阻塞泵。

---

## 实战要点

- 建「空白页 / 业务页」内存基线。  
- 关闭路径打日志确认 `OnBeforeClose`。  

---

## 下一步

- [10-security-sandbox.md](10-security-sandbox.md)
