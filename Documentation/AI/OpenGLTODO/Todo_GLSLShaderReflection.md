# OpenGL着色器反射系统实现计划

## 背景

为了与D3D12反射系统保持一致，我们需要为OpenGL着色器实现类似的反射功能。OpenGL没有像D3D12那样的独立反射API，但可以通过一系列内置函数查询着色器程序信息。

## 实现步骤

### 1. 设计OpenGLShaderReflection类

- 实现`ShaderReflection`抽象接口
- 添加缓存机制，避免重复查询
- 设计OpenGL特有的辅助函数

```cpp
class OpenGLShaderReflection : public ShaderReflection
{
public:
    OpenGLShaderReflection(GLuint program);
    virtual ~OpenGLShaderReflection();
    
    virtual BufferLayout ReflectVertexInputLayout() override;
    virtual std::vector<ShaderParameter> ReflectParameters() override;
    virtual std::vector<ResourceBinding> ReflectResourceBindings() override;
    
private:
    GLuint m_Program;
    
    // 缓存结果
    BufferLayout m_InputLayout;
    std::vector<ShaderParameter> m_Parameters;
    std::vector<ResourceBinding> m_ResourceBindings;
    
    // 缓存标志
    bool m_HasReflectedInputLayout = false;
    bool m_HasReflectedParameters = false;
    bool m_HasReflectedResourceBindings = false;
    
    // 辅助函数
    ShaderDataType GLTypeToShaderDataType(GLenum type);
};
```

### 2. 实现顶点输入布局反射

- 利用`glGetActiveAttrib`获取着色器顶点属性
- 将OpenGL类型转换为引擎的`ShaderDataType`
- 构建`BufferLayout`
- 实现示例:

```cpp
BufferLayout OpenGLShaderReflection::ReflectVertexInputLayout()
{
    if (m_HasReflectedInputLayout)
        return m_InputLayout;
        
    std::vector<BufferElement> elements;
    
    // 获取活动属性数量
    GLint attributeCount = 0;
    glGetProgramiv(m_Program, GL_ACTIVE_ATTRIBUTES, &attributeCount);
    
    // 查询属性信息并构建元素
    for (GLint i = 0; i < attributeCount; i++)
    {
        GLchar name[128];
        GLint size;
        GLenum type;
        glGetActiveAttrib(m_Program, i, sizeof(name), nullptr, &size, &type, name);
        
        GLint location = glGetAttribLocation(m_Program, name);
        if (location != -1)
        {
            ShaderDataType dataType = GLTypeToShaderDataType(type);
            // 解析语义索引（从属性名称提取，如position0, normal1等）
            uint32_t semanticIndex = 0; // 需要设计一套规则从名称中提取索引
            
            elements.push_back({ dataType, name, semanticIndex });
        }
    }
    
    m_InputLayout = BufferLayout(elements);
    m_HasReflectedInputLayout = true;
    return m_InputLayout;
}
```

### 3. 实现着色器参数反射

- 使用`glGetActiveUniform`和`glGetUniformLocation`获取uniform变量
- 使用`glGetActiveUniformBlock*`系列函数获取uniform块信息
- 将信息填充到`ShaderParameter`中
- 实现示例:

```cpp
std::vector<ShaderParameter> OpenGLShaderReflection::ReflectParameters()
{
    if (m_HasReflectedParameters)
        return m_Parameters;
        
    m_Parameters.clear();
    
    // 获取活动uniform变量数量
    GLint uniformCount = 0;
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &uniformCount);
    
    // 查询uniform变量信息
    for (GLint i = 0; i < uniformCount; i++)
    {
        GLchar name[128];
        GLint size;
        GLenum type;
        glGetActiveUniform(m_Program, i, sizeof(name), nullptr, &size, &type, name);
        
        GLint location = glGetUniformLocation(m_Program, name);
        if (location != -1)
        {
            ShaderParameter param;
            param.Name = name;
            param.Size = GetTypeSize(type) * size;
            param.Offset = 0; // OpenGL不直接提供偏移信息
            param.BindPoint = location;
            
            m_Parameters.push_back(param);
        }
    }
    
    // 处理Uniform块
    GLint blockCount = 0;
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
    
    for (GLint i = 0; i < blockCount; i++)
    {
        // 获取块名称
        GLchar blockName[128];
        glGetActiveUniformBlockName(m_Program, i, sizeof(blockName), nullptr, blockName);
        
        // 获取块大小
        GLint blockSize = 0;
        glGetActiveUniformBlockiv(m_Program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        
        ShaderParameter param;
        param.Name = blockName;
        param.Size = blockSize;
        param.Offset = 0;
        param.BindPoint = i; // 块索引
        
        m_Parameters.push_back(param);
    }
    
    m_HasReflectedParameters = true;
    return m_Parameters;
}
```

