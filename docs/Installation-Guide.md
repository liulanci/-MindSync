# 安装指南

本文档提供灵犀/MindSync在各个平台上的详细安装说明。

## 📋 系统要求

### 服务器端要求
- **操作系统**: Linux (Ubuntu 18.04+, CentOS 7+), Windows Server 2016+, macOS Server
- **Node.js**: 16.x 或更高版本
- **数据库**: MySQL 8.0+ 或 PostgreSQL 12+
- **内存**: 最低 2GB，推荐 4GB+
- **存储**: 最低 10GB 可用空间

### 客户端要求

| 平台 | 最低版本 | 推荐版本 | 架构支持 |
|------|----------|----------|----------|
| Windows | Windows 10 | Windows 11 | x64, ARM64 |
| macOS | macOS 10.14 | macOS 12+ | Intel, Apple Silicon |
| Linux | Ubuntu 18.04 | Ubuntu 20.04+ | x64, ARM64 |
| Android | Android 8.0 | Android 12+ | ARM, ARM64 |
| iOS | iOS 12.0 | iOS 15+ | ARM64 |

## 🖥️ 桌面端安装

### Windows 安装

#### 方法一：安装包（推荐）
1. 下载最新版本的 `.exe` 安装包
2. 双击运行安装程序
3. 按照向导完成安装
4. 启动灵犀应用

#### 方法二：Microsoft Store
1. 打开 Microsoft Store
2. 搜索 "MindSync"
3. 点击"获取"进行安装
4. 启动应用

#### 方法三：便携版
1. 下载 `.zip` 便携版
2. 解压到任意目录
3. 运行 `MindSync.exe`

### macOS 安装

#### 方法一：DMG安装包
1. 下载 `.dmg` 文件
2. 双击打开DMG镜像
3. 将灵犀图标拖拽到应用程序文件夹
4. 从启动台启动应用

#### 方法二：Homebrew
```bash
brew install --cask mindsync
```

#### 方法三：Mac App Store
1. 打开 App Store
2. 搜索 "MindSync"
3. 点击"获取"进行安装

### Linux 安装

#### Ubuntu/Debian
```bash
# 下载DEB包
wget https://github.com/liulanci/-MindSync/releases/latest/download/mindsync_amd64.deb

# 安装
sudo dpkg -i mindsync_amd64.deb
sudo apt-get install -f  # 修复依赖

# 启动
mindsync
```

#### CentOS/RHEL/Fedora
```bash
# 下载RPM包
wget https://github.com/liulanci/-MindSync/releases/latest/download/mindsync.x86_64.rpm

# 安装
sudo rpm -i mindsync.x86_64.rpm

# 启动
mindsync
```

#### Arch Linux
```bash
# 使用AUR
yay -S mindsync

# 或手动安装
git clone https://aur.archlinux.org/mindsync.git
cd mindsync
makepkg -si
```

## 📱 移动端安装

### Android 安装

#### 方法一：Google Play Store
1. 打开 Google Play Store
2. 搜索 "MindSync"
3. 点击"安装"
4. 打开应用完成设置

#### 方法二：APK手动安装
1. 下载最新APK文件
2. 在设备上启用"未知来源"安装
3. 使用文件管理器找到APK文件
4. 点击安装并确认权限

#### 方法三：F-Droid
1. 添加F-Droid仓库
2. 搜索 "MindSync"
3. 下载并安装

### iOS 安装

#### App Store安装
1. 打开 App Store
2. 搜索 "MindSync"
3. 点击"获取"并验证
4. 等待安装完成

#### TestFlight测试版
1. 安装 TestFlight
2. 接受测试邀请
3. 下载测试版本

## 🖥️ 服务器端安装

### Docker安装（推荐）

