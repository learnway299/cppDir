# 22 - 字符串与编码 (String and Encoding)

## 简介

字符编码是软件国际化的基础。理解 ASCII、UTF-8、UTF-16、UTF-32 以及 C++ 中的字符串处理对于开发跨平台应用至关重要。

**核心概念**：
- **字符编码**：ASCII、Latin-1、Unicode
- **UTF 编码**：UTF-8、UTF-16、UTF-32
- **C++ 字符串类型**：`char`、`wchar_t`、`char16_t`、`char32_t`、`char8_t`
- **字符串操作**：转换、验证、处理

---

## 1. 字符编码基础

### 1.1 ASCII 与 Latin-1？

**ASCII（7 位）**：
- 范围：0-127
- 涵盖：英文字母、数字、标点符号、控制字符

```cpp
char ascii = 'A';  // 65
char newline = '\n';  // 10
```

**Latin-1 / ISO-8859-1（8 位）**：
- 范围：0-255
- 扩展 ASCII，增加西欧字符

**问题**：
- 无法表示中文、日文等非拉丁字符
- 不同的代码页（Code Page）导致乱码

### 1.2 Unicode？

**Unicode 标准**：
- 统一的字符集（Character Set）
- 每个字符分配唯一的码点（Code Point）
- 表示方法：`U+XXXX`（十六进制）

**示例**：
- `U+0041`：'A'
- `U+4E2D`：'中'
- `U+1F600`：😀（Emoji）

**术语**：
- **码点（Code Point）**：Unicode 中字符的编号
- **编码单元（Code Unit）**：存储时的最小单位
- **字符（Character）**：用户感知的一个符号（可能由多个码点组成）

---

## 2. UTF 编码

### 2.1 UTF-8？

**特点**：
- 变长编码：1-4 字节
- ASCII 兼容（0-127 只用 1 字节）
- 网络传输首选

**编码规则**：

| 码点范围 | 字节数 | 编码格式 |
|----------|--------|----------|
| U+0000 - U+007F | 1 | `0xxxxxxx` |
| U+0080 - U+07FF | 2 | `110xxxxx 10xxxxxx` |
| U+0800 - U+FFFF | 3 | `1110xxxx 10xxxxxx 10xxxxxx` |
| U+10000 - U+10FFFF | 4 | `11110xxx 10xxxxxx 10xxxxxx 10xxxxxx` |

**示例**：
```cpp
// 'A' (U+0041) → 1 字节
// 0x41

// '中' (U+4E2D) → 3 字节
// 0xE4 0xB8 0xAD
//   1110 0100   10 111000   10 101101
//   ^^^^        ^^          ^^
//   标记        标记        标记

// 😀 (U+1F600) → 4 字节
// 0xF0 0x9F 0x98 0x80
```

**C++ 中使用**：
```cpp
// C++11: u8"" 字符串字面量
const char* utf8 = u8"Hello 世界 😀";

// C++20: char8_t 类型
const char8_t* utf8_new = u8"Hello 世界";
```

### 2.2 UTF-16？

**特点**：
- 变长编码：2 或 4 字节
- BMP（基本多文种平面）用 2 字节
- 其他平面用代理对（Surrogate Pair）

**编码规则**：
- U+0000 - U+FFFF（BMP）：直接用 2 字节
- U+10000 - U+10FFFF：用代理对（4 字节）

**代理对计算**：
```cpp
// 码点 U+1F600（😀）
// 1. 减去 0x10000: 0x1F600 - 0x10000 = 0xF600
// 2. 分成高 10 位和低 10 位
//    高 10 位: 0xF600 >> 10 = 0x3D = 0b0000111101
//    低 10 位: 0xF600 & 0x3FF = 0x200 = 0b1000000000
// 3. 高代理: 0xD800 + 0x3D = 0xD83D
//    低代理: 0xDC00 + 0x200 = 0xDE00
// 结果: 0xD83D 0xDE00
```

