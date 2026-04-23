<div align="center">

<img src="https://raw.githubusercontent.com/liulanci/-MindSync/main/assets/logo.svg" alt="灵犀/MindSync Logo" width="100" height="100">

# 💡 灵犀 / MindSync Wiki

### 智能跨平台剪贴板同步知识库 | Intelligent Cross-Platform Clipboard Sync Knowledge Base

**心有灵犀 · 同步无界**  
**When Minds Connect, Data Flows**

</div>

---

## 📚 文档导航 | Documentation Navigation

### 📖 核心文档 | Core Documents
| 文档 | 中文 | English |
|------|------|---------|
| **技术白皮书** | [论文式技术阐述](Technical-Paper.md) | Thesis-style technical paper |
| **验证报告** | [代码质量验证报告](Verification-Report.md) | Code quality verification report |
| **快速入门** | [5分钟上手指南](Getting-Started.md) | 5-minute quick start guide |
| **安装指南** | [详细安装步骤](Installation-Guide.md) | Detailed installation steps |

### 🔧 功能文档 | Feature Documents
| 文档 | 说明 |
|------|------|
| [用户手册](User-Manual.md) | 完整功能使用说明 |
| [API文档](API-Documentation.md) | RESTful API 接口参考 |
| [架构设计](Architecture-Design.md) | 系统架构和技术选型 |

### 📋 规范文档 | Specification Documents
| 文档 | 说明 |
|------|------|
| [技术规范](Technical-Specifications.md) | 技术规格说明 |
| [品牌口号](Brand-Slogans.md) | 品牌标识和口号系统 |
| [贡献指南](../CONTRIBUTING.md) | 参与项目开发指南 |

---

## 🏗️ 系统架构 | System Architecture

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

### 技术栈 | Technology Stack

| 组件 | 技术 | 说明 |
|------|------|------|
| 服务端 | Node.js + Express | RESTful API + WebSocket |
| 数据库 | MySQL 8.0+ | 持久化存储 |
| 实时通信 | Socket.IO | 剪贴板实时同步 |
| 加密 | AES-256-GCM + JWT | 端到端加密 + 身份认证 |
| 桌面端 | Electron + React | 跨平台桌面应用 |
| 移动端 | Kotlin (Android) | Android 原生应用 |
| 输入法 | C++17 + RIME | 跨平台输入法方案 |
| 部署 | Docker + Nginx | 容器化部署 |

---

## 🔒 安全设计 | Security Design

```
应用层安全                    传输层安全                    数据层安全
├── 输入验证与过滤             ├── TLS 加密传输              ├── AES-256-GCM 端到端加密
├── SQL注入防护               └── JWT 身份认证              ├── bcrypt 密码哈希
├── XSS攻击防护                                             └── 访问权限控制
└── CSRF令牌保护
```

---

## 🏆 验证结果 | Verification Results

| 检查项 | 结果 | 状态 |
|--------|------|------|
| ESLint 代码质量 | 0 errors, 0 warnings | ✅ |
| 服务端安全审计 | 0 vulnerabilities | ✅ |
| 客户端安全审计 | 0 vulnerabilities | ✅ |
| 依赖许可证合规 | 全部 MIT/BSD | ✅ |

详见 [验证报告](Verification-Report.md)

---

## 🎯 品牌口号 | Brand Slogans

- **心有灵犀，同步无界** - 核心品牌理念
- **智慧连接，数据随行** - 功能价值主张
- **When Minds Connect, Data Flows** - Core brand philosophy
- **Intelligent Connections, Data On-the-Go** - Functional value proposition

---

## ⚖️ 法律声明 | Legal Notices

- **开源许可证**: MIT License
- **免责声明**: 本项目按"原样"提供，不提供任何明示或暗示的担保

---

## 📞 联系方式 | Contact

- [GitHub Issues](https://github.com/liulanci/-MindSync/issues)
- [GitHub Discussions](https://github.com/liulanci/-MindSync/discussions)