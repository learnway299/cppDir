# 资源管理与换肤

## 概述

Duilib 的资源系统管理图片、字体、颜色等 UI 素材的加载和缓存。换肤功能基于资源路径切换——切换资源目录后重新加载 XML 和图片即可实现全局换肤。理解资源加载机制和缓存策略，对性能优化和内存管理都很重要。

---

## 1. 资源路径体系

### 资源路径类型

```cpp
// CPaintManagerUI 支持多种资源加载方式
enum {
    UILIB_FILE = 1,       // 从文件系统加载
    UILIB_ZIP = 2,        // 从 ZIP 包加载
    UILIB_RESOURCE = 3,   // 从 EXE 资源加载
    UILIB_ZIPRESOURCE = 4 // 从 EXE 嵌入的 ZIP 资源加载
};
```

### 设置资源路径

```cpp
// 方式1：文件系统目录
CPaintManagerUI::SetInstance(hInstance);
CPaintManagerUI::SetResourcePath(_T("D:\\App\\skin\\default\\"));

// 方式2：ZIP 包
CPaintManagerUI::SetResourceZip(_T("skin.zip"), true);  // true=缓存到内存

// 方式3：EXE 嵌入资源（适合发布单文件）
CPaintManagerUI::SetResourceType(UILIB_RESOURCE);

// 方式4：EXE 中的 ZIP 资源
CPaintManagerUI::SetResourceZip(_T("IDR_SKIN"), true, _T("ZIPRES"));
```

### 资源查找顺序

```
图片路径解析规则：
1. 如果是绝对路径（如 "C:\img\bg.png"）→ 直接加载
2. 否则拼接资源根目录 → ResourcePath + 相对路径
3. ZIP 模式下在 ZIP 包内查找对应路径
4. 资源模式下在 EXE 资源中查找

示例：
ResourcePath = "D:\App\skin\default\"
XML 中 bkimage="images\bg.png"
实际加载 → "D:\App\skin\default\images\bg.png"
```

---

## 2. 图片加载与缓存

### 图片缓存机制

```cpp
// CPaintManagerUI 维护图片缓存（全局共享 + 实例独立）
class CPaintManagerUI {
    // 共享缓存（所有窗口共用）
    static CStdStringPtrMap m_SharedImageHash;
    // 实例缓存
    CStdStringPtrMap m_ImageHash;
};

// 图片数据结构
struct TImageInfo {
    HBITMAP hBitmap;      // GDI 位图句柄
    LPBYTE  pBits;        // 像素数据指针（DIB）
    int     nX;           // 图片宽度
    int     nY;           // 图片高度
    bool    bAlpha;       // 是否有 Alpha 通道
    bool    bUseHSL;      // 是否使用 HSL 调色
    CDuiString sResType;  // 资源类型
};
```

### 图片加载流程

```cpp
TImageInfo* CRenderEngine::LoadImage(LPCTSTR pstrFilename,
                                      CPaintManagerUI* pManager)
{
    // 1. 先查缓存
    TImageInfo* data = (TImageInfo*)pManager->GetImageEx(pstrFilename);
    if (data) return data;

    // 2. 加载图片文件
    LPBYTE pData = NULL;
    DWORD dwSize = 0;

    switch (pManager->GetResourceType()) {
    case UILIB_FILE:
        pData = LoadFileData(pstrFilename, &dwSize);
        break;
    case UILIB_ZIP:
        pData = LoadZipData(pstrFilename, &dwSize);
        break;
    case UILIB_RESOURCE:
        pData = LoadResourceData(pstrFilename, &dwSize);
        break;
    }

    if (!pData) return NULL;

    // 3. 解码图片（支持 PNG/JPG/BMP/GIF）
    HBITMAP hBitmap = DecodeToBitmap(pData, dwSize, &bAlpha);
    free(pData);

    // 4. 存入缓存
    TImageInfo* pImageInfo = new TImageInfo;
    pImageInfo->hBitmap = hBitmap;
    pImageInfo->bAlpha = bAlpha;
    // ...
    pManager->AddImage(pstrFilename, pImageInfo);

    return pImageInfo;
}
```

