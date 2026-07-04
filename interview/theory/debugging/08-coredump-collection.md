# 崩溃收集系统 (Crash Collection System)

崩溃收集系统用于自动捕获、上报和分析生产环境的程序崩溃。

---

## 1. 崩溃收集概述

### 1.1 为什么需要崩溃收集

```
┌─────────────────────────────────────────────────────────────┐
│                   崩溃收集的必要性                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  生产环境问题：                                              │
│  • 无法附加调试器                                            │
│  • 用户环境难以复现                                          │
│  • 崩溃发生后程序已终止                                      │
│  • 需要收集多个用户的崩溃数据                                │
│                                                             │
│  崩溃收集系统功能：                                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  1. 捕获崩溃：捕获程序崩溃，生成 dump                  │   │
│  │  2. 收集信息：系统信息、日志、用户操作                 │   │
│  │  3. 上报服务：将崩溃数据发送到服务器                   │   │
│  │  4. 符号化：将地址转换为函数名和行号                   │   │
│  │  5. 聚合分析：相同崩溃分组，统计频率                   │   │
│  │  6. 告警通知：新崩溃或崩溃率上升时告警                 │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  价值：                                                      │
│  • 发现线上问题                                              │
│  • 优先级排序（按影响用户数）                                │
│  • 版本质量监控                                              │
│  • 快速定位和修复                                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 崩溃收集架构

```
┌─────────────────────────────────────────────────────────────┐
│                   崩溃收集系统架构                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  客户端                        服务端                        │
│  ┌─────────┐                  ┌──────────────────────┐     │
│  │ 应用程序 │                  │    接收服务          │     │
│  │  ↓崩溃   │                  │    (HTTP API)       │     │
│  │ 崩溃处理│────────────────→ │         ↓           │     │
│  │ (捕获)  │     上报 dump    │    存储服务          │     │
│  │  ↓      │                  │    (S3/OSS)         │     │
│  │ 生成dump│                  │         ↓           │     │
│  └─────────┘                  │    符号化服务        │     │
│                               │    (Symbol Server)  │     │
│                               │         ↓           │     │
│                               │    分析聚合          │     │
│                               │    (按栈分组)       │     │
│                               │         ↓           │     │
│                               │    展示/告警         │     │
│                               │    (Dashboard)      │     │
│                               └──────────────────────┘     │
│                                                             │
│  符号管理：                                                  │
│  ┌─────────┐    构建    ┌─────────┐    符号化    ┌───────┐│
│  │ 源代码  │ ─────────→ │符号文件  │ ───────────→ │地址解析││
│  │         │            │PDB/dSYM│              │       ││
│  └─────────┘            └─────────┘              └───────┘│
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Google Breakpad

### 2.1 Breakpad 概述

```
┌─────────────────────────────────────────────────────────────┐
│                    Google Breakpad                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  简介：                                                      │
│  Google 开源的跨平台崩溃报告框架                            │
│  支持 Windows、Linux、macOS、Android、iOS                  │
│                                                             │
│  核心组件：                                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Client Library                                      │   │
│  │  • exception_handler: 异常捕获                       │   │
│  │  • minidump_writer: 生成 minidump                   │   │
│  └─────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Tools                                               │   │
│  │  • dump_syms: 从二进制提取符号                       │   │
│  │  • minidump_stackwalk: 符号化 minidump              │   │
│  │  • symupload: 上传符号到服务器                       │   │
│  └─────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Processor Library                                   │   │
│  │  • 解析 minidump                                     │   │
│  │  • 符号化调用栈                                      │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Breakpad 集成 (C++)

```cpp
// === Windows 集成 ===
#include "client/windows/handler/exception_handler.h"

// 回调函数：dump 生成后调用
bool DumpCallback(const wchar_t* dump_path,
                  const wchar_t* minidump_id,
                  void* context,
                  EXCEPTION_POINTERS* exinfo,
                  MDRawAssertionInfo* assertion,
                  bool succeeded) {
    if (succeeded) {
        wprintf(L"Dump generated: %s\\%s.dmp\n", dump_path, minidump_id);
        // 上传 dump 到服务器
        UploadDump(dump_path, minidump_id);
    }
    return succeeded;
}

