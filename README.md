<div align="center">

# 💡 灵犀 / MindSync

### 智能跨平台剪贴板同步解决方案 | Intelligent Cross-Platform Clipboard Synchronization Solution

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux%20%7C%20Android%20%7C%20iOS-lightgrey)](https://github.com/liulanci/-MindSync)
[![Version](https://img.shields.io/badge/Version-1.0.0-green.svg)](https://github.com/liulanci/-MindSync/releases)
[![GitHub stars](https://img.shields.io/github/stars/liulanci/-MindSync?style=social)](https://github.com/liulanci/-MindSync/stargazers)
[![ESLint](https://img.shields.io/badge/ESLint-0%20errors-brightgreen.svg)](docs/Verification-Report.md)
[![Security](https://img.shields.io/badge/Security-0%20vulnerabilities-brightgreen.svg)](docs/Verification-Report.md)
[![Code Quality](https://img.shields.io/badge/Code_Quality-A-brightgreen.svg)](docs/Verification-Report.md)

**心有灵犀 · 同步无界**  
**When Minds Connect, Data Flows**

[English](#english) | [中文](#中文)

</div>

---

<div id="中文">

## 🌟 项目简介

**灵犀 (MindSync)** 是一款开源的跨平台剪贴板同步工具，支持在 Windows、macOS、Linux、Android 和 iOS 设备间实现剪贴板内容的实时同步。采用端到端加密技术保障数据安全，支持私有化部署。

### ✨ 核心特性

- **🔗 跨设备实时同步** - 文本、图片内容实时同步
- **🔒 端到端加密** - AES-256-GCM 加密保障数据安全
- **📱 多平台支持** - 覆盖 Windows / macOS / Linux / Android / iOS
- **🌐 云原生架构** - Docker 部署，支持私有化
- **📖 开源免费** - MIT 许可证，完全开源

### 🛠️ 技术架构

| 组件 | 技术选型 | 说明 |
|------|----------|------|
| **服务端** | Node.js + Express | RESTful API + WebSocket |
| **数据库** | MySQL 8.0+ | 用户数据持久化存储 |
| **实时通信** | Socket.IO | 剪贴板内容实时同步 |
| **加密** | AES-256-GCM + JWT | 端到端加密 + 身份认证 |
| **桌面客户端** | Electron + React | 跨平台桌面应用 |
| **移动客户端** | Kotlin (Android) | Android 原生应用 |
| **输入法引擎** | C++17 + RIME | 跨平台输入法方案 |
| **部署** | Docker + Nginx | 容器化部署方案 |

### 🏗️ 系统架构

```
┌─────────────────────────────────────────────┐
│                  客户端层                     │
│  桌面端(Electron)  移动端(Android/iOS)        │
└──────────────────────┬──────────────────────┘
                       │ WebSocket / REST API
┌──────────────────────▼──────────────────────┐
│                  服务端层                     │
│  认证服务  同步服务  设备管理  历史管理         │
└──────────────────────┬──────────────────────┘
                       │
┌──────────────────────▼──────────────────────┐
│                  数据层                       │
│  MySQL  Redis  文件存储                       │
└─────────────────────────────────────────────┘
```

### 🔒 安全设计

```
应用层安全
├── 输入验证与过滤
├── SQL注入防护
├── XSS攻击防护
└── CSRF令牌保护

传输层安全
├── TLS 加密传输
└── JWT 身份认证

数据层安全
├── AES-256-GCM 端到端加密
├── bcrypt 密码哈希
└── 访问权限控制
```

### 🚀 快速开始

#### 系统要求
- **服务器端**: Node.js 16+, MySQL 8.0+
- **客户端**: Windows 10+ / macOS 10.14+ / Linux Ubuntu 18.04+ / Android 8.0+ / iOS 12.0+

#### 安装步骤

1. **克隆仓库**
```bash
git clone https://github.com/liulanci/-MindSync.git
cd -MindSync
```

2. **服务器部署**
```bash
cd server
npm install
cp .env.example .env
# 编辑 .env 文件配置数据库等信息
npm start
```

3. **Docker 部署**
```bash
docker-compose up -d
```

4. **客户端安装**
- 桌面端：进入 `client/` 目录构建
- Android：进入 `android-app/` 目录构建 APK

### 📖 文档

| 文档 | 说明 |
|------|------|
| [快速入门](docs/Getting-Started.md) | 5分钟上手指南 |
| [安装指南](docs/Installation-Guide.md) | 详细安装步骤 |
| [用户手册](docs/User-Manual.md) | 完整功能说明 |
| [API文档](docs/API-Documentation.md) | 接口参考文档 |
| [架构设计](docs/Architecture-Design.md) | 技术架构详解 |
| [技术规范](docs/Technical-Specifications.md) | 技术规格说明 |
| [品牌口号](docs/Brand-Slogans.md) | 品牌标识体系 |
| [验证报告](docs/Verification-Report.md) | 代码质量验证报告 |

### 🎯 品牌口号

- **心有灵犀，同步无界** - 核心品牌理念
- **智慧连接，数据随行** - 功能价值主张
- **When Minds Connect, Data Flows** - Core brand philosophy
- **Intelligent Connections, Data On-the-Go** - Functional value proposition

### 🤝 贡献指南

我们欢迎社区贡献！请阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何参与项目开发。

### 📄 许可证

本项目采用 [MIT 许可证](LICENSE)。

### ⚠️ 免责声明

本项目按"原样"提供，不提供任何明示或暗示的担保。用户需自行承担使用风险。

### 📞 联系方式

- **GitHub Issues**: [问题反馈](https://github.com/liulanci/-MindSync/issues)
- **GitHub Discussions**: [社区讨论](https://github.com/liulanci/-MindSync/discussions)

</div>

---

<div id="english">

## 🌟 Project Introduction

**MindSync** is an open-source cross-platform clipboard synchronization tool that enables real-time clipboard content synchronization between Windows, macOS, Linux, Android, and iOS devices. It uses end-to-end encryption to ensure data security and supports self-hosted deployment.

### ✨ Core Features

- **🔗 Cross-Device Real-time Sync** - Instant text and image synchronization
- **🔒 End-to-End Encryption** - AES-256-GCM encryption for data security
- **📱 Multi-Platform Support** - Windows / macOS / Linux / Android / iOS
- **🌐 Cloud-Native Architecture** - Docker deployment, self-hosted supported
- **📖 Open Source & Free** - MIT License, fully open source

### 🛠️ Technical Architecture

| Component | Technology | Description |
|-----------|------------|-------------|
| **Server** | Node.js + Express | RESTful API + WebSocket |
| **Database** | MySQL 8.0+ | Persistent data storage |
| **Real-time** | Socket.IO | Real-time clipboard sync |
| **Encryption** | AES-256-GCM + JWT | E2E encryption + Authentication |
| **Desktop Client** | Electron + React | Cross-platform desktop app |
| **Mobile Client** | Kotlin (Android) | Android native app |
| **IME Engine** | C++17 + RIME | Cross-platform IME solution |
| **Deployment** | Docker + Nginx | Containerized deployment |

### 🚀 Quick Start

#### System Requirements
- **Server**: Node.js 16+, MySQL 8.0+
- **Client**: Windows 10+ / macOS 10.14+ / Linux Ubuntu 18.04+ / Android 8.0+ / iOS 12.0+

#### Installation

1. **Clone Repository**
```bash
git clone https://github.com/liulanci/-MindSync.git
cd -MindSync
```

2. **Server Deployment**
```bash
cd server
npm install
cp .env.example .env
# Edit .env file to configure database etc.
npm start
```

3. **Docker Deployment**
```bash
docker-compose up -d
```

### 📖 Documentation

| Document | Description |
|----------|-------------|
| [Getting Started](docs/Getting-Started.md) | 5-minute quick start guide |
| [Installation Guide](docs/Installation-Guide.md) | Detailed installation steps |
| [User Manual](docs/User-Manual.md) | Complete feature guide |
| [API Documentation](docs/API-Documentation.md) | API reference |
| [Architecture Design](docs/Architecture-Design.md) | Technical architecture |
| [Technical Specs](docs/Technical-Specifications.md) | Technical specifications |
| [Brand Slogans](docs/Brand-Slogans.md) | Brand identity system |

### 🤝 Contributing

We welcome community contributions! Please read [CONTRIBUTING.md](CONTRIBUTING.md) to learn how to participate.

### 📄 License

This project is licensed under the [MIT License](LICENSE).

### ⚠️ Disclaimer

This project is provided "as is", without any express or implied warranties. Users assume all risks associated with its use.

### 📞 Contact

- **GitHub Issues**: [Report Issues](https://github.com/liulanci/-MindSync/issues)
- **GitHub Discussions**: [Community Discussion](https://github.com/liulanci/-MindSync/discussions)

</div>

---

<div align="center">

**⭐ 如果灵犀/MindSync对您有帮助，请给我们一个星标！**  
**⭐ If MindSync helps you, please give us a star!**

</div>