### 图片属性字符串

```xml
<!-- 完整格式 -->
<Button normalimage="file='btn.png' source='0,0,80,30' corner='5,5,5,5'
                      dest='0,0,80,30' fade='255' hole='false'" />

<!-- 简写（只指定文件名） -->
<Button normalimage="btn.png" />
```

| 参数 | 说明 | 示例 |
|------|------|------|
| file | 图片文件路径 | `file='images/btn.png'` |
| source | 图片源矩形（裁剪） | `source='0,0,80,30'` |
| corner | 九宫格边距 | `corner='5,5,5,5'` |
| dest | 目标绘制矩形（相对控件） | `dest='10,5,70,25'` |
| fade | 透明度 0-255 | `fade='200'` |
| hole | 是否中心镂空 | `hole='true'` |
| xtiled | 水平平铺 | `xtiled='true'` |
| ytiled | 垂直平铺 | `ytiled='true'` |

---

## 3. 字体管理

### 添加自定义字体

```cpp
// 在 CPaintManagerUI 中注册字体
// id: 字体索引，后续通过 id 引用
// pStrFontName: 字体名称
// nSize: 字号
// bBold/bUnderline/bItalic: 样式
int CPaintManagerUI::AddFont(int id, LPCTSTR pStrFontName, int nSize,
                              bool bBold, bool bUnderline, bool bItalic)
{
    TFontInfo* pFontInfo = new TFontInfo;
    // 创建 HFONT
    LOGFONT lf = { 0 };
    _tcscpy(lf.lfFaceName, pStrFontName);
    lf.lfHeight = -nSize;
    lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
    lf.lfUnderline = bUnderline;
    lf.lfItalic = bItalic;
    pFontInfo->hFont = ::CreateFontIndirect(&lf);

    m_aCustomFonts.Add(pFontInfo);
    return id;
}
```

### XML 中定义和使用字体

```xml
<Window>
    <!-- 定义字体 -->
    <Font id="0" name="Microsoft YaHei" size="12" />
    <Font id="1" name="Microsoft YaHei" size="14" bold="true" />
    <Font id="2" name="Microsoft YaHei" size="20" />

    <VerticalLayout>
        <Label text="普通文本" font="0" />
        <Label text="加粗标题" font="1" />
        <Label text="大号标题" font="2" />
    </VerticalLayout>
</Window>
```

---

## 4. 颜色管理

### 全局颜色定义

```xml
<!-- 通过 Default 集中定义常用颜色 -->
<Default name="Label" value="textcolor=&quot;#FF333333&quot;" />
<Default name="Button" value="textcolor=&quot;#FFFFFFFF&quot; hotbkcolor=&quot;#FF1890FF&quot;" />
```

### 颜色格式

```cpp
// Duilib 颜色格式：#AARRGGBB（注意不是 AABBGGRR）
// A=Alpha, R=Red, G=Green, B=Blue

// 解析颜色字符串
DWORD ParseColor(LPCTSTR pstrValue)
{
    // "#FFFF0000" → 0xFFFF0000（不透明红色）
    // "#80000000" → 0x80000000（半透明黑色）
    while (*pstrValue == _T('#')) pstrValue++;
    DWORD dwColor = _tcstoul(pstrValue, NULL, 16);
    return dwColor;
}
```

---

## 5. 换肤实现

### 方案1：切换资源目录

```cpp
// 最简单的换肤方式：切换资源路径后重建 UI
void SwitchSkin(LPCTSTR pstrSkinPath)
{
    // 1. 清除所有图片缓存
    CPaintManagerUI::RemoveAllImages();

    // 2. 切换资源目录
    CPaintManagerUI::SetResourcePath(pstrSkinPath);
    // 如 "D:\App\skin\dark\"  或 "D:\App\skin\light\"

    // 3. 重新加载 XML（控件树重建）
    m_pm.RevertStatic();
    CDialogBuilder builder;
    CControlUI* pRoot = builder.Create(_T("main.xml"), 0, this, &m_pm);
    m_pm.AttachDialog(pRoot);
    m_pm.AddNotifier(this);
    m_pm.NeedUpdate();
}
```