int main() {
    // 创建 exception handler
    google_breakpad::ExceptionHandler* handler =
        new google_breakpad::ExceptionHandler(
            L"C:\\CrashDumps",    // dump 保存路径
            nullptr,              // filter callback
            DumpCallback,         // minidump callback
            nullptr,              // callback context
            google_breakpad::ExceptionHandler::HANDLER_ALL
        );

    // 应用程序代码...

    delete handler;
    return 0;
}

// === Linux 集成 ===
#include "client/linux/handler/exception_handler.h"

bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
                  void* context, bool succeeded) {
    if (succeeded) {
        printf("Dump generated: %s\n", descriptor.path());
    }
    return succeeded;
}

int main() {
    google_breakpad::MinidumpDescriptor descriptor("/tmp/crashes");
    google_breakpad::ExceptionHandler handler(
        descriptor,
        nullptr,        // filter
        DumpCallback,
        nullptr,        // context
        true,           // install handler
        -1              // server fd
    );

    // 应用程序代码...

    return 0;
}

// === macOS 集成 ===
#include "client/mac/handler/exception_handler.h"

bool DumpCallback(const char* dump_dir, const char* minidump_id,
                  void* context, bool succeeded) {
    printf("Dump: %s/%s.dmp\n", dump_dir, minidump_id);
    return succeeded;
}

int main() {
    google_breakpad::ExceptionHandler handler(
        "/tmp/crashes",   // dump 目录
        nullptr,          // filter
        DumpCallback,
        nullptr,          // context
        true,             // install handler
        nullptr           // port name
    );

    // 应用程序代码...

    return 0;
}
```

### 2.3 符号处理

```bash
# 1. 生成符号文件

# Windows (从 PDB 生成)
dump_syms.exe MyApp.exe MyApp.pdb > MyApp.sym

# Linux (从带调试信息的 ELF 生成)
dump_syms ./MyApp > MyApp.sym

# macOS (从 dSYM 生成)
dump_syms -g MyApp.dSYM/Contents/Resources/DWARF/MyApp MyApp > MyApp.sym

# 2. 符号文件格式
# 第一行包含模块信息
# MODULE <os> <arch> <id> <name>
# MODULE Linux x86_64 ABCD1234... MyApp

# 3. 组织符号目录结构
# symbols/
# └── MyApp/
#     └── ABCD1234.../
#         └── MyApp.sym

# 4. 符号化 minidump
minidump_stackwalk crash.dmp symbols/ > crash_report.txt

# 输出示例：
# Thread 0 (crashed)
#  0  MyApp!crash_function [main.cpp : 42 + 0x5]
#  1  MyApp!main [main.cpp : 100 + 0x12]
#  2  libc.so.6!__libc_start_main + 0xf3
```

### 2.4 符号服务器

```python
# 简单的符号服务器 (Python Flask)
from flask import Flask, request, send_file
import os

app = Flask(__name__)
SYMBOL_PATH = '/var/symbols'

@app.route('/symbols/<module>/<build_id>/<filename>')
def get_symbol(module, build_id, filename):
    """符号文件请求格式：/symbols/{module}/{build_id}/{filename}"""
    path = os.path.join(SYMBOL_PATH, module, build_id, filename)
    if os.path.exists(path):
        return send_file(path)
    return 'Not Found', 404

@app.route('/upload', methods=['POST'])
def upload_symbol():
    """上传符号文件"""
    file = request.files['symbol']
    # 解析符号文件获取模块信息
    first_line = file.readline().decode()
    parts = first_line.split()
    # MODULE <os> <arch> <build_id> <module_name>
    build_id = parts[3]
    module_name = parts[4]

    # 保存到正确路径
    dir_path = os.path.join(SYMBOL_PATH, module_name, build_id)
    os.makedirs(dir_path, exist_ok=True)
    file.seek(0)
    file.save(os.path.join(dir_path, f'{module_name}.sym'))

    return 'OK'

if __name__ == '__main__':
    app.run(port=8080)
