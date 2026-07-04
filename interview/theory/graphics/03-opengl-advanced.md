# 03 - OpenGL 进阶 (OpenGL Advanced)

## 简介

OpenGL 高级技术：光照、阴影、帧缓冲等。

---

## 1. 光照模型

### 1.1 Phong 光照

```
Phong = Ambient + Diffuse + Specular

环境光：I_a = k_a × L_a
漫反射：I_d = k_d × L_d × max(0, N·L)
镜面反射：I_s = k_s × L_s × max(0, R·V)^n
```

```glsl
// 片段着色器
vec3 ambient = ambientStrength * lightColor;

vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;

vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
vec3 specular = specularStrength * spec * lightColor;

vec3 result = (ambient + diffuse + specular) * objectColor;
FragColor = vec4(result, 1.0);
```

### 1.2 Blinn-Phong

```glsl
// 使用半程向量，更高效
vec3 halfwayDir = normalize(lightDir + viewDir);
float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
```

---

## 2. 阴影

### 2.1 Shadow Mapping

```
1. 从光源视角渲染深度图
2. 从相机视角渲染场景
3. 将片段变换到光源空间
4. 比较深度：若片段深度 > 深度图，则在阴影中
```

```glsl
// 阴影计算
float shadow = 0.0;
vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
projCoords = projCoords * 0.5 + 0.5;
float closestDepth = texture(shadowMap, projCoords.xy).r;
float currentDepth = projCoords.z;
shadow = currentDepth > closestDepth + bias ? 1.0 : 0.0;
```

---

## 3. 帧缓冲

```cpp
// 创建帧缓冲
GLuint FBO;
glGenFramebuffers(1, &FBO);
glBindFramebuffer(GL_FRAMEBUFFER, FBO);

// 附加颜色附件
GLuint texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

// 附加深度缓冲
GLuint RBO;
glGenRenderbuffers(1, &RBO);
glBindRenderbuffer(GL_RENDERBUFFER, RBO);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
```

---

## 4. 后处理效果

### 4.1 反相

```glsl
FragColor = vec4(1.0 - texture(screenTexture, TexCoords).rgb, 1.0);
```

### 4.2 高斯模糊

```glsl
const float kernel[9] = float[](
    1.0/16, 2.0/16, 1.0/16,
    2.0/16, 4.0/16, 2.0/16,
    1.0/16, 2.0/16, 1.0/16
);
```

---

## 5. 面试常见问题

**Q1：Phong 和 Blinn-Phong 区别？**
- Phong：使用反射向量
- Blinn-Phong：使用半程向量，计算更快

**Q2：Shadow Mapping 原理？**
- 从光源渲染深度图
- 比较片段深度判断阴影

**Q3：帧缓冲的用途？**
- 离屏渲染
- 后处理效果
- 多目标渲染