### 方案2：不重建控件树的换肤

```cpp
// 更高效：只替换图片引用，不重建控件树
void SwitchSkinLight()
{
    // 清除图片缓存
    m_pm.RemoveAllImages();

    // 切换资源路径
    CPaintManagerUI::SetResourcePath(_T("skin\\light\\"));

    // 刷新所有控件图片（不重建控件树）
    // 因为控件保存的是图片路径字符串，重绘时会重新加载
    ::InvalidateRect(m_hWnd, NULL, FALSE);
}
// 前提：两套皮肤使用相同的文件名，只是目录不同
```

### 方案3：HSL 动态调色

```cpp
// Duilib 支持通过 HSL 色相/饱和度/亮度调整来动态换肤
// 不需要多套图片资源

CPaintManagerUI::SetHSL(true, 180, 100, 100);
// 调整色相为 180（青色系），饱和度和亮度保持

// 所有标记 bUseHSL=true 的图片会被动态调色
// <Button normalimage="file='btn.png' hsl='true'" />
```

### 皮肤目录结构

```
skin/
├── default/            # 默认皮肤
│   ├── main.xml
│   ├── images/
│   │   ├── btn_normal.png
│   │   ├── btn_hover.png
│   │   └── bg.png
│   └── fonts/
├── dark/               # 暗色皮肤（同样的文件名，不同的图片内容）
│   ├── main.xml        # 颜色值可能不同
│   ├── images/
│   │   ├── btn_normal.png
│   │   ├── btn_hover.png
│   │   └── bg.png
│   └── fonts/
└── blue/               # 蓝色皮肤
    ├── main.xml
    └── images/
```

---

## 6. ZIP 资源打包

### 打包与加载

```cpp
// 将皮肤目录打成 ZIP，减少文件数量和磁盘 I/O
// 发布时只需要 app.exe + skin.zip

// 加载 ZIP 资源
CPaintManagerUI::SetResourceZip(_T("skin.zip"), true);
// true = 将整个 ZIP 加载到内存中，后续读取无磁盘 I/O

// 也可以嵌入到 EXE 资源中（单文件发布）
// 1. 将 skin.zip 作为自定义资源加入 RC 文件
//    IDR_SKIN ZIPRES "skin.zip"
// 2. 代码中加载
CPaintManagerUI::SetResourceZip(_T("IDR_SKIN"), true, _T("ZIPRES"));
```

### ZIP 内部文件读取

```cpp
// 从 ZIP 中读取文件数据
LPBYTE LoadZipData(LPCTSTR pstrFilename, DWORD* pdwSize)
{
    // 使用 minizip / zlib 解压指定文件
    unzFile zf = unzOpen(m_pstrZipPath);
    if (unzLocateFile(zf, pstrFilename, 0) != UNZ_OK)
        return NULL;

    unz_file_info fi;
    unzGetCurrentFileInfo(zf, &fi, NULL, 0, NULL, 0, NULL, 0);

    LPBYTE pData = (LPBYTE)malloc(fi.uncompressed_size);
    unzOpenCurrentFile(zf);
    unzReadCurrentFile(zf, pData, fi.uncompressed_size);
    unzCloseCurrentFile(zf);
    unzClose(zf);

    *pdwSize = fi.uncompressed_size;
    return pData;
}
```

---

## 7. 图片格式支持

### 内置解码器

| 格式 | 支持情况 | 说明 |
|------|---------|------|
| PNG | 完整支持（含 Alpha） | 最常用，推荐用于 UI 图标 |
| JPG | 支持 | 适合大图/照片，无 Alpha |
| BMP | 支持 | 未压缩，文件体积大 |
| GIF | 静态支持 | 动画 GIF 需要自行实现帧播放 |

### PNG Alpha 预乘处理

```cpp
// Windows AlphaBlend 要求 Premultiplied Alpha
// 加载 PNG 后需要预乘 Alpha
void PremultiplyAlpha(LPBYTE pBits, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        BYTE a = pBits[3];  // Alpha
        pBits[0] = pBits[0] * a / 255;  // B
        pBits[1] = pBits[1] * a / 255;  // G
        pBits[2] = pBits[2] * a / 255;  // R
        pBits += 4;
    }
}
```