### 4. 实现资源绑定反射

- 使用`glGetProgramInterfaceiv`和`glGetProgramResourceiv`（OpenGL 4.3+）获取资源信息
- 对于早期版本的OpenGL，可能需要依赖命名约定
- 实现示例:

```cpp
std::vector<ResourceBinding> OpenGLShaderReflection::ReflectResourceBindings()
{
    if (m_HasReflectedResourceBindings)
        return m_ResourceBindings;
        
    m_ResourceBindings.clear();
    
    // 在OpenGL 4.3+中使用着色器存储块反射
#if defined(GL_VERSION_4_3)
    // 查询着色器存储缓冲对象
    GLint ssboCount = 0;
    glGetProgramInterfaceiv(m_Program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &ssboCount);
    
    for (GLint i = 0; i < ssboCount; i++)
    {
        GLchar name[128];
        glGetProgramResourceName(m_Program, GL_SHADER_STORAGE_BLOCK, i, sizeof(name), nullptr, name);
        
        // 获取绑定点
        GLenum props = GL_BUFFER_BINDING;
        GLint binding = 0;
        glGetProgramResourceiv(m_Program, GL_SHADER_STORAGE_BLOCK, i, 1, &props, 1, nullptr, &binding);
        
        ResourceBinding resource;
        resource.Name = name;
        resource.BindPoint = binding;
        resource.BindSpace = 0; // OpenGL没有绑定空间的概念
        
        m_ResourceBindings.push_back(resource);
    }
    
    // 查询图像单元（texture units）
    // 这部分可能需要依赖命名约定
#else
    // 在早期OpenGL版本中，可能需要采用命名约定或其他方式推断资源绑定
#endif
    
    m_HasReflectedResourceBindings = true;
    return m_ResourceBindings;
}
```

### 5. 更新OpenGLShader类

- 修改OpenGLShader构造函数，增加反射支持
- 实现新增的Shader抽象接口方法
- 更新输入布局生成逻辑

```cpp
void OpenGLShader::CreateReflection()
{
    // 创建反射对象
    m_Reflection = CreateRef<OpenGLShaderReflection>(m_RendererID);
    
    // 获取输入布局
    m_InputLayout = m_Reflection->ReflectVertexInputLayout();
}
```

### 6. 类型转换辅助函数

实现OpenGL类型到引擎类型的转换：

```cpp
ShaderDataType OpenGLShaderReflection::GLTypeToShaderDataType(GLenum type)
{
    switch (type)
    {
        case GL_FLOAT:             return ShaderDataType::Float;
        case GL_FLOAT_VEC2:        return ShaderDataType::Float2;
        case GL_FLOAT_VEC3:        return ShaderDataType::Float3;
        case GL_FLOAT_VEC4:        return ShaderDataType::Float4;
        case GL_FLOAT_MAT3:        return ShaderDataType::Mat3;
        case GL_FLOAT_MAT4:        return ShaderDataType::Mat4;
        case GL_INT:               return ShaderDataType::Int;
        case GL_INT_VEC2:          return ShaderDataType::Int2;
        case GL_INT_VEC3:          return ShaderDataType::Int3;
        case GL_INT_VEC4:          return ShaderDataType::Int4;
        case GL_BOOL:              return ShaderDataType::Bool;
        default:                    return ShaderDataType::None;
    }
}
```

## 注意事项和挑战

1. **命名约定**：OpenGL没有语义概念（如POSITION, NORMAL等），可能需要依赖命名约定来匹配D3D12的行为

2. **内存布局差异**：OpenGL和D3D12的内存布局规则不同，反射时需要考虑

3. **API版本支持**：一些反射功能（如着色器存储块）只在较新的OpenGL版本中可用，需要版本检查和后备方案

4. **同步实现**：确保OpenGL反射系统与D3D12版本保持功能一致性，提供同样的接口和行为

## 下一步工作

1. 实现`OpenGLShaderReflection`类基本框架
2. 完成顶点输入布局反射
3. 实现Uniform变量和块的反射
4. 添加资源绑定反射（根据OpenGL版本采取不同策略）
5. 更新`OpenGLShader`类
6. 测试与D3D12版本的兼容性
7. 编写使用文档和示例
