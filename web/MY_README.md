# 本地开发

```sh
# 添加编译环境
source "/data/github.com/emsdk/emsdk_env.sh"

# 编译 debug 包
cd script
./build.sh debug

# 编译 pag 入口文件
npm run build:watch

# 通过 headless-gl 运行 pag 绘制
xvfb-run -s "-ac -screen 0 1280x1024x24" node /data/github.com/node-webgl/test/09-pag-video.js 
```