**C++ 中使用**：
```cpp
// C++11: u"" 字符串字面量
const char16_t* utf16 = u"Hello 世界 😀";

// Windows WCHAR（通常是 UTF-16）
const wchar_t* wide = L"Hello 世界";
```

### 2.3 UTF-32？

**特点**：
- 定长编码：每个字符 4 字节
- 直接使用码点值
- 内存占用大

**C++ 中使用**：
```cpp
// C++11: U"" 字符串字面量
const char32_t* utf32 = U"Hello 世界 😀";

// 码点直接对应
char32_t c = U'中';  // 0x4E2D
```

### 2.4 编码对比？

| 编码 | 单位大小 | 变长/定长 | ASCII 兼容 | 用途 |
|------|----------|-----------|-----------|------|
| UTF-8 | 1 字节 | 变长（1-4） | ✅ | 文件、网络、Linux |
| UTF-16 | 2 字节 | 变长（2/4） | ❌ | Windows、Java、JavaScript |
| UTF-32 | 4 字节 | 定长 | ❌ | 内部处理 |

---

## 3. C++ 字符串类型

### 3.1 字符类型？

```cpp
// C++98
char c1 = 'A';           // 8 位，编码不确定
wchar_t wc = L'中';      // 16/32 位（平台相关）

// C++11
char16_t c16 = u'中';    // 16 位，UTF-16
char32_t c32 = U'中';    // 32 位，UTF-32

// C++20
char8_t c8 = u8'A';      // 8 位，UTF-8
```

### 3.2 字符串类型？

```cpp
// 窄字符串（编码不确定，通常是系统默认编码）
std::string str = "Hello";

// 宽字符串（Windows 上通常是 UTF-16）
std::wstring wstr = L"Hello 世界";

// C++11: UTF-16 和 UTF-32
std::u16string u16str = u"Hello 世界 😀";
std::u32string u32str = U"Hello 世界 😀";

// C++20: UTF-8
std::u8string u8str = u8"Hello 世界 😀";
```

### 3.3 std::string 的问题？

```cpp
std::string utf8 = u8"中文";  // C++17

// ❌ size() 返回字节数，不是字符数
std::cout << utf8.size();  // 6（"中文" 各 3 字节）

// ❌ operator[] 访问字节，不是字符
char byte = utf8[0];  // 0xE4（"中" 的第一个字节）

// ❌ 截断可能破坏多字节字符
utf8.resize(4);  // 可能在"中"字符中间截断
```

---

## 4. 编码转换

### 4.1 标准库转换（C++11）？

```cpp
#include <codecvt>
#include <locale>

// UTF-8 → UTF-16
std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
std::u16string utf16 = converter.from_bytes(utf8_str);

// UTF-16 → UTF-8
std::string utf8 = converter.to_bytes(utf16_str);

// ⚠️ C++17 标记为 deprecated
```

### 4.2 使用第三方库？

**ICU（International Components for Unicode）**：
```cpp
#include <unicode/ucnv.h>

UErrorCode status = U_ZERO_ERROR;
UConverter* conv = ucnv_open("UTF-8", &status);

// 转换...

ucnv_close(conv);
```

**iconv（Linux/Unix）**：
```cpp
#include <iconv.h>

iconv_t cd = iconv_open("UTF-8", "GBK");

// 转换...

iconv_close(cd);
```

### 4.3 手动转换（UTF-8 ↔ UTF-32）？

