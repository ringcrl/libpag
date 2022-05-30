# 记录依赖安装流程

# 更新依赖环境
apt-get update

# 安装 git
apt-get install git
apt-get install git-lfs

# 安装 node
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.38.0/install.sh | bash
source ~/.bashrc
nvm install 16

# 安装依赖安装器
npm install -g depsync

# linux 需要安装 libx11-dev
apt-get install libx11-dev

# 同步项目依赖
depsync

# 安装 cmake
mkdir -p /temp && cd /temp
wget https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1.tar.gz
tar -zxvf cmake-3.23.1.tar.gz
cd cmake-3.23.1
./bootstrap
