# 材质系统实现计划
当前我要做的， 能够读数据， 存数据，存在MaterialProperty里面。
要能根据数据创建Buffer， 并且传到对应的位置上去。 也就是模拟处理下onattach里面的逻辑。
这个地方需要什么关键信息？
MaterialProperty提供信息源， ShaderReflection.Layout提供这个数据应该填充在哪个位置，
然后把它放到一个largebuffer里面？ 这个largebuffer其实有待商榷，因为我现在发现提供一个offset就可以解决材质rootsignature更新问题。
总之我现在需要解决的最大问题是利用shader数据， 组织起来buffer，然后再传递到rootsignature中，
这一步需要实现下
## 核心组件

- **材质资产**：JSON或Meta文件格式，存储材质参数、着色器路径和渲染状态
- **材质类**：运行时加载和管理材质资产
- **着色器系统**：与材质系统协同工作
- **材质实例**：支持材质变体和实例重载

## 阶段一：基础架构实现

- [x] 定义材质属性类型枚举（Float, Vec2, Vec3, Vec4, Int, Texture等）
- [x] 实现材质属性类（MaterialProperty）
  - [x] 支持多种数据类型（使用std::variant）
  - [x] 类型安全的访问器
- [x] 实现材质基类（Material）
  - [x] 属性设置/获取方法
  - [x] 与Shader系统集成
  - [ ] 渲染状态设置方法
- [x] 实现JSON序列化和反序列化
  - [x] 从JSON读取材质配置
  - [x] 将材质配置保存为JSON

## 阶段二：着色器反射与同步系统

- [x] 实现着色器反射系统
  - [x] 提取着色器中的uniform变量和采样器
  - [x] 分析类型和绑定点
  - [x] 生成反射数据
- [ ] 创建着色器缓存系统
  - [ ] 缓存已编译着色器的反射数据
  - [ ] 实现反射数据的序列化与持久化
- [x] 实现材质验证器
  - [x] 检查材质属性与着色器是否匹配
  - [x] 标识缺失属性和类型不匹配
  - [x] 提供自动修复机制

## 阶段三：特定材质类型

- [ ] 实现PBR材质
  - [ ] 标准物理参数（反照率、金属度、粗糙度等）
  - [ ] 贴图通道（颜色、法线、金属度、粗糙度等）
  - [ ] PBR渲染状态配置
- [ ] 实现Unlit材质
  - [ ] 基本参数（颜色、透明度）
  - [ ] 纹理通道
  - [ ] 无光照渲染状态配置
- [ ] 实现自定义材质基础设施
  - [ ] 支持用户扩展材质系统
  - [ ] 自定义着色器绑定

## 阶段四：材质库与资源管理

- [x] 实现材质库
  - [x] 材质注册与查找
  - [x] 资源缓存管理
  - [x] 材质实例创建
- [ ] 实现材质变体系统
  - [ ] 支持基于宏定义的变体
  - [ ] 变体自动生成
  - [ ] 变体缓存
- [ ] 实现材质资源热重载
  - [ ] 监控材质文件变化
  - [ ] 动态更新运行时材质

## 阶段五：Shader-Material同步系统

- [x] 实现智能检测与同步机制
  - [x] 当着色器修改时，标记受影响的材质
  - [x] 提供批量更新工具
  - [x] 差异对比与选择性更新
- [x] 实现冲突解决策略
  - [x] 保留材质现有值选项
  - [x] 使用着色器默认值选项
  - [x] 提供手动解决冲突界面
- [ ] 实现版本管理系统
  - [ ] 在材质Meta中记录着色器哈希/版本
  - [ ] 支持版本兼容性映射
  - [ ] 旧版材质升级路径

## 阶段六：编辑器集成

- [ ] 实现材质编辑器UI
  - [ ] 属性编辑界面
  - [ ] 实时预览
  - [ ] 贴图通道编辑
- [ ] 实现材质创建向导
  - [ ] 基于模板创建材质
  - [ ] 智能推荐设置
- [ ] 整合资产管道
  - [ ] 导入/导出材质
  - [ ] 材质依赖管理
  - [ ] 批量操作

## 关于Shader和Meta同步的解决方案

### 工作流程

1. **着色器为主，材质自适应**
   - 着色器作为"真相来源"(source of truth)
   - 材质Meta文件根据着色器自动调整
   - 添加新参数时使用默认值填充

2. **智能检测与同步**
   - 自动检测着色器变更影响
   - 提供批量更新工具
   - 支持差异对比和选择性更新

3. **冲突解决策略**
   - 保留材质现有值（类型匹配时）
   - 使用着色器默认值
   - 手动解决冲突选项

4. **版本管理**
   - 记录着色器哈希/版本号
   - 版本兼容性映射
   - 材质升级路径

## 材质JSON/Meta文件示例

```json
{
  "name": "StandardPBR",
  "type": "PBR",
  "shader": "assets/shaders/PBR.glsl",
  "properties": {
    "u_Albedo": { "type": "vec3", "value": [0.8, 0.2, 0.2] },
    "u_Metallic": { "type": "float", "value": 0.2 },
    "u_Roughness": { "type": "float", "value": 0.8 },
    "u_AlbedoMap": { "type": "texture2d", "value": "assets/textures/default_albedo.png" },
    "u_NormalMap": { "type": "texture2d", "value": "assets/textures/default_normal.png" }
  },
  "renderState": {
    "transparent": false,
    "cullMode": "back",
    "depthTest": true,
    "depthWrite": true,
    "blend": {
      "enabled": false,
      "srcFactor": "srcAlpha",
      "dstFactor": "oneMinusSrcAlpha"
    }
  }
}
```
