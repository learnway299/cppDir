# Git 深入解析

Git 是分布式版本控制系统，面试高频考点。

---

## 1. Git 基础概念

### 1.1 三个工作区域

```
┌─────────────────────────────────────────────────────────────┐
│                    Git 三个区域                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   工作区              暂存区               本地仓库          │
│  (Working           (Staging            (Repository        │
│   Directory)         Area/Index)         .git)             │
│                                                             │
│  ┌─────────┐        ┌─────────┐        ┌─────────┐         │
│  │  编辑   │  add   │  暂存   │ commit │  提交   │         │
│  │  文件   │ ─────→ │  修改   │ ─────→ │  历史   │         │
│  └─────────┘        └─────────┘        └─────────┘         │
│       ↑                                     │              │
│       └─────────── checkout ────────────────┘              │
│                                                             │
│  文件状态：                                                  │
│  • Untracked  - 未跟踪（新文件）                             │
│  • Modified   - 已修改（工作区与暂存区不同）                  │
│  • Staged     - 已暂存（等待提交）                           │
│  • Committed  - 已提交（安全存储）                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Git 对象模型

```
┌─────────────────────────────────────────────────────────────┐
│                    Git 对象类型                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. blob（文件内容）                                         │
│     存储文件内容，不包含文件名                                │
│     SHA-1 哈希作为唯一标识                                   │
│                                                             │
│  2. tree（目录结构）                                         │
│     存储目录结构，包含文件名和对 blob/tree 的引用            │
│                                                             │
│  3. commit（提交）                                           │
│     指向 tree，包含作者、时间、提交信息、父提交              │
│                                                             │
│  4. tag（标签）                                              │
│     指向 commit 的命名引用                                   │
│                                                             │
│  关系图：                                                    │
│                                                             │
│  commit ──→ tree ──→ blob (file1.txt)                       │
│    │          │                                             │
│    │          └──→ blob (file2.txt)                         │
│    │          │                                             │
│    │          └──→ tree (subdir) ──→ blob (file3.txt)       │
│    ↓                                                        │
│  parent commit                                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```bash
# 查看对象类型
git cat-file -t <sha1>

# 查看对象内容
git cat-file -p <sha1>

# 查看 HEAD 指向的 commit
git cat-file -p HEAD

# 示例输出：
# tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904
# parent a1b2c3d4e5f6...
# author John <john@example.com> 1234567890 +0800
# committer John <john@example.com> 1234567890 +0800
#
# Initial commit
```

### 1.3 HEAD、分支与标签

```
┌─────────────────────────────────────────────────────────────┐
│                    引用系统                                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  HEAD                                                       │
│  ├── 指向当前所在分支（或直接指向 commit = detached HEAD）  │
│  └── 存储在 .git/HEAD                                       │
│                                                             │
│  分支 (Branch)                                              │
│  ├── 指向某个 commit 的可移动指针                           │
│  ├── 存储在 .git/refs/heads/                                │
│  └── 新提交时自动前移                                       │
│                                                             │
│  标签 (Tag)                                                 │
│  ├── 指向某个 commit 的固定指针                             │
│  ├── 轻量标签：仅指针                                       │
│  └── 附注标签：包含作者、日期、说明                         │
│                                                             │
│  示例：                                                      │
│                                                             │
│  HEAD → main → C4                                           │
│                 ↓                                           │
│  C1 ← C2 ← C3 ← C4                                          │
│         ↑                                                   │
│       feature                                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 常用命令速查

### 2.1 基础操作

```bash
# 初始化仓库
git init

# 克隆仓库
git clone <url>
git clone <url> --depth 1          # 浅克隆，只获取最新提交

# 查看状态
git status
git status -s                      # 简短格式

# 添加到暂存区
git add <file>
git add .                          # 添加所有
git add -p                         # 交互式添加（按块选择）

# 提交
git commit -m "message"
git commit -am "message"           # add + commit（仅已跟踪文件）
git commit --amend                 # 修改最后一次提交

# 查看日志
git log
git log --oneline                  # 单行显示
git log --graph --oneline --all    # 图形化显示所有分支
git log -p                         # 显示 diff
git log --stat                     # 显示统计
```

### 2.2 分支操作

```bash
# 查看分支
git branch                         # 本地分支
git branch -r                      # 远程分支
git branch -a                      # 所有分支