```

---

## 3. Google Crashpad

### 3.1 Crashpad vs Breakpad

```
┌─────────────────────────────────────────────────────────────┐
│                 Crashpad vs Breakpad                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  特性              Breakpad           Crashpad              │
│  ─────────────────────────────────────────────────────────  │
│  架构              进程内             进程外（独立进程）    │
│  稳定性            可能被崩溃影响      更稳定               │
│  复杂度            简单               复杂                  │
│  维护状态          维护模式            活跃开发             │
│  附加信息          基础               丰富                  │
│  上报方式          回调处理            自动后台上报         │
│                                                             │
│  Crashpad 优势：                                             │
│  • 独立 handler 进程，不受崩溃进程状态影响                  │
│  • 支持附加任意键值对信息                                   │
│  • 更好的 Windows 支持                                      │
│  • 支持限流和排队                                           │
│                                                             │
│  架构对比：                                                  │
│                                                             │
│  Breakpad:                                                  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  应用进程                                              │  │
│  │  ┌─────────────────┐                                  │  │
│  │  │ Exception Handler│→ 生成 dump → 回调上报           │  │
│  │  └─────────────────┘                                  │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
│  Crashpad:                                                  │
│  ┌─────────────────┐     ┌─────────────────────────────┐  │
│  │  应用进程        │ IPC │  Crashpad Handler 进程       │  │
│  │  ┌────────────┐ │←──→ │  • 生成 dump                │  │
│  │  │ Client     │ │     │  • 上传到服务器              │  │
│  │  └────────────┘ │     │  • 管理排队                  │  │
│  └─────────────────┘     └─────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Crashpad 集成

```cpp
// === Windows Crashpad 集成 ===
#include "client/crashpad_client.h"
#include "client/crash_report_database.h"
#include "client/settings.h"

bool InitializeCrashpad() {
    using namespace crashpad;

    // 配置路径
    base::FilePath handler(L"crashpad_handler.exe");
    base::FilePath reports_dir(L"C:\\CrashReports");
    base::FilePath metrics_dir(L"C:\\CrashMetrics");

    // 上报 URL
    std::string url = "https://crash.example.com/submit";

    // 附加信息
    std::map<std::string, std::string> annotations;
    annotations["product"] = "MyApp";
    annotations["version"] = "1.0.0";
    annotations["build"] = "12345";

    // 文件附件
    std::vector<base::FilePath> attachments;
    attachments.push_back(base::FilePath(L"C:\\Logs\\app.log"));

    // 启动 Crashpad
    CrashpadClient client;
    bool success = client.StartHandler(
        handler,
        reports_dir,
        metrics_dir,
        url,
        annotations,
        {"--no-rate-limit"},  // 参数
        true,                  // 重启 handler
        true                   // 异步启动
    );

    if (success) {
        // 设置上报策略
        std::unique_ptr<CrashReportDatabase> database =
            CrashReportDatabase::Initialize(reports_dir);
        if (database) {
            Settings* settings = database->GetSettings();
            settings->SetUploadsEnabled(true);
        }
    }

    return success;
}

// 添加自定义信息
void AddCrashAnnotation(const std::string& key, const std::string& value) {
    static crashpad::SimpleStringDictionary* annotations = nullptr;
    if (!annotations) {
        annotations = new crashpad::SimpleStringDictionary();
        crashpad::CrashpadInfo::GetCrashpadInfo()->set_simple_annotations(
            annotations);
    }
    annotations->SetKeyValue(key.c_str(), value.c_str());
}

int main() {
    if (!InitializeCrashpad()) {
        fprintf(stderr, "Failed to initialize Crashpad\n");
    }

    // 添加运行时信息
    AddCrashAnnotation("user_id", "12345");
    AddCrashAnnotation("last_action", "open_file");

    // 应用程序代码...

    return 0;
}
```

### 3.3 服务端接收

```python
# Crashpad minidump 接收服务
from flask import Flask, request
import os
import uuid

app = Flask(__name__)
UPLOAD_DIR = '/var/crash_reports'

@app.route('/submit', methods=['POST'])
def submit_crash():
    """接收 Crashpad 上报的崩溃"""
    # Crashpad 使用 multipart/form-data 上传

    # 获取 minidump 文件
    if 'upload_file_minidump' not in request.files:
        return 'No minidump', 400

    minidump = request.files['upload_file_minidump']

    # 获取附加信息
    annotations = {}
    for key in request.form:
        annotations[key] = request.form[key]

    # 生成唯一 ID
    crash_id = str(uuid.uuid4())

    # 保存 minidump
    crash_dir = os.path.join(UPLOAD_DIR, crash_id)
    os.makedirs(crash_dir, exist_ok=True)
    minidump.save(os.path.join(crash_dir, 'minidump.dmp'))

    # 保存附加信息
    import json
    with open(os.path.join(crash_dir, 'annotations.json'), 'w') as f:
        json.dump(annotations, f)

    # 保存附件
    for key in request.files:
        if key != 'upload_file_minidump':
            file = request.files[key]
            file.save(os.path.join(crash_dir, key))

    # 触发后台处理
    process_crash.delay(crash_id)

    return crash_id, 200

if __name__ == '__main__':
    app.run(port=8080)
```