---

## 8. 面试常见问题

### Q1: 图片缓存什么时候清理？

- 窗口关闭时，对应 CPaintManagerUI 析构会清理实例缓存
- 调用 RemoveImage(name) 可手动清理单张图片
- 调用 RemoveAllImages() 清理所有缓存（换肤时使用）
- 共享缓存在程序退出时清理

注意：缓存只存一份图片数据，多个控件引用同一图片不会重复加载。

### Q2: 为什么 ZIP 加载比文件系统快？

- 文件系统每加载一张图片就是一次磁盘 I/O（open/read/close）
- ZIP 模式将整包加载到内存后，后续读取都是内存操作
- 减少了大量 syscall 和磁盘寻道时间
- 特别在控件众多、图片资源丰富时差异明显

### Q3: 如何实现运行时动态换肤不闪烁？

```cpp
void SmoothSwitchSkin(LPCTSTR newPath)
{
    // 1. 先锁住绘制（不响应 WM_PAINT）
    ::LockWindowUpdate(m_hWnd);

    // 2. 清缓存、切路径
    m_pm.RemoveAllImages();
    CPaintManagerUI::SetResourcePath(newPath);

    // 3. 重建或刷新
    ::InvalidateRect(m_hWnd, NULL, FALSE);

    // 4. 解锁绘制（一次性刷新）
    ::LockWindowUpdate(NULL);
}
```

### Q4: 图片的 source 和 dest 参数有什么用？

- `source`：从图片文件中裁剪出一块区域（实现精灵图 Sprite Sheet）
- `dest`：指定绘制到控件内的位置（不是铺满整个控件）

```xml
<!-- 精灵图：一张大图中裁出不同状态 -->
<!-- sprite.png 是 320x40 的图，每个状态 80x40 -->
<Button
    normalimage="file='sprite.png' source='0,0,80,40'"
    hotimage="file='sprite.png' source='80,0,160,40'"
    pushedimage="file='sprite.png' source='160,0,240,40'"
    disabledimage="file='sprite.png' source='240,0,320,40'"
/>
```

---

## 9. 常见陷阱

```cpp
// ❌ 换肤后忘记清除图片缓存，显示的还是旧图片
CPaintManagerUI::SetResourcePath(_T("skin\\dark\\"));
::InvalidateRect(m_hWnd, NULL, FALSE);
// 重绘时从缓存读到旧图片！

// ✅ 必须先清缓存
m_pm.RemoveAllImages();
CPaintManagerUI::SetResourcePath(_T("skin\\dark\\"));
::InvalidateRect(m_hWnd, NULL, FALSE);
```

```xml
<!-- ❌ 图片路径使用反斜杠在某些 ZIP 实现中可能找不到 -->
<Button normalimage="images\btn.png" />

<!-- ✅ 使用正斜杠更安全（两者兼容） -->
<Button normalimage="images/btn.png" />
```

```cpp
// ❌ 手动 DeleteObject 缓存中的 HBITMAP
TImageInfo* pInfo = m_pm.GetImageEx(_T("btn.png"));
::DeleteObject(pInfo->hBitmap);  // 破坏了缓存！其他控件再用会崩溃

// ✅ 通过 RemoveImage 让 PaintManager 统一管理
m_pm.RemoveImage(_T("btn.png"));
```

---

## 10. 总结

| 特性 | 说明 |
|------|------|
| **资源来源** | 文件系统 / ZIP / EXE 资源 |
| **图片缓存** | 按路径哈希缓存，共享 + 实例两级 |
| **图片格式** | PNG（推荐）/ JPG / BMP / GIF |
| **九宫格** | corner 属性实现自适应拉伸 |
| **字体管理** | Font 节点注册，按 id 引用 |
| **换肤原理** | 切换资源路径 + 清缓存 + 重绘/重建 |
| **HSL 调色** | 动态改变色相/饱和度实现无资源换色 |
| **ZIP 打包** | 减少磁盘 I/O，支持嵌入 EXE |