# 创建分支
git branch <name>
git checkout -b <name>             # 创建并切换
git switch -c <name>               # Git 2.23+ 推荐

# 切换分支
git checkout <branch>
git switch <branch>                # Git 2.23+ 推荐

# 删除分支
git branch -d <name>               # 删除已合并的分支
git branch -D <name>               # 强制删除

# 重命名分支
git branch -m <old> <new>
```

### 2.3 远程操作

```bash
# 查看远程仓库
git remote -v

# 添加远程仓库
git remote add origin <url>

# 拉取
git fetch                          # 获取远程更新，不合并
git pull                           # fetch + merge
git pull --rebase                  # fetch + rebase

# 推送
git push origin <branch>
git push -u origin <branch>        # 设置上游并推送
git push --force                   # 强制推送（危险！）
git push --force-with-lease        # 安全的强制推送
```

---

## 3. merge vs rebase（重点）

### 3.1 merge 合并

```
┌─────────────────────────────────────────────────────────────┐
│                    git merge                                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  合并前：                                                    │
│                                                             │
│        C3 ← C4  (feature)                                   │
│       /                                                     │
│  C1 ← C2 ← C5 ← C6  (main)                                  │
│                                                             │
│  执行：git checkout main && git merge feature               │
│                                                             │
│  合并后（创建合并提交 M）：                                   │
│                                                             │
│        C3 ← C4 ─────┐                                       │
│       /              ↘                                      │
│  C1 ← C2 ← C5 ← C6 ← M  (main)                              │
│                                                             │
│  特点：                                                      │
│  ✓ 保留完整历史                                              │
│  ✓ 非破坏性操作                                              │
│  ✓ 能看到分支合并点                                          │
│  ✗ 历史可能变得复杂                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 rebase 变基

```
┌─────────────────────────────────────────────────────────────┐
│                    git rebase                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  变基前：                                                    │
│                                                             │
│        C3 ← C4  (feature)                                   │
│       /                                                     │
│  C1 ← C2 ← C5 ← C6  (main)                                  │
│                                                             │
│  执行：git checkout feature && git rebase main              │
│                                                             │
│  变基后（C3、C4 被"重放"为 C3'、C4'）：                       │
│                                                             │
│  C1 ← C2 ← C5 ← C6 ← C3' ← C4'  (feature)                   │
│                 ↑                                           │
│               (main)                                        │
│                                                             │
│  特点：                                                      │
│  ✓ 线性历史，更整洁                                          │
│  ✓ 便于 code review                                         │
│  ✗ 改写历史（新的 commit hash）                              │
│  ✗ 不要对公共分支使用                                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 merge vs rebase 对比

```
┌─────────────────────────────────────────────────────────────┐
│                  merge vs rebase 对比                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  维度              merge              rebase                │
│  ─────────────────────────────────────────────────────────  │
│  历史记录          保留完整历史        线性历史              │
│  合并提交          创建合并提交        无合并提交            │
│  Commit Hash      不变               改变                   │
│  冲突处理          一次性解决          可能多次解决          │
│  公共分支          安全               危险（禁止！）         │
│  私有分支          可用               推荐                   │
│  可追溯性          高                 低                    │
│                                                             │
│  黄金法则：                                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  永远不要对已推送到公共仓库的 commit 进行 rebase！     │   │
│  │  只对本地私有分支使用 rebase。                         │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  选择建议：                                                  │
│  • 合并公共分支到私有分支 → rebase（保持线性）              │
│  • 合并私有分支到公共分支 → merge（保留历史）               │
│  • 团队协作分支 → merge（安全）                             │
│  • 个人功能分支 → rebase（整洁）                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. pull vs pull --rebase（重点）

### 4.1 git pull