#### 使用Docker Compose
```yaml
# docker-compose.yml
version: '3.8'
services:
  mindsync-server:
    image: mindsync/server:latest
    ports:
      - "3000:3000"
    environment:
      - DB_HOST=db
      - DB_USER=mindsync
      - DB_PASSWORD=your_password
      - DB_NAME=mindsync
    depends_on:
      - db
    
  db:
    image: mysql:8.0
    environment:
      - MYSQL_ROOT_PASSWORD=root_password
      - MYSQL_DATABASE=mindsync
      - MYSQL_USER=mindsync
      - MYSQL_PASSWORD=your_password
    volumes:
      - db_data:/var/lib/mysql

volumes:
  db_data:
```

启动服务：
```bash
docker-compose up -d
```

### 手动安装

#### 1. 环境准备
```bash
# 安装Node.js
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs

# 安装MySQL
sudo apt-get install mysql-server
```

#### 2. 数据库配置
```sql
CREATE DATABASE mindsync CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'mindsync'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON mindsync.* TO 'mindsync'@'localhost';
FLUSH PRIVILEGES;
```

#### 3. 应用部署
```bash
# 克隆代码
git clone https://github.com/liulanci/-MindSync.git
cd -MindSync/server

# 安装依赖
npm install

# 配置环境变量
cp .env.example .env
# 编辑 .env 文件配置数据库等信息

# 启动服务
npm start
```

## 🔧 开发环境安装

### 前置要求
- Node.js 16+
- Git
- 代码编辑器（VS Code推荐）

### 环境搭建
```bash
# 克隆仓库
git clone https://github.com/liulanci/-MindSync.git
cd -MindSync

# 安装依赖
npm install

# 启动开发服务器
npm run dev

# 运行测试
npm test
```

### 开发工具配置

#### VS Code推荐扩展
- ESLint
- Prettier
- GitLens
- Docker
- Thunder Client（API测试）

## 🐳 容器化部署

### Kubernetes部署
```yaml
# mindsync-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: mindsync-server
spec:
  replicas: 3
  selector:
    matchLabels:
      app: mindsync
  template:
    metadata:
      labels:
        app: mindsync
    spec:
      containers:
      - name: mindsync
        image: mindsync/server:latest
        ports:
        - containerPort: 3000
        env:
        - name: DB_HOST
          value: "mysql-service"
        - name: DB_USER
          valueFrom:
            secretKeyRef:
              name: mindsync-secrets
              key: db-user
        - name: DB_PASSWORD
          valueFrom:
            secretKeyRef:
              name: mindsync-secrets
              key: db-password
```

## 🔍 安装验证

### 客户端验证
1. 启动应用
2. 检查系统托盘图标
3. 验证网络连接状态
4. 测试基本功能

### 服务器验证
```bash
# 检查服务状态
curl http://localhost:3000/health

# 预期响应
{"status":"healthy","version":"1.0.0"}
```

### 端到端测试
1. 在两台设备上安装客户端
2. 登录同一账号
3. 在一台设备复制文本
4. 在另一台设备验证同步

## ❗ 故障排除

### 常见安装问题

#### Windows安装失败
- 检查系统权限
- 关闭杀毒软件临时
- 以管理员身份运行安装程序

#### macOS安全阻止
- 系统偏好设置 → 安全性与隐私 → 通用
- 点击"仍要打开"
- 或使用命令行：`sudo spctl --master-disable`

#### Linux依赖问题
```bash
# Ubuntu/Debian
sudo apt-get update && sudo apt-get install -f

# CentOS/RHEL
sudo yum update && sudo yum install epel-release
```

#### 移动端安装失败
- 检查存储空间
- 确认系统版本兼容性
- 重启设备后重试

---

## 📞 技术支持

如果安装过程中遇到问题，请：
1. 查看详细错误信息
2. 检查系统要求是否满足
3. 查阅[故障排除指南](Troubleshooting.md)
4. 在[GitHub Issues](https://github.com/liulanci/-MindSync/issues)提交问题

---

*文档版本: v1.0.0*  
*最后更新: 2024年12月*