**UTF-8 解码**：
```cpp
char32_t decode_utf8(const char*& p) {
    unsigned char c0 = *p++;

    if (c0 < 0x80) {  // 1 字节
        return c0;
    } else if ((c0 & 0xE0) == 0xC0) {  // 2 字节
        char32_t c1 = *p++ & 0x3F;
        return ((c0 & 0x1F) << 6) | c1;
    } else if ((c0 & 0xF0) == 0xE0) {  // 3 字节
        char32_t c1 = *p++ & 0x3F;
        char32_t c2 = *p++ & 0x3F;
        return ((c0 & 0x0F) << 12) | (c1 << 6) | c2;
    } else if ((c0 & 0xF8) == 0xF0) {  // 4 字节
        char32_t c1 = *p++ & 0x3F;
        char32_t c2 = *p++ & 0x3F;
        char32_t c3 = *p++ & 0x3F;
        return ((c0 & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
    }

    return 0xFFFD;  // 替换字符
}
```

**UTF-8 编码**：
```cpp
std::string encode_utf8(char32_t codepoint) {
    std::string result;

    if (codepoint < 0x80) {  // 1 字节
        result += static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {  // 2 字节
        result += static_cast<char>(0xC0 | (codepoint >> 6));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {  // 3 字节
        result += static_cast<char>(0xE0 | (codepoint >> 12));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x110000) {  // 4 字节
        result += static_cast<char>(0xF0 | (codepoint >> 18));
        result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    }

    return result;
}
```

---

## 5. 字符串操作

### 5.1 UTF-8 字符串长度？

```cpp
// 计算 UTF-8 字符串的字符数（非字节数）
size_t utf8_length(const char* str) {
    size_t count = 0;
    while (*str) {
        unsigned char c = *str;
        if ((c & 0x80) == 0) {          // 1 字节
            str += 1;
        } else if ((c & 0xE0) == 0xC0) { // 2 字节
            str += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3 字节
            str += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4 字节
            str += 4;
        } else {
            str += 1;  // 非法序列，跳过
        }
        count++;
    }
    return count;
}
```

### 5.2 UTF-8 验证？

```cpp
bool is_valid_utf8(const char* str, size_t len) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str);
    size_t i = 0;

    while (i < len) {
        unsigned char c = bytes[i];

        if (c < 0x80) {  // 1 字节
            i++;
        } else if ((c & 0xE0) == 0xC0) {  // 2 字节
            if (i + 1 >= len || (bytes[i + 1] & 0xC0) != 0x80) {
                return false;
            }
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {  // 3 字节
            if (i + 2 >= len ||
                (bytes[i + 1] & 0xC0) != 0x80 ||
                (bytes[i + 2] & 0xC0) != 0x80) {
                return false;
            }
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {  // 4 字节
            if (i + 3 >= len ||
                (bytes[i + 1] & 0xC0) != 0x80 ||
                (bytes[i + 2] & 0xC0) != 0x80 ||
                (bytes[i + 3] & 0xC0) != 0x80) {
                return false;
            }
            i += 4;
        } else {
            return false;  // 非法起始字节
        }
    }

    return true;
}
```

### 5.3 UTF-8 子串？

```cpp
// 安全地截取 UTF-8 字符串（不破坏多字节字符）
std::string utf8_substr(const std::string& str, size_t start, size_t count) {
    const char* p = str.c_str();
    size_t char_count = 0;
    size_t start_pos = 0;
    size_t end_pos = 0;

    // 找到起始位置
    while (*p && char_count < start) {
        unsigned char c = *p;
        if ((c & 0x80) == 0) p += 1;
        else if ((c & 0xE0) == 0xC0) p += 2;
        else if ((c & 0xF0) == 0xE0) p += 3;
        else if ((c & 0xF8) == 0xF0) p += 4;
        char_count++;
    }
    start_pos = p - str.c_str();

    // 找到结束位置
    char_count = 0;
    while (*p && char_count < count) {
        unsigned char c = *p;
        if ((c & 0x80) == 0) p += 1;
        else if ((c & 0xE0) == 0xC0) p += 2;
        else if ((c & 0xF0) == 0xE0) p += 3;
        else if ((c & 0xF8) == 0xF0) p += 4;
        char_count++;
    }
    end_pos = p - str.c_str();

    return str.substr(start_pos, end_pos - start_pos);
}
```

---