```
┌─────────────────────────────────────────────────────────────┐
│                    git pull = fetch + merge                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  场景：本地有新提交 C3，远程有新提交 C4                       │
│                                                             │
│  本地：C1 ← C2 ← C3 (main)                                  │
│  远程：C1 ← C2 ← C4 (origin/main)                           │
│                                                             │
│  执行 git pull 后：                                          │
│                                                             │
│        C3 ─────────┐                                        │
│       /             ↘                                       │
│  C1 ← C2 ← C4 ← ─ ─ M  (main)                               │
│         ↑                                                   │
│    (origin/main)                                            │
│                                                             │
│  结果：产生一个合并提交 M                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 git pull --rebase

```
┌─────────────────────────────────────────────────────────────┐
│               git pull --rebase = fetch + rebase             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  场景：本地有新提交 C3，远程有新提交 C4                       │
│                                                             │
│  本地：C1 ← C2 ← C3 (main)                                  │
│  远程：C1 ← C2 ← C4 (origin/main)                           │
│                                                             │
│  执行 git pull --rebase 后：                                 │
│                                                             │
│  C1 ← C2 ← C4 ← C3'  (main)                                 │
│         ↑                                                   │
│    (origin/main)                                            │
│                                                             │
│  结果：本地提交 C3 变基到 C4 之后，成为 C3'                  │
│        历史保持线性，无合并提交                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 对比与选择

```
┌─────────────────────────────────────────────────────────────┐
│              pull vs pull --rebase 对比                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  维度              pull               pull --rebase         │
│  ─────────────────────────────────────────────────────────  │
│  内部操作          fetch + merge      fetch + rebase        │
│  历史形态          分叉+合并           线性                  │
│  合并提交          有                 无                    │
│  本地提交 Hash     不变               改变                  │
│  适用场景          协作分支           个人开发              │
│                                                             │
│  设置默认行为：                                              │
│  # 全局设置 pull 默认使用 rebase                            │
│  git config --global pull.rebase true                       │
│                                                             │
│  # 仅对当前仓库设置                                          │
│  git config pull.rebase true                                │
│                                                             │
│  # 保留合并提交的 rebase（推荐）                             │
│  git config --global pull.rebase merges                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. 冲突解决

### 5.1 冲突产生场景

```
┌─────────────────────────────────────────────────────────────┐
│                    冲突产生原因                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  两个分支修改了同一文件的同一位置：                          │
│                                                             │
│  main:     修改 file.txt 第 10 行为 "Hello"                 │
│  feature:  修改 file.txt 第 10 行为 "World"                 │
│                                                             │
│  Git 无法自动决定保留哪个修改 → 产生冲突                    │
│                                                             │
│  冲突场景：                                                  │
│  • merge 时                                                 │
│  • rebase 时                                                │
│  • pull 时                                                  │
│  • cherry-pick 时                                           │
│  • stash pop 时                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 冲突标记

```
<<<<<<< HEAD
这是当前分支的内容
=======
这是要合并进来的内容
>>>>>>> feature-branch
```

### 5.3 解决冲突流程

```bash
# 1. 查看冲突文件
git status

# 2. 手动编辑冲突文件，删除冲突标记，保留需要的内容

# 3. 标记为已解决
git add <resolved-file>

# 4. 继续操作
git merge --continue      # merge 冲突
git rebase --continue     # rebase 冲突
git commit                # pull 冲突

# 放弃解决
git merge --abort         # 放弃 merge
git rebase --abort        # 放弃 rebase
```

### 5.4 冲突解决工具

```bash
# 使用图形化工具
git mergetool

# 配置合并工具（以 VS Code 为例）
git config --global merge.tool vscode
git config --global mergetool.vscode.cmd 'code --wait $MERGED'

# 使用 ours/theirs 策略
git checkout --ours <file>      # 保留当前分支版本
git checkout --theirs <file>    # 保留合并分支版本

# merge 时指定策略
git merge -X ours <branch>      # 冲突时优先使用当前分支
git merge -X theirs <branch>    # 冲突时优先使用目标分支
```

---

## 6. 撤销操作（重点）

### 6.1 reset vs revert vs checkout

