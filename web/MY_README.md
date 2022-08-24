# 查看 diff 记录

https://github.com/Tencent/libpag/compare/main...ringcrl:libpag:node

# 合并上游修改

```sh
# 合并上游项目修改到 node 分支
git checkout node
git pull upstream main
```

# 本地开发

```sh
# 添加 emcc 编译环境到 .bashrc
source "/data/github.com/emsdk/emsdk_env.sh" # Linux
source "/Users/ringcrl/Documents/github/emsdk/emsdk_env.sh" # Mac

# 编译
cd script

# 先删除历史构建产物
rm -rf /data/github.com/libpag/build

# debug 版本
./build.sh debug web
./build.sh debug node
npm run build:watch # debug 版本需要手动编译 pag 入口文件

# release 版本
./build.sh release web
./build.sh release node
```
