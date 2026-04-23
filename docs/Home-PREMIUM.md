<div align="center">

# 💡 灵犀 / MindSync Wiki

### 智能跨平台剪贴板同步知识库 | Intelligent Cross-Platform Clipboard Sync Knowledge Base

[![Documentation Status](https://img.shields.io/badge/文档-持续更新-brightgreen)](.)
[![Multi-Language](https://img.shields.io/badge/多语言-中英文支持-blue)](.)

**智慧连接世界，数据同步无界**  
**Intelligently Connecting Worlds, Data Synchronization Without Borders**

---

## 📚 文档导航 | Documentation Navigation

### 🚀 入门指南 | Getting Started
- [快速入门指南](Getting-Started.md) - 5分钟上手使用
- [安装指南](Installation-Guide.md) - 详细安装步骤

### 🔧 用户指南 | User Guide
- [用户手册](User-Manual.md) - 完整功能使用说明
- [API文档](API-Documentation.md) - 完整的API接口说明

### 🛠️ 开发者文档 | Developer Docs
- [架构设计](Architecture-Design.md) - 系统架构和技术选型
- [技术规范](Technical-Specifications.md) - 技术规格说明

### 📋 品牌与规范 | Brand & Standards
- [品牌口号体系](Brand-Slogans.md) - 品牌标识和口号系统

---

## 🌟 项目特色 | Project Features

### 核心功能
- **跨平台同步** - 支持 Windows、macOS、Linux、Android、iOS
- **实时传输** - 基于 WebSocket 的实时数据同步
- **端到端加密** - AES-256-GCM 加密保障数据安全
- **开源免费** - MIT 许可证，完全开源

### 技术架构
- **服务端**: Node.js + Express + Socket.IO
- **数据库**: MySQL + Redis
- **桌面客户端**: Electron + React
- **移动客户端**: Kotlin (Android)
- **输入法引擎**: C++17 + RIME
- **部署方案**: Docker + Nginx

---

## 🎯 品牌口号 | Brand Slogans

### 中文口号
- **心有灵犀，同步无界** - 核心品牌理念
- **智慧连接，数据随行** - 功能价值主张
- **安全同步，效率倍增** - 安全效率平衡

### English Slogans
- **When Minds Connect, Data Flows** - Core brand philosophy
- **Intelligent Connections, Data On-the-Go** - Functional value proposition
- **Secure Sync, Enhanced Productivity** - Security and efficiency balance

---

## 🔬 技术架构概览 | Technical Architecture Overview

### 系统架构
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

### 安全设计
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

---

## ⚖️ 法律声明 | Legal Notices

### 开源许可证
本项目采用 **MIT许可证**，允许自由使用、修改和分发。

### 免责声明
本项目按"原样"提供，不提供任何明示或暗示的担保。用户需自行承担使用风险。

---

## 📞 支持与联系 | Support & Contact

- **GitHub Issues**: [问题反馈](https://github.com/liulanci/-MindSync/issues)
- **GitHub Discussions**: [社区讨论](https://github.com/liulanci/-MindSync/discussions)

---

<div align="center">

**智慧连接世界，同步创造价值**  
**Intelligently Connecting Worlds, Sync Creating Value**

[💡 灵犀/MindSync](https://github.com/liulanci/-MindSync) | 
[📖 完整文档](.) | 
[🤝 加入社区](https://github.com/liulanci/-MindSync/discussions) | 
[🐛 报告问题](https://github.com/liulanci/-MindSync/issues)

</div>