```
┌─────────────────────────────────────────────────────────────┐
│              reset vs revert vs checkout                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  命令              作用域          是否改写历史    安全性    │
│  ─────────────────────────────────────────────────────────  │
│  checkout         工作区          否             安全       │
│  reset            提交历史        是             危险       │
│  revert           提交历史        否             安全       │
│                                                             │
│  checkout - 切换分支或恢复文件                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  git checkout <branch>      # 切换分支              │   │
│  │  git checkout -- <file>     # 丢弃工作区修改        │   │
│  │  git checkout <commit> -- <file>  # 恢复到特定版本  │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  reset - 移动 HEAD 指针                                     │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  git reset --soft <commit>   # 仅移动 HEAD          │   │
│  │  git reset --mixed <commit>  # 移动 HEAD + 清空暂存 │   │
│  │  git reset --hard <commit>   # 移动 HEAD + 清空一切 │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  revert - 创建反向提交                                      │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  git revert <commit>         # 撤销指定提交         │   │
│  │  git revert HEAD             # 撤销最后一次提交     │   │
│  │  git revert HEAD~3..HEAD     # 撤销最近3次提交      │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 reset 三种模式详解

```
┌─────────────────────────────────────────────────────────────┐
│                    reset 三种模式                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  初始状态：HEAD → C3                                        │
│  执行：git reset <mode> C1                                  │
│                                                             │
│  --soft：仅移动 HEAD                                        │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  HEAD: C1                                           │   │
│  │  暂存区: C3 的内容（C2、C3 的修改被暂存）            │   │
│  │  工作区: C3 的内容                                   │   │
│  │  用途: 合并多个提交、修改提交信息                    │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  --mixed（默认）：移动 HEAD + 清空暂存区                    │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  HEAD: C1                                           │   │
│  │  暂存区: C1 的内容                                   │   │
│  │  工作区: C3 的内容（修改变成未暂存状态）             │   │
│  │  用途: 取消暂存、重新组织提交                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  --hard：移动 HEAD + 清空暂存区 + 清空工作区               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  HEAD: C1                                           │   │
│  │  暂存区: C1 的内容                                   │   │
│  │  工作区: C1 的内容（所有修改丢失！）                 │   │
│  │  用途: 完全回退、丢弃所有修改                        │   │
│  │  ⚠️ 危险操作，无法恢复未提交的修改                   │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.3 常见撤销场景

```bash
# 场景 1: 撤销工作区修改（未 add）
git checkout -- <file>
git restore <file>              # Git 2.23+

# 场景 2: 撤销暂存（已 add，未 commit）
git reset HEAD <file>
git restore --staged <file>     # Git 2.23+

# 场景 3: 修改最后一次提交
git commit --amend

# 场景 4: 撤销最后一次提交（保留修改）
git reset --soft HEAD~1

# 场景 5: 撤销最后一次提交（丢弃修改）
git reset --hard HEAD~1

# 场景 6: 撤销某次提交（公共分支安全方式）
git revert <commit>

# 场景 7: 撤销 merge
git reset --hard ORIG_HEAD      # merge 后立即撤销
git revert -m 1 <merge-commit>  # 已推送的 merge

# 场景 8: 撤销 rebase
git reflog                      # 找到 rebase 前的 HEAD
git reset --hard HEAD@{n}       # 恢复到 rebase 前
```

---

## 7. 高级操作

### 7.1 reflog - 救命神器

```bash
# reflog 记录 HEAD 的所有移动历史
git reflog

# 输出示例：
# a1b2c3d HEAD@{0}: commit: Add feature
# e4f5g6h HEAD@{1}: checkout: moving from main to feature
# i7j8k9l HEAD@{2}: reset: moving to HEAD~1
# m1n2o3p HEAD@{3}: commit: Important work (被 reset 掉的提交！)

# 恢复误删的提交
git reset --hard HEAD@{3}

# 恢复误删的分支
git reflog                      # 找到分支最后的 commit
git branch <branch-name> <sha1>

# reflog 保留期限（默认 90 天）
git config gc.reflogExpire 180.days
```

### 7.2 cherry-pick

```bash
# 将特定提交应用到当前分支
git cherry-pick <commit>

# 应用多个提交
git cherry-pick <commit1> <commit2>

# 应用范围（不包含起点）
git cherry-pick <start>..<end>

# 不自动提交（只应用修改）
git cherry-pick -n <commit>

# 解决冲突后继续
git cherry-pick --continue

# 放弃
git cherry-pick --abort

# 使用场景：
# • 将 hotfix 从 main 应用到 release 分支
# • 选择性地合并某些提交
# • 修复错误提交到错误分支的情况
```

