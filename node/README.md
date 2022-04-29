# 使用 node 封装 C++ 插件

```sh
# 依赖安装
npm i

# 插件安装
npm install -g cmake-js

# 编译
npm run cmake

# 测试
node node-pag.js
```

# API 绑定参考

`src/platform/web/PAGWasmBindings.cpp`

# 配置头文件

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
