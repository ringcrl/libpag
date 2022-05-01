# 使用 node 封装 C++ 插件

```sh
# 依赖安装
npm i

# 编译
npm run build

# 测试
npm run test
```

# API 绑定参考

`src/platform/web/PAGWasmBindings.cpp`

# VSCode 配置头文件

```json
// .vscode/c_cpp_properties.json
{
  "configurations": [
    {
      "name": "gcc-node",
      "includePath": [
        "/root/.nvm/versions/node/v16.15.0/include/node"
      ]
    }
  ],
  "version": 4
}
```