### 7.3 stash

```bash
# 暂存当前修改
git stash
git stash save "描述信息"
git stash push -m "描述信息"    # 推荐

# 包含未跟踪文件
git stash -u

# 包含忽略的文件
git stash -a

# 查看暂存列表
git stash list

# 恢复暂存
git stash pop                   # 恢复并删除
git stash apply                 # 恢复不删除
git stash apply stash@{2}       # 恢复特定暂存

# 删除暂存
git stash drop stash@{0}
git stash clear                 # 删除所有

# 查看暂存内容
git stash show -p stash@{0}

# 从暂存创建分支
git stash branch <branch-name>

# 使用场景：
# • 临时切换分支处理紧急任务
# • 保存进行中的工作
# • 清理工作区后再拉取代码
```

### 7.4 bisect - 二分查找 Bug

```bash
# 开始二分查找
git bisect start

# 标记当前版本有 bug
git bisect bad

# 标记已知的好版本
git bisect good <commit>

# Git 会自动 checkout 中间版本，测试后标记
git bisect good    # 这个版本没问题
git bisect bad     # 这个版本有问题

# 找到引入 bug 的提交后，结束
git bisect reset

# 自动化测试
git bisect start HEAD <good-commit>
git bisect run ./test.sh        # 脚本返回 0 表示 good，非 0 表示 bad
```

### 7.5 rebase -i 交互式变基

```bash
# 交互式修改最近 n 次提交
git rebase -i HEAD~n

# 交互式修改从某个 commit 之后的所有提交
git rebase -i <commit>

# 交互式命令：
# pick   - 使用该提交
# reword - 使用该提交，但修改提交信息
# edit   - 使用该提交，但停下来修改
# squash - 合并到前一个提交，保留提交信息
# fixup  - 合并到前一个提交，丢弃提交信息
# drop   - 删除该提交

# 示例：合并最近 3 个提交为 1 个
git rebase -i HEAD~3
# 将第 2、3 个 pick 改为 squash 或 fixup

# 示例：修改某个历史提交
git rebase -i <commit>^
# 将目标 commit 的 pick 改为 edit
# 修改后：
git commit --amend
git rebase --continue
```

---

## 8. Git Hooks

### 8.1 常用 Hooks

```
┌─────────────────────────────────────────────────────────────┐
│                    Git Hooks                                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  客户端 Hooks（.git/hooks/）：                               │
│  ─────────────────────────────────────────────────────────  │
│  pre-commit        提交前执行（代码检查、格式化）            │
│  prepare-commit-msg 编辑提交信息前                          │
│  commit-msg        提交信息检查                              │
│  post-commit       提交后执行                                │
│  pre-push          推送前执行（运行测试）                    │
│  pre-rebase        变基前执行                                │
│                                                             │
│  服务端 Hooks：                                              │
│  ─────────────────────────────────────────────────────────  │
│  pre-receive       接收推送前                                │
│  update            更新分支前                                │
│  post-receive      接收推送后（触发 CI/CD）                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 Hook 示例

```bash
#!/bin/bash
# .git/hooks/pre-commit

# 检查是否有未格式化的代码
if ! command -v clang-format &> /dev/null; then
    echo "Warning: clang-format not found"
    exit 0
fi

# 获取暂存的 C++ 文件
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp|h|hpp)$')

if [ -n "$files" ]; then
    for file in $files; do
        if ! clang-format --dry-run -Werror "$file" 2>/dev/null; then
            echo "Error: $file needs formatting"
            echo "Run: clang-format -i $file"
            exit 1
        fi
    done
fi

exit 0
```

```bash
#!/bin/bash
# .git/hooks/commit-msg

# 检查提交信息格式
commit_msg=$(cat "$1")

# 要求格式：type: description
pattern="^(feat|fix|docs|style|refactor|test|chore): .+"

if ! echo "$commit_msg" | grep -qE "$pattern"; then
    echo "Error: Invalid commit message format"
    echo "Expected: type: description"
    echo "Types: feat, fix, docs, style, refactor, test, chore"
    exit 1
fi