## 6. 平台差异

### 6.1 Windows vs Linux？

| 特性 | Windows | Linux |
|------|---------|-------|
| 默认窄字符编码 | GBK/CP936（中文） | UTF-8 |
| `wchar_t` 大小 | 2 字节（UTF-16） | 4 字节（UTF-32） |
| API 字符串 | UTF-16（Wide API） | UTF-8 |

**Windows 示例**：
```cpp
// Windows 文件路径使用 Wide API
HANDLE file = CreateFileW(
    L"C:\\中文路径\\文件.txt",
    GENERIC_READ,
    0,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
);

// 需要转换 UTF-8 → UTF-16
std::wstring utf8_to_wide(const std::string& utf8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], len);
    return result;
}
```

### 6.2 文件读写？

```cpp
// 写入 UTF-8 文件（带 BOM）
std::ofstream file("output.txt", std::ios::binary);
file.write("\xEF\xBB\xBF", 3);  // UTF-8 BOM
file << u8"中文内容";

// 读取时跳过 BOM
std::ifstream file("input.txt", std::ios::binary);
char bom[3];
file.read(bom, 3);
if (bom[0] != '\xEF' || bom[1] != '\xBB' || bom[2] != '\xBF') {
    file.seekg(0);  // 没有 BOM，回到开头
}
```

---

## 7. 最佳实践

### 7.1 字符串处理原则？

1. **内部使用 UTF-8**：
   - 存储和传输都用 UTF-8
   - 仅在必要时转换为 UTF-16/32

2. **Windows 文件 API**：
   - 使用 Wide API（`CreateFileW`）
   - UTF-8 ↔ UTF-16 转换

3. **避免 `std::wstring`**：
   - 跨平台不一致（16/32 位）
   - 优先使用 `std::string` (UTF-8)

4. **使用第三方库**：
   - ICU、utf8cpp、Boost.Locale

### 7.2 常见陷阱？

```cpp
// ❌ 错误：假设 size() 是字符数
std::string utf8 = u8"中文";
if (utf8.size() > 2) {  // ❌ 实际是 6 字节
    // ...
}

// ❌ 错误：截断可能破坏字符
utf8.resize(4);  // ❌ 可能在字符中间截断

// ❌ 错误：反转字符串
std::reverse(utf8.begin(), utf8.end());  // ❌ 破坏 UTF-8 序列

// ✅ 正确：使用 UTF-8 长度函数
if (utf8_length(utf8.c_str()) > 2) {
    // ...
}
```

---

## 总结

### 编码对比速查

| 特性 | UTF-8 | UTF-16 | UTF-32 |
|------|-------|--------|--------|
| 单位大小 | 1 字节 | 2 字节 | 4 字节 |
| 变长/定长 | 变长（1-4） | 变长（2/4） | 定长 |
| ASCII 兼容 | ✅ | ❌ | ❌ |
| 空间效率 | 高（英文） | 中等 | 低 |
| 随机访问 | 慢 | 慢 | 快 |
| 推荐用途 | 存储、网络 | Windows API | 内部处理 |

### 关键记忆点

1. **UTF-8 是首选编码**：兼容 ASCII，节省空间
2. **`std::string::size()` 返回字节数，不是字符数**
3. **Windows 使用 UTF-16，Linux 使用 UTF-8**
4. **`wchar_t` 大小平台相关**（Windows 2 字节，Linux 4 字节）
5. **UTF-8 验证**：检查字节序列合法性
6. **C++20** 引入 `char8_t` 和 `std::u8string`
7. **避免手动处理**：使用 ICU、utf8cpp 等库

---

## 参考资源

- [UTF-8 Everywhere Manifesto](http://utf8everywhere.org/)
- [Unicode Standard](https://www.unicode.org/versions/latest/)
- [ICU - International Components for Unicode](https://icu.unicode.org/)
- [utf8cpp Library](https://github.com/nemtrif/utfcpp)
