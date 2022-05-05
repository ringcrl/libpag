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

```sh
# 删除 third_party 编译内容 out

# 查看 build 帮助
node build_vendor -h

# 修改 vendor_tool/CMake.js，buildArch 方法添加 buildType + " -DCMAKE_POSITION_INDEPENDENT_CODE=ON"

# 重新编译 third_party
node build_vendor -p linux

# 编译 pag
./build_node.sh

# 编译 .node
cd node
npm run build

# 测试运行 pag
npm run test
```