exit 0
```

---

## 9. 工作流对比

### 9.1 Git Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    Git Flow                                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  main ─────●─────────────────────────●───────────●──→      │
│            ↑                         ↑           ↑          │
│            │                         │           │          │
│  release   │         ●───●───●───────┤           │          │
│            │        ↗               ↘│           │          │
│  develop ──●───●───●─────────────────●───●───●───●──→      │
│              ↖   ↗                     ↖   ↗              │
│  feature     ●─●                        ●─●                │
│                                                             │
│  分支类型：                                                  │
│  • main     - 生产环境代码                                  │
│  • develop  - 开发主分支                                    │
│  • feature  - 功能开发（从 develop 分出，合并回 develop）   │
│  • release  - 发布准备（从 develop 分出，合并到 main）      │
│  • hotfix   - 紧急修复（从 main 分出，合并到 main 和 develop）│
│                                                             │
│  优点：适合有计划发布周期的项目                              │
│  缺点：分支多、流程复杂                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 GitHub Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    GitHub Flow                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  main ─────●─────────────●─────────────●─────────────→     │
│            ↑             ↑             ↑                    │
│            │             │             │                    │
│  feature1  ●───●───●─────┘             │                    │
│                                        │                    │
│  feature2          ●───●───●───●───────┘                    │
│                                                             │
│  流程：                                                      │
│  1. 从 main 创建功能分支                                    │
│  2. 在功能分支上开发、提交                                  │
│  3. 创建 Pull Request                                       │
│  4. Code Review                                             │
│  5. 合并到 main                                             │
│  6. 部署                                                    │
│                                                             │
│  优点：简单、适合持续部署                                    │
│  缺点：不适合多版本并行维护                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.3 工作流对比

```
┌─────────────────────────────────────────────────────────────┐
│                    工作流对比                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  工作流          复杂度    适用场景                          │
│  ─────────────────────────────────────────────────────────  │
│  Git Flow       高        有明确版本发布周期的大型项目       │
│  GitHub Flow    低        持续部署的 Web 项目               │
│  GitLab Flow    中        需要多环境部署的项目               │
│  Trunk Based    低        高频发布、成熟团队                 │
│                                                             │
│  Trunk Based Development：                                  │
│  • 所有人直接在 main 开发                                   │
│  • 使用 Feature Flag 控制功能发布                           │
│  • 要求：强大的 CI/CD、充分的测试覆盖                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 10. 面试常见问题

### Q1: git fetch 和 git pull 的区别？

```
git fetch：
  • 从远程仓库下载最新数据
  • 不自动合并到本地分支
  • 更新远程跟踪分支（origin/main）
  • 安全操作，不影响本地工作

git pull：
  • = git fetch + git merge（默认）
  • 下载数据并自动合并到当前分支
  • 可能产生合并冲突
  • 可用 --rebase 改为 fetch + rebase

推荐：先 fetch 查看变更，再决定 merge/rebase
```

### Q2: merge 和 rebase 什么时候用？

```
使用 merge：
  • 合并公共分支（如 develop → main）
  • 保留完整的分支历史
  • 团队协作的共享分支
  • 需要追溯分支合并点

使用 rebase：
  • 同步上游更新到私有分支
  • 整理本地提交历史（交互式 rebase）
  • 保持线性历史
  • 提交 PR 前整理 commits

黄金法则：不要 rebase 已推送的公共分支
```

### Q3: 如何撤销已推送的提交？

```bash
# 方式 1: revert（推荐，安全）
git revert <commit>
git push

# 方式 2: reset + force push（危险）
git reset --hard <commit>
git push --force-with-lease  # 比 --force 安全

# 撤销已推送的 merge
git revert -m 1 <merge-commit>  # -m 1 指定主线
```

### Q4: HEAD~n 和 HEAD^n 的区别？

```
HEAD~n：沿着第一父提交回溯 n 次
  HEAD~1 = 父提交
  HEAD~2 = 祖父提交

HEAD^n：第 n 个父提交（用于 merge commit）
  HEAD^1 = 第一个父提交（当前分支）
  HEAD^2 = 第二个父提交（被合并的分支）

对于普通提交，HEAD^ = HEAD~1
对于 merge 提交：
  HEAD^1 = 合并前的主分支提交
  HEAD^2 = 被合并进来的分支提交
```

### Q5: 如何找回误删的分支或提交？

