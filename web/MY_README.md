# 本地开发

```sh
# 添加编译环境
source "/data/github.com/emsdk/emsdk_env.sh"

# 编译
cd script
# 编译 node 版本
./build.sh debug node
# 编译 web 版本
./build.sh debug web
# 编译 release 版本
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
