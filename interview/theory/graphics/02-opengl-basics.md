# 02 - OpenGL 基础 (OpenGL Basics)

## 简介

OpenGL 是跨平台的图形渲染 API。

---

## 1. 渲染管线

```
┌─────────────┐
│  顶点数据   │
└──────┬──────┘
       ▼
┌─────────────┐
│ 顶点着色器  │ ← 可编程：变换顶点位置
└──────┬──────┘
       ▼
┌─────────────┐
│  图元装配   │ ← 组成三角形等
└──────┬──────┘
       ▼
┌─────────────┐
│   光栅化    │ ← 生成片段（像素）
└──────┬──────┘
       ▼
┌─────────────┐
│ 片段着色器  │ ← 可编程：计算颜色
└──────┬──────┘
       ▼
┌─────────────┐
│ 测试与混合  │ ← 深度测试、模板测试
└──────┬──────┘
       ▼
┌─────────────┐
│  帧缓冲    │
└─────────────┘
```

---

## 2. VAO/VBO/EBO

```cpp
// VBO：顶点缓冲对象
GLuint VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// EBO：索引缓冲对象
GLuint EBO;
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// VAO：顶点数组对象（存储顶点属性配置）
GLuint VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// 配置顶点属性
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

---

## 3. 着色器

### 3.1 顶点着色器

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
```

### 3.2 片段着色器

```glsl
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
```

---

## 4. 纹理

```cpp
// 加载纹理
GLuint texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

// 设置纹理参数
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 上传纹理数据
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```

---

## 5. 面试常见问题

**Q1：渲染管线的主要阶段？**
- 顶点着色器：变换顶点
- 光栅化：生成片段
- 片段着色器：计算颜色

**Q2：VAO/VBO/EBO 的作用？**
- VBO：存储顶点数据
- EBO：存储索引，减少重复顶点
- VAO：存储顶点属性配置

**Q3：Mipmap 是什么？**
- 预生成不同分辨率的纹理
- 减少远处纹理的走样
- 提高渲染效率