```bash
# 使用 reflog
git reflog

# 找到误删的 commit hash
git branch <branch-name> <sha1>     # 恢复分支
git cherry-pick <sha1>              # 恢复提交
git reset --hard <sha1>             # 恢复 HEAD

# reflog 也能恢复误操作的 rebase
git reflog
git reset --hard HEAD@{n}
```

### Q6: git reset --hard 后如何恢复？

```bash
# 如果还没有 gc，reflog 中还有记录
git reflog
git reset --hard HEAD@{n}

# 如果已经 gc，尝试 fsck 找到悬空对象
git fsck --lost-found
# 检查 .git/lost-found/

# 预防：重要操作前创建备份分支
git branch backup-$(date +%Y%m%d)
```

### Q7: 如何合并特定文件的修改？

```bash
# 方式 1: checkout 特定文件
git checkout <branch> -- <file>

# 方式 2: 使用 patch
git diff <branch> -- <file> | git apply

# 方式 3: cherry-pick + reset
git cherry-pick -n <commit>    # 不自动提交
git reset HEAD                 # 取消暂存
git add <file>                 # 只添加需要的文件
git checkout -- .              # 丢弃其他修改
git commit
```

### Q8: .gitignore 不生效怎么办？

```bash
# 原因：文件已被跟踪

# 解决：从索引中移除（保留本地文件）
git rm --cached <file>
git rm -r --cached <directory>

# 提交 .gitignore 和删除操作
git add .gitignore
git commit -m "Update .gitignore"

# 全局忽略（个人配置，不提交到仓库）
git config --global core.excludesFile ~/.gitignore_global
```

### Q9: 如何修改历史提交的作者信息？

```bash
# 修改最后一次提交
git commit --amend --author="Name <email@example.com>"

# 修改历史提交
git rebase -i <commit>^
# 将目标 commit 的 pick 改为 edit
git commit --amend --author="Name <email@example.com>"
git rebase --continue

# 批量修改（慎用）
git filter-branch --env-filter '
if [ "$GIT_AUTHOR_EMAIL" = "old@email.com" ]; then
    export GIT_AUTHOR_NAME="New Name"
    export GIT_AUTHOR_EMAIL="new@email.com"
fi
' --tag-name-filter cat -- --all
```

### Q10: git stash 和新建分支保存修改有什么区别？

```
git stash：
  • 临时保存，适合短期切换
  • 存储在专门的 stash 栈中
  • 可跨分支应用
  • 不创建提交记录

新建分支：
  • 长期保存，适合功能开发中途切换
  • 创建正式的分支和提交
  • 有完整的历史记录
  • 可推送到远程

建议：
  • 短暂切换（< 1天）→ stash
  • 长期保存或需要协作 → 新分支
```

---

## 11. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                    Git 知识图谱                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  基础概念                                                   │
│  ├── 三个区域（工作区/暂存区/仓库）                          │
│  ├── 对象模型（blob/tree/commit/tag）                       │
│  └── 引用系统（HEAD/分支/标签）                             │
│                                                             │
│  分支与合并                                                  │
│  ├── merge vs rebase                                        │
│  ├── pull vs pull --rebase                                  │
│  └── 冲突解决策略                                           │
│                                                             │
│  撤销操作                                                   │
│  ├── reset（--soft/--mixed/--hard）                         │
│  ├── revert（安全撤销）                                     │
│  └── checkout/restore（恢复文件）                           │
│                                                             │
│  高级操作                                                   │
│  ├── reflog（救命神器）                                     │
│  ├── cherry-pick（选择性合并）                              │
│  ├── stash（临时保存）                                      │
│  ├── bisect（二分查找 Bug）                                 │
│  └── rebase -i（交互式变基）                                │
│                                                             │
│  工程实践                                                   │
│  ├── Git Hooks（自动化）                                    │
│  ├── 工作流（Git Flow/GitHub Flow）                         │
│  └── .gitignore 配置                                        │
│                                                             │
│  面试重点                                                   │
│  ├── merge vs rebase 区别与选择                             │
│  ├── pull vs pull --rebase                                  │
│  ├── reset 三种模式                                         │
│  ├── 如何恢复误删内容                                       │
│  └── 工作流选择                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
