# 23 - 文件与 IO (File and I/O)

## 简介

文件 IO 是程序与外部世界交互的重要方式。C++ 提供了多种文件操作方式，从 C 风格的 FILE 到现代的 std::filesystem。

**核心概念**：
- **C++ 流**：`ifstream`、`ofstream`、`fstream`
- **C 文件 API**：`fopen`、`fread`、`fwrite`
- **文件系统**：`std::filesystem`（C++17）
- **内存映射**：`mmap`（Linux）、`MapViewOfFile`（Windows）

---

## 1. C++ 流（iostream）

### 1.1 文件流基础？

```cpp
#include <fstream>
#include <iostream>

// 读取文件
void read_file() {
    std::ifstream file("input.txt");

    if (!file.is_open()) {
        std::cerr << "打开文件失败\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }

    // 文件自动关闭（RAII）
}

// 写入文件
void write_file() {
    std::ofstream file("output.txt");

    if (!file.is_open()) {
        std::cerr << "打开文件失败\n";
        return;
    }

    file << "Hello, World!\n";
    file << "Second line\n";

    // 文件自动关闭
}
```

### 1.2 打开模式？

| 模式 | 说明 |
|------|------|
| `std::ios::in` | 只读 |
| `std::ios::out` | 只写（清空原内容） |
| `std::ios::app` | 追加写入 |
| `std::ios::ate` | 打开时定位到文件末尾 |
| `std::ios::trunc` | 清空文件 |
| `std::ios::binary` | 二进制模式 |

**组合使用**：
```cpp
// 读写模式
std::fstream file("data.bin", std::ios::in | std::ios::out | std::ios::binary);

// 追加模式
std::ofstream file("log.txt", std::ios::app);

// 二进制写入（覆盖）
std::ofstream file("output.bin", std::ios::binary | std::ios::trunc);
```

### 1.3 文件位置操作？

```cpp
std::fstream file("data.txt", std::ios::in | std::ios::out);

// 获取读位置
std::streampos read_pos = file.tellg();

// 设置读位置
file.seekg(0, std::ios::beg);  // 文件开头
file.seekg(10, std::ios::cur); // 当前位置向后 10 字节
file.seekg(-5, std::ios::end); // 文件末尾前 5 字节

// 获取写位置
std::streampos write_pos = file.tellp();

// 设置写位置
file.seekp(0, std::ios::end);  // 文件末尾
```

### 1.4 二进制读写？

```cpp
struct Data {
    int id;
    double value;
    char name[32];
};

// 写入二进制数据
void write_binary() {
    std::ofstream file("data.bin", std::ios::binary);

    Data d{42, 3.14, "test"};
    file.write(reinterpret_cast<const char*>(&d), sizeof(Data));
}

// 读取二进制数据
void read_binary() {
    std::ifstream file("data.bin", std::ios::binary);

    Data d;
    file.read(reinterpret_cast<char*>(&d), sizeof(Data));

    if (file.gcount() == sizeof(Data)) {
        std::cout << "ID: " << d.id << "\n";
        std::cout << "Value: " << d.value << "\n";
    }
}
```

**注意事项**：
- 不能直接序列化包含指针的结构体
- 不能直接序列化 `std::string`、`std::vector` 等动态类型
- 跨平台可能有字节序和对齐问题

---

## 2. C 文件 API

### 2.1 基本操作？

```cpp
#include <cstdio>

// 打开文件
FILE* file = fopen("data.txt", "r");
if (!file) {
    perror("打开文件失败");
    return;
}

// 读取数据
char buffer[256];
while (fgets(buffer, sizeof(buffer), file)) {
    printf("%s", buffer);
}

// 关闭文件
fclose(file);
```

**打开模式**：

| 模式 | 说明 |
|------|------|
| `"r"` | 只读 |
| `"w"` | 只写（清空） |
| `"a"` | 追加 |
| `"r+"` | 读写 |
| `"w+"` | 读写（清空） |
| `"a+"` | 读写（追加） |
| `"rb"` | 二进制只读 |
| `"wb"` | 二进制只写 |

### 2.2 二进制读写？

```cpp
// 写入二进制
FILE* file = fopen("data.bin", "wb");
int data[] = {1, 2, 3, 4, 5};
size_t written = fwrite(data, sizeof(int), 5, file);
fclose(file);

// 读取二进制
FILE* file = fopen("data.bin", "rb");
int buffer[5];
size_t read = fread(buffer, sizeof(int), 5, file);
fclose(file);
```

### 2.3 文件位置操作？

