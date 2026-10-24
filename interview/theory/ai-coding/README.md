# AI Coding 面试考察

面向 C++ / 客户端工程师的 AI 辅助编程专项 — 从原理到实践，覆盖概念、架构、术语、工具、产品、质量安全与面试题。

> 定位：面试能答、工程能用；不展开模型训练数学。  
> 相关模块：[dev-tools/](../dev-tools/)（Git/CMake）、[cpp-core/](../cpp-core/)（语言本身）

---

## 主题列表 (9 个主题)

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 01 | [01-ai-fundamentals.md](01-ai-fundamentals.md) | AI 基础概念 | LLM、Token、Context、RAG、Embedding、Agent、AGI 辨析 |
| 02 | [02-coding-agent-architecture.md](02-coding-agent-architecture.md) | 编程 Agent 架构 | 客户端 ↔ Runtime ↔ 模型 ↔ 工具/MCP ↔ 索引 |
| 03 | [03-phenomena-and-terms.md](03-phenomena-and-terms.md) | 现象与术语 | 幻觉、注入、漂移、空转等 + 缓解手段 |
| 04 | [04-prompt-context-engineering.md](04-prompt-context-engineering.md) | Prompt / Context | 规则文件、检索裁剪、多文件改动策略 |
| 05 | [05-oss-toolchain.md](05-oss-toolchain.md) | 开源工具链 | 本地模型、编排、向量库、检索、评测、MCP |
| 06 | [06-mainstream-products.md](06-mainstream-products.md) | 主流产品对比 | Copilot、Cursor、Claude、Codex、Windsurf 等 |
| 07 | [07-quality-safety-eval.md](07-quality-safety-eval.md) | 质量、安全与评测 | 验收、Guardrails、合规、Eval |
| 08 | [08-theory-interview-qa.md](08-theory-interview-qa.md) | 理论考察题 | 高频概念题与追问链 |
| 09 | [09-practice-scenarios.md](09-practice-scenarios.md) | 实战场景考察 | 修 bug、重构、Review、机密代码等 |

---

## 学习路径（由浅入深）

1. **01**：先建立共同语言（LLM / RAG / Agent）
2. **02 → 03**：架构怎么跑 + 会出现什么坑、业内怎么叫
3. **04 → 05 → 06**：怎么喂上下文、用什么开源件、主流产品差在哪
4. **07**：质量与安全底线（面试加分）
5. **08 → 09**：理论速刷 + 场景口述题

---

## 面试高频考点

- RAG 解决什么问题？和 Fine-tune / 长 Context 怎么选？
- Chatbot 与 Coding Agent 的本质区别（Tool Use / 闭环）
- 幻觉是什么？工程上如何系统性降低？
- Coding Agent 客户端与服务端各自职责
- MCP 是什么？和「插件/工具调用」的关系
- Context 满了怎么办？Rules / AGENTS.md 的作用
- Copilot vs Cursor vs Claude Code vs Codex 的定位差异
- 如何 Review AI 生成的代码？如何定验收标准？
- 私有代码 / 密钥如何防泄露？

---

## 文件列表

```
ai-coding/
├── README.md
├── 01-ai-fundamentals.md
├── 02-coding-agent-architecture.md
├── 03-phenomena-and-terms.md
├── 04-prompt-context-engineering.md
├── 05-oss-toolchain.md
├── 06-mainstream-products.md
├── 07-quality-safety-eval.md
├── 08-theory-interview-qa.md
└── 09-practice-scenarios.md
```
