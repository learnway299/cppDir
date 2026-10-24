# 11 - CEF 理论考察题

---

## 定位与版本

**Q1：CEF 是什么？**  
答：用 C/C++ 把 Chromium 嵌进原生应用的框架（现行 CEF3 多进程）。

**Q2：和 Electron / WebView2 差异？**  
答：CEF=原生宿主嵌入；Electron=JS/Node 应用壳；WebView2=Windows 系统 Edge，包体小但平台绑定。

**Q3：版本怎么谈？**  
答：CEF3 API + 捆绑 Chromium 大版本；安全靠跟火车升级。

---

## 多进程与启动

**Q4：有哪些主要进程？**  
答：Browser、Renderer、GPU、Utility 等；Browser 与宿主交互多。

**Q5：CefExecuteProcess 作用？**  
答：判断并进入子进程角色；子进程返回后勿再跑宿主 Initialize。

**Q6：关闭顺序？**  
答：关 Browser → 等 BeforeClose、放引用 → CefShutdown。

---

## Handler 与集成

**Q7：App vs Client？**  
答：App 进程级；Client 每浏览器实例。

**Q8：点名 LifeSpan / Load / Request Handler。**  
答：生命周期、加载、导航与请求拦截。

**Q9：如何嵌进父窗口？**  
答：WindowInfo 设父句柄；resize/focus/DPI 同步。

---

## JS 与 IPC

**Q10：JS 调 C++ 链路？**  
答：V8 注入(Renderer) → ProcessMessage → Browser 业务 → 可选回传。

**Q11：为何业务放 Browser？**  
答：沙箱与权限；Renderer 不宜直接碰特权资源。

**Q12：OSR 是什么？**  
答：无子窗，Paint 回调像素；输入由宿主注入。

---

## 性能与安全

**Q13：内存高是否一定泄漏？**  
答：不一定；多进程基线高。对比空白页/Chrome，查未关闭 Browser 与页面泄漏。

**Q14：生产能否 no-sandbox？**  
答：默认否。

**Q15：JS 桥最大风险？**  
答：任意网页触发特权原生能力；需白名单与鉴权。

---

## 速记清单

- [ ] CEF3 多进程 + ExecuteProcess  
- [ ] App / Client / Handler 地图  
- [ ] 有序关闭  
- [ ] JS 桥跨进程  
- [ ] 有窗 vs OSR  
- [ ] 跟 Chromium 升级  
- [ ] 沙箱与内容源  