```cpp
FILE* file = fopen("data.txt", "r+");

// 获取当前位置
long pos = ftell(file);

// 移动文件指针
fseek(file, 0, SEEK_SET);  // 文件开头
fseek(file, 10, SEEK_CUR); // 当前位置后 10 字节
fseek(file, -5, SEEK_END); // 文件末尾前 5 字节

// 重置到开头
rewind(file);

fclose(file);
```

### 2.4 大文件支持？

```cpp
// 64 位文件偏移（Linux）
#define _FILE_OFFSET_BITS 64

FILE* file = fopen("large_file.bin", "rb");

// fseeko/ftello 支持 off_t（64 位）
off_t pos = ftello(file);
fseeko(file, 1LL << 35, SEEK_SET);  // 跳到 32 GB 位置

fclose(file);
```

---

## 3. std::filesystem（C++17）

### 3.1 基本操作？

```cpp
#include <filesystem>
namespace fs = std::filesystem;

// 检查文件是否存在
if (fs::exists("file.txt")) {
    std::cout << "文件存在\n";
}

// 检查是否为目录
if (fs::is_directory("my_folder")) {
    std::cout << "是目录\n";
}

// 获取文件大小
uintmax_t size = fs::file_size("file.txt");

// 创建目录
fs::create_directory("new_folder");
fs::create_directories("path/to/nested/folder");

// 删除文件
fs::remove("file.txt");

// 递归删除目录
fs::remove_all("my_folder");

// 重命名/移动
fs::rename("old.txt", "new.txt");

// 复制文件
fs::copy("source.txt", "dest.txt");
```

### 3.2 路径操作？

```cpp
fs::path p = "folder/file.txt";

// 路径组件
std::cout << p.filename() << "\n";   // "file.txt"
std::cout << p.stem() << "\n";       // "file"
std::cout << p.extension() << "\n";  // ".txt"
std::cout << p.parent_path() << "\n"; // "folder"
std::cout << p.root_path() << "\n";  // ""（相对路径）

// 拼接路径
fs::path full = fs::path("base") / "sub" / "file.txt";
// "base/sub/file.txt"（自动处理分隔符）

// 绝对路径
fs::path abs = fs::absolute(p);

// 规范化路径
fs::path canonical = fs::canonical(p);  // 解析符号链接、. 和 ..
```

### 3.3 遍历目录？

```cpp
// 遍历目录（非递归）
for (const auto& entry : fs::directory_iterator("my_folder")) {
    std::cout << entry.path() << "\n";

    if (entry.is_regular_file()) {
        std::cout << "  文件，大小: " << entry.file_size() << "\n";
    } else if (entry.is_directory()) {
        std::cout << "  目录\n";
    }
}

// 递归遍历
for (const auto& entry : fs::recursive_directory_iterator("my_folder")) {
    std::cout << entry.path() << "\n";
}
```

### 3.4 文件属性？

```cpp
fs::path p = "file.txt";

// 文件状态
fs::file_status status = fs::status(p);

if (fs::is_regular_file(status)) {
    std::cout << "普通文件\n";
}

if (fs::is_symlink(p)) {
    std::cout << "符号链接\n";
}

// 文件权限
fs::perms perms = status.permissions();
if ((perms & fs::perms::owner_write) != fs::perms::none) {
    std::cout << "所有者可写\n";
}

// 修改时间
auto ftime = fs::last_write_time(p);
// C++20 可以转换为 system_clock::time_point
```

---

## 4. 内存映射文件

### 4.1 Linux（mmap）？

```cpp
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void mmap_example() {
    // 打开文件
    int fd = open("large_file.bin", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    // 获取文件大小
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return;
    }

    // 映射文件到内存
    void* addr = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    // 使用映射的内存
    const char* data = static_cast<const char*>(addr);
    // ... 处理数据 ...

    // 解除映射
    munmap(addr, sb.st_size);
    close(fd);
}
```

### 4.2 Windows（MapViewOfFile）？

```cpp
#include <windows.h>

void mapview_example() {
    // 打开文件
    HANDLE hFile = CreateFileW(
        L"large_file.bin",
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }

    // 创建文件映射对象
    HANDLE hMap = CreateFileMappingW(
        hFile,
        nullptr,
        PAGE_READONLY,
        0,
        0,
        nullptr
    );

    if (!hMap) {
        CloseHandle(hFile);
        return;
    }

    // 映射到内存
    void* addr = MapViewOfFile(
        hMap,
        FILE_MAP_READ,
        0,
        0,
        0
    );

    if (!addr) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        return;
    }

    // 使用映射的内存
    const char* data = static_cast<const char*>(addr);
    // ... 处理数据 ...

    // 清理
    UnmapViewOfFile(addr);
    CloseHandle(hMap);
    CloseHandle(hFile);
}
```

