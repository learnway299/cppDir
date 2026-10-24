# 10 - wxWidgets 理论考察题

---

## 定位与架构

**Q1：wxWidgets 是什么？和 Qt、Duilib 的核心差别？**  
答：跨平台 C++ GUI；默认**原生控件包装**。Qt 自有控件+元对象生态更强；Duilib 是 Windows DirectUI 自绘。wx 外观跟系统，深度换肤弱于自绘方案。

**Q2：公共 API 如何落到各平台？**  
答：port 层（MSW/GTK/OSX）实现同一抽象；应用写 wx API。

**Q3：子窗口所有权？**  
答：父拥有子，父销毁级联删子；通常勿对子 `delete`。

---

## 应用与事件

**Q4：程序入口？**  
答：`wxApp` 派生 + `wxIMPLEMENT_APP`；业务在 `OnInit`。

**Q5：事件表 vs Bind？**  
答：Bind 更现代、可动态解绑、支持 lambda；事件表多见于老代码。

**Q6：命令事件为何能传到 Frame？**  
答：命令事件默认可向父传播，便于集中处理。

**Q7：`event.Skip()` 作用？**  
答：允许继续默认处理/传播；该 Skip 不 Skip 会丢默认行为或短路。

---

## 布局与绘制

**Q8：Sizer 的 proportion / EXPAND？**  
答：proportion 分剩余空间；EXPAND 在垂直于主轴或指定方向拉满。

**Q9：Paint 用什么 DC？**  
答：`wxPaintDC`（或 Buffered 变体）；仅在 Paint 处理器内。

---

## 线程与对话框

**Q10：工作线程更新 UI？**  
答：`CallAfter` / `wxQueueEvent`，禁止直接碰控件。

**Q11：ShowModal 特点？**  
答：嵌套消息循环、阻塞调用栈直到关闭。

---

## 版本与工程

**Q12：3.x 相对 2.x 重要变化？**  
答：Unicode 默认、Bind、HiDPI/现代后端、构建与 API 清理。

**Q13：跨平台差异如何隔离？**  
答：`__WXMSW__` 等条件编译 + 平台实现文件 + CI 多平台。

---

## 追问链示例

> 为何列表滚动卡？  
> → 是否每行自绘过重 / 全量 Refresh / UI 线程做 IO？  
> → 改为脏区刷新、虚拟列表思路、IO 下沉线程。

---

## 速记清单

- [ ] 原生包装 ≠ 自绘  
- [ ] wxApp / OnInit / 主循环  
- [ ] 父删子、别双删  
- [ ] Bind + Skip 语义  
- [ ] Sizer proportion/EXPAND/DIP  
- [ ] UI 线程 + CallAfter  
- [ ] 2.x→3.x：Unicode、Bind、HiDPI  
