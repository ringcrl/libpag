# 本地开发

```sh
# 添加编译环境
source "/data/github.com/emsdk/emsdk_env.sh"
# Mac
source "/Users/ringcrl/Documents/github/emsdk/emsdk_env.sh"

# 编译
cd script
# debug 版本
./build.sh debug web
./build.sh debug node
# release 版本
./build.sh release web
./build.sh release node

# 编译 pag 入口文件
npm run build:watch
```

# 查看 diff 记录

https://github.com/ringcrl/libpag/pull/1/files

# 合并上游修改

```
git checkout main
git pull upstream main

git checkout node
git pull upstream main
```