---

## 4. 符号服务器

### 4.1 符号存储结构

```
┌─────────────────────────────────────────────────────────────┐
│                    符号存储结构                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Breakpad 格式：                                             │
│  symbols/                                                   │
│  └── <module_name>/                                        │
│      └── <debug_id>/                                       │
│          └── <module_name>.sym                             │
│                                                             │
│  示例：                                                      │
│  symbols/                                                   │
│  ├── MyApp/                                                │
│  │   └── ABCD123456789/                                   │
│  │       └── MyApp.sym                                    │
│  └── libc.so.6/                                           │
│      └── 1234ABCD.../                                     │
│          └── libc.so.6.sym                                │
│                                                             │
│  Microsoft 格式（PDB）：                                     │
│  symbols/                                                   │
│  └── <filename>.pdb/                                       │
│      └── <GUID><Age>/                                      │
│          └── <filename>.pdb                                │
│                                                             │
│  示例：                                                      │
│  symbols/                                                   │
│  └── MyApp.pdb/                                            │
│      └── 123456789ABCDEF01/                               │
│          └── MyApp.pdb                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 符号上传

```bash
# CI/CD 构建后上传符号

# Breakpad 符号
#!/bin/bash
MODULE=$1
BUILD_ID=$(head -1 ${MODULE}.sym | awk '{print $4}')
MODULE_NAME=$(head -1 ${MODULE}.sym | awk '{print $5}')

# 上传到符号服务器
curl -X POST "https://symbols.example.com/upload" \
    -F "symbol=@${MODULE}.sym" \
    -F "build_id=${BUILD_ID}" \
    -F "module=${MODULE_NAME}"

# 或复制到共享存储
mkdir -p /symbols/${MODULE_NAME}/${BUILD_ID}
cp ${MODULE}.sym /symbols/${MODULE_NAME}/${BUILD_ID}/

# Windows PDB 上传（使用 symstore）
symstore add /r /f *.pdb /s \\symbols\symbols /t "MyApp" /v "1.0.0"
```

### 4.3 自动化符号管理

```python
# 符号管理服务
import os
import subprocess
import shutil
from dataclasses import dataclass

@dataclass
class SymbolInfo:
    module: str
    build_id: str
    platform: str
    version: str

