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

# 问题定位

## 符号链接错误

```sh
# 找到具体函数
c++filt <symbol>

# 确认 pathkit 有问题，在 third_party/pathkit/CMakeLists.txt 添加
# add_compile_options(-fPIC)

# 重新编译 third_party
node build_vendor -p linux
```

## 所有 third_party 添加 -fPIC

```cpp

```