---

## 5. 高级技巧

### 5.1 一次性读取整个文件？

```cpp
// 方法 1：C++ 流
std::string read_file(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 方法 2：预分配 + read
std::string read_file_fast(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, '\0');
    file.read(buffer.data(), size);
    return buffer;
}

// 方法 3：C API + fstat
std::string read_file_c(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return "";

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string buffer(size, '\0');
    fread(buffer.data(), 1, size, file);
    fclose(file);

    return buffer;
}
```

### 5.2 逐行读取大文件？

```cpp
void process_large_file(const std::string& path) {
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        // 处理每一行（不需要将整个文件加载到内存）
        process_line(line);
    }
}
```

### 5.3 缓冲区优化？

```cpp
// 设置自定义缓冲区
char buffer[1024 * 64];  // 64 KB 缓冲区
std::ifstream file("large_file.txt");
file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

// 禁用同步（提高性能）
std::ios::sync_with_stdio(false);
std::cin.tie(nullptr);
```

### 5.4 临时文件？

```cpp
#include <cstdio>

// C API：临时文件
FILE* temp = tmpfile();  // 创建临时文件，自动删除
// 使用 temp...
fclose(temp);

// C++17：临时目录
fs::path temp_dir = fs::temp_directory_path();
fs::path temp_file = temp_dir / "my_temp_file.txt";
```

---

## 6. 性能对比

### 6.1 读取性能？

| 方法 | 速度 | 内存占用 | 适用场景 |
|------|------|----------|----------|
| `fread` | 最快 | 低 | 大文件、二进制 |
| 内存映射 | 快 | 低（虚拟内存） | 超大文件、随机访问 |
| `std::ifstream` | 中等 | 低 | 文本文件、逐行处理 |
| `file.rdbuf()` | 慢 | 高 | 小文件 |

### 6.2 写入性能？

```cpp
// ❌ 慢：频繁 flush
for (int i = 0; i < 100000; i++) {
    file << i << "\n" << std::flush;  // 每次都刷新
}

// ✅ 快：批量写入
for (int i = 0; i < 100000; i++) {
    file << i << "\n";  // 缓冲区满时自动刷新
}
file.flush();  // 最后统一刷新
```

---

## 7. 错误处理

### 7.1 C++ 流错误？

```cpp
std::ifstream file("data.txt");

if (!file.is_open()) {
    std::cerr << "打开文件失败\n";
    return;
}

file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

try {
    std::string line;
    while (std::getline(file, line)) {
        // 处理行
    }
} catch (const std::ios_base::failure& e) {
    std::cerr << "读取错误: " << e.what() << "\n";
}
```

### 7.2 C API 错误？

```cpp
FILE* file = fopen("data.txt", "r");
if (!file) {
    perror("fopen");  // 打印系统错误消息
    std::cerr << "错误码: " << errno << "\n";
    return;
}

// 清除错误标志
clearerr(file);

// 检查错误
if (ferror(file)) {
    std::cerr << "IO 错误\n";
}

// 检查 EOF
if (feof(file)) {
    std::cout << "到达文件末尾\n";
}
```

---

## 总结

### IO 方法选择指南

| 场景 | 推荐方法 |
|------|----------|
| 文本文件逐行读取 | `std::ifstream` + `getline` |
| 二进制文件读写 | `fread`/`fwrite` 或 `std::ifstream::read/write` |
| 超大文件随机访问 | 内存映射（`mmap` / `MapViewOfFile`） |
| 文件系统操作 | `std::filesystem`（C++17） |
| 性能关键场景 | `fread`/`fwrite` + 大缓冲区 |

### 关键记忆点

1. **C++ 流**：面向对象，RAII，类型安全
2. **C API**：更快，更底层，需要手动管理资源
3. **`std::filesystem`**：C++17 的现代文件系统 API
4. **内存映射**：超大文件的高效访问方式
5. **二进制模式**：跨平台时注意字节序和对齐
6. **错误处理**：检查 `is_open()`、`fail()`、`errno`
7. **性能优化**：大缓冲区、减少 flush、批量操作

---

## 参考资源

- [std::filesystem Reference](https://en.cppreference.com/w/cpp/filesystem)
- [C File I/O](https://en.cppreference.com/w/c/io)
- [mmap Manual](https://man7.org/linux/man-pages/man2/mmap.2.html)