class SymbolManager:
    def __init__(self, symbol_path: str):
        self.symbol_path = symbol_path

    def extract_symbols(self, binary_path: str, output_path: str) -> SymbolInfo:
        """从二进制文件提取符号"""
        # 调用 dump_syms
        result = subprocess.run(
            ['dump_syms', binary_path],
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            raise Exception(f"dump_syms failed: {result.stderr}")

        # 解析输出
        lines = result.stdout.split('\n')
        first_line = lines[0].split()
        # MODULE <platform> <arch> <build_id> <module_name>

        info = SymbolInfo(
            module=first_line[4],
            build_id=first_line[3],
            platform=first_line[1],
            version=''
        )

        # 保存符号文件
        with open(output_path, 'w') as f:
            f.write(result.stdout)

        return info

    def upload_symbol(self, symbol_path: str, info: SymbolInfo):
        """上传符号到存储"""
        dest_dir = os.path.join(
            self.symbol_path,
            info.module,
            info.build_id
        )
        os.makedirs(dest_dir, exist_ok=True)
        shutil.copy(symbol_path, os.path.join(dest_dir, f"{info.module}.sym"))

    def get_symbol(self, module: str, build_id: str) -> str:
        """获取符号文件路径"""
        path = os.path.join(
            self.symbol_path,
            module,
            build_id,
            f"{module}.sym"
        )
        if os.path.exists(path):
            return path
        return None

    def cleanup_old_symbols(self, keep_days: int = 30):
        """清理旧符号"""
        import time
        cutoff = time.time() - keep_days * 86400

        for root, dirs, files in os.walk(self.symbol_path):
            for file in files:
                path = os.path.join(root, file)
                if os.path.getmtime(path) < cutoff:
                    os.remove(path)
                    print(f"Removed: {path}")
```

---

## 5. 崩溃分析服务

### 5.1 崩溃处理流程

```
┌─────────────────────────────────────────────────────────────┐
│                    崩溃处理流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 接收崩溃报告                                             │
│     └── 保存 minidump 和附加信息                            │
│                                                             │
│  2. 符号化                                                   │
│     ├── 解析 minidump 获取模块列表                          │
│     ├── 从符号服务器获取对应符号                            │
│     └── 生成可读的调用栈                                    │
│                                                             │
│  3. 聚合分组                                                 │
│     ├── 计算崩溃签名（基于调用栈）                          │
│     └── 相同签名的崩溃归为一组                              │
│                                                             │
│  4. 存储入库                                                 │
│     ├── 崩溃详情                                            │
│     ├── 统计信息                                            │
│     └── 趋势数据                                            │
│                                                             │
│  5. 告警通知                                                 │
│     ├── 新崩溃类型                                          │
│     ├── 崩溃率上升                                          │
│     └── 关键崩溃                                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 崩溃签名算法

```python
import hashlib

def compute_crash_signature(stacktrace: list) -> str:
    """
    计算崩溃签名
    相同崩溃应该有相同签名
    """
    # 提取关键帧（排除系统库等）
    key_frames = []
    for frame in stacktrace:
        # 跳过系统库
        if is_system_module(frame['module']):
            continue
        # 跳过不可靠的帧
        if frame.get('trust') == 'scan':
            continue
        # 使用函数名（不含偏移）
        key_frames.append(f"{frame['module']}!{frame['function']}")
        # 最多取前 5 个关键帧
        if len(key_frames) >= 5:
            break

    if not key_frames:
        # 没有有效帧，使用崩溃地址
        return f"unknown_{stacktrace[0].get('address', 'no_addr')}"

    # 生成签名
    signature_str = '|'.join(key_frames)
    return hashlib.md5(signature_str.encode()).hexdigest()[:16]

def is_system_module(module: str) -> bool:
    """判断是否是系统模块"""
    system_prefixes = [
        'ntdll', 'kernel32', 'kernelbase',  # Windows
        'libc', 'libpthread', 'ld-linux',    # Linux
        'libsystem', 'libdyld',              # macOS
    ]
    module_lower = module.lower()
    return any(module_lower.startswith(p) for p in system_prefixes)
```

### 5.3 崩溃分析服务实现

```python
import subprocess
import json
from typing import Optional

class CrashAnalyzer:
    def __init__(self, symbol_path: str):
        self.symbol_path = symbol_path

    def analyze(self, minidump_path: str) -> dict:
        """分析 minidump 文件"""
        # 1. 使用 minidump_stackwalk 符号化
        result = subprocess.run(
            ['minidump_stackwalk', '-m', minidump_path, self.symbol_path],
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            return {'error': result.stderr}

        # 2. 解析输出
        report = self._parse_stackwalk_output(result.stdout)

        # 3. 计算签名
        if report.get('crash_thread'):
            report['signature'] = compute_crash_signature(
                report['crash_thread']['frames']
            )

        return report

    def _parse_stackwalk_output(self, output: str) -> dict:
        """解析 minidump_stackwalk 输出"""
        report = {
            'os': '',
            'cpu': '',
            'crash_reason': '',
            'crash_address': '',
            'threads': [],
            'crash_thread': None,
            'modules': []
        }

        lines = output.split('\n')
        current_thread = None

        for line in lines:
            if line.startswith('Operating system:'):
                report['os'] = line.split(':', 1)[1].strip()
            elif line.startswith('CPU:'):
                report['cpu'] = line.split(':', 1)[1].strip()
            elif line.startswith('Crash reason:'):
                report['crash_reason'] = line.split(':', 1)[1].strip()
            elif line.startswith('Crash address:'):
                report['crash_address'] = line.split(':', 1)[1].strip()
            elif line.startswith('Thread'):
                # 新线程
                if current_thread:
                    report['threads'].append(current_thread)
                current_thread = {
                    'id': line.split()[1],
                    'crashed': 'crashed' in line.lower(),
                    'frames': []
                }
            elif current_thread and line.strip().startswith(tuple('0123456789')):
                # 栈帧
                frame = self._parse_frame(line)
                if frame:
                    current_thread['frames'].append(frame)

        if current_thread:
            report['threads'].append(current_thread)

        # 找到崩溃线程
        for thread in report['threads']:
            if thread['crashed']:
                report['crash_thread'] = thread
                break

        return report

    def _parse_frame(self, line: str) -> Optional[dict]:
        """解析栈帧行"""
        # 格式: <frame_num> <module> <function> [<file>:<line>] + <offset>
        parts = line.split()
        if len(parts) < 3:
            return None

        frame = {
            'index': int(parts[0]),
            'module': parts[1] if parts[1] != '???' else 'unknown',
            'function': parts[2] if len(parts) > 2 else 'unknown',
            'file': None,
            'line': None,
            'offset': None
        }

        # 解析文件和行号
        for i, part in enumerate(parts[3:], 3):
            if ':' in part and part[0] != '+':
                file_line = part.split(':')
                frame['file'] = file_line[0]
                frame['line'] = int(file_line[1]) if len(file_line) > 1 else None
            elif part.startswith('+'):
                frame['offset'] = part[1:]

        return frame
```

---

## 6. 商业崩溃收集服务

### 6.1 常见服务对比

```
┌─────────────────────────────────────────────────────────────┐
│                  商业崩溃收集服务对比                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  服务          平台支持              主要特点               │
│  ─────────────────────────────────────────────────────────  │
│  Sentry        全平台               开源可自建，功能全面    │
│  Bugly         移动端/桌面          腾讯出品，国内友好      │
│  Firebase      Android/iOS         Google 服务，免费        │
│  Crashlytics                                                │
│  Backtrace     全平台               专业级，企业功能        │
│  Raygun        全平台               APM 集成               │
│  HockeyApp/    全平台               微软服务，集成 VS      │
│  AppCenter                                                  │
│                                                             │
│  选择建议：                                                  │
│  • 初创团队：Sentry（免费额度）或 Firebase                  │
│  • 国内应用：Bugly（免费，稳定）                            │
│  • 企业级：Sentry 自建或 Backtrace                         │
│  • 纯 C++：Breakpad/Crashpad + 自建服务                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 Sentry 集成 (C++)

```cpp
// Sentry Native SDK 集成
#include <sentry.h>

int main() {
    // 初始化 Sentry
    sentry_options_t* options = sentry_options_new();
    sentry_options_set_dsn(options,
        "https://key@sentry.io/project_id");
    sentry_options_set_release(options, "my-app@1.0.0");
    sentry_options_set_environment(options, "production");

    // 设置 handler 路径（Crashpad）
    sentry_options_set_handler_path(options, "crashpad_handler");
    sentry_options_set_database_path(options, ".sentry-native");

    sentry_init(options);

    // 添加用户信息
    sentry_value_t user = sentry_value_new_object();
    sentry_value_set_by_key(user, "id",
        sentry_value_new_string("12345"));
    sentry_value_set_by_key(user, "username",
        sentry_value_new_string("john"));
    sentry_set_user(user);

    // 添加标签
    sentry_set_tag("feature", "main_module");

    // 添加上下文
    sentry_value_t ctx = sentry_value_new_object();
    sentry_value_set_by_key(ctx, "last_action",
        sentry_value_new_string("open_file"));
    sentry_set_context("app_state", ctx);

    // 手动捕获异常
    sentry_value_t event = sentry_value_new_event();
    sentry_value_set_by_key(event, "message",
        sentry_value_new_string("Something went wrong"));
    sentry_capture_event(event);

    // 应用程序代码...

    // 关闭 Sentry
    sentry_close();
    return 0;
}

// CMakeLists.txt
// find_package(sentry REQUIRED)
// target_link_libraries(myapp PRIVATE sentry::sentry)
```

---

## 7. 面试深度问题

### Q1: Breakpad 和 Crashpad 如何选择？

**答案**：

```
选择依据：

Breakpad 适合：
• 简单集成需求
• 自定义处理流程
• 已有成熟的上报系统
• 资源受限环境

Crashpad 适合：
• 需要高可靠性
• Windows 应用
• 需要丰富的附加信息
• 希望自动后台上报

技术对比：
Breakpad: 进程内处理
- 崩溃时 handler 代码也在崩溃进程
- 如果进程状态损坏，可能无法生成 dump

Crashpad: 进程外处理
- 独立的 handler 进程
- 崩溃进程状态不影响 dump 生成
- 但需要额外的进程管理

现代项目建议使用 Crashpad
```

### Q2: 如何保证符号文件与二进制匹配？

**答案**：

```
匹配机制：

1. Build ID
   - ELF: .note.gnu.build-id section
   - PE: PDB GUID + Age
   - Mach-O: LC_UUID

2. 构建流程
   - 构建时保存 Build ID
   - 符号文件按 Build ID 组织
   - 上传符号时记录 Build ID

3. 验证步骤
   从 minidump 提取模块 Build ID
   ↓
   在符号服务器查找匹配符号
   ↓
   符号化时验证 Build ID 一致

4. 常见问题
   - 未上传符号：符号化失败
   - Build ID 不匹配：显示错误函数
   - 增量构建：Build ID 未更新

最佳实践：
- CI/CD 自动上传符号
- 保留历史符号（至少保留线上版本的）
- 使用符号服务器管理
```

### Q3: 崩溃聚合的签名算法如何设计？

**答案**：

```
设计原则：

1. 稳定性
   相同根因的崩溃应该有相同签名
   - 忽略地址偏移
   - 忽略参数值

2. 区分度
   不同崩溃应该有不同签名
   - 使用函数名
   - 考虑多个栈帧

3. 噪声过滤
   排除干扰因素
   - 系统库帧
   - 不可靠帧（scan）
   - 内联帧

常用算法：
def signature(stack):
    frames = filter_noise(stack)  # 过滤噪声
    top_frames = frames[:5]       # 取 top 5
    names = [f.module + f.function for f in top_frames]
    return hash(join(names))

高级策略：
- 按崩溃类型使用不同算法
- 机器学习辅助聚合
- 人工标注反馈
```

### Q4: 如何处理符号化失败的崩溃？

**答案**：

```
处理策略：

1. 记录原始信息
   - 保存 minidump
   - 记录模块和 Build ID
   - 后续可以重新符号化

2. 部分符号化
   - 系统库符号化（公共符号服务器）
   - 应用模块显示偏移

3. 补充符号
   - 通知开发上传缺失符号
   - 后台重新处理

4. 模糊匹配
   - 如果差异很小，尝试相近版本符号
   - 标记为"近似符号化"

5. 聚合策略
   - 按模块+偏移聚合
   - 补充符号后重新分组

流程：
缺失符号 → 记录日志 → 通知开发 → 上传符号 → 重新处理
```

### Q5: 如何设计崩溃率告警？

**答案**：

```
告警维度：

1. 整体崩溃率
   崩溃次数 / 活跃用户数
   阈值：0.1% - 1%

2. 版本崩溃率
   新版本崩溃率 vs 旧版本
   阈值：上升 50% 告警

3. 新崩溃类型
   首次出现的签名
   立即告警

4. 崩溃数趋势
   环比/同比增长
   阈值：增长 100%

告警策略：
- 分级：P0（紧急）、P1（高）、P2（中）
- 抑制：短时间内相同告警只发一次
- 聚合：多个相关告警合并

实现示例：
class CrashAlertRule:
    def check_overall_rate(self, current_rate, threshold=0.01):
        if current_rate > threshold:
            alert(P1, f"Overall crash rate {current_rate:.2%}")

    def check_new_crash(self, signature, count):
        if is_new_signature(signature) and count > 10:
            alert(P0, f"New crash type: {signature}")

    def check_rate_increase(self, version, old_rate, new_rate):
        if new_rate > old_rate * 1.5:
            alert(P1, f"Version {version} crash rate increased")
```

---

## 8. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                  崩溃收集系统知识图谱                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  客户端 SDK                                                 │
│  ├── Google Breakpad: 进程内，简单                          │
│  ├── Google Crashpad: 进程外，稳定                          │
│  └── Sentry Native: 商业 SDK                               │
│                                                             │
│  符号管理                                                   │
│  ├── dump_syms: 符号提取                                   │
│  ├── minidump_stackwalk: 符号化                            │
│  └── Symbol Server: 符号存储                               │
│                                                             │
│  服务端处理                                                  │
│  ├── 接收上报                                               │
│  ├── 符号化                                                 │
│  ├── 聚合分组                                               │
│  └── 告警通知                                               │
│                                                             │
│  商业服务                                                   │
│  ├── Sentry: 开源可自建                                    │
│  ├── Bugly: 腾讯，国内                                     │
│  └── Firebase: Google，移动端                              │
│                                                             │
│  面试重点                                                   │
│  ├── Breakpad vs Crashpad                                  │
│  ├── 符号匹配机制                                           │
│  ├── 签名算法设计                                           │
│  └── 告警策略                                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
