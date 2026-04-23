# 贡献指南

感谢您对灵犀/MindSync项目的关注！我们欢迎各种形式的贡献。

## 🚀 快速开始

### 开发环境搭建

1. **克隆仓库**
```bash
git clone https://github.com/liulanci/-MindSync.git
cd -MindSync
```

2. **安装依赖**
```bash
npm install
```

3. **启动开发服务器**
```bash
npm run dev
```

## 📝 贡献方式

### 报告 Bug

- 使用 [GitHub Issues](https://github.com/liulanci/-MindSync/issues) 报告问题
- 提供详细的错误描述、复现步骤和环境信息
- 如果可能，提供截图或日志文件

### 功能建议

- 在 Issues 中提出新功能想法
- 描述功能的使用场景和预期效果
- 讨论技术实现方案

### 代码贡献

1. **Fork 仓库**
2. **创建功能分支**
```bash
git checkout -b feature/amazing-feature
```
3. **提交更改**
```bash
git commit -m "Add amazing feature"
```
4. **推送到分支**
```bash
git push origin feature/amazing-feature
```
5. **创建 Pull Request**

## 🔧 开发规范

### 代码风格

- 使用 ESLint 和 Prettier 保持代码一致性
- 遵循项目现有的代码风格
- 添加适当的注释和文档

### 提交信息规范

使用约定式提交格式：
```
feat: 添加新功能
fix: 修复bug
docs: 文档更新
style: 代码格式调整
refactor: 代码重构
test: 测试相关
chore: 构建过程或辅助工具变动
```

### 测试要求

- 新功能需要包含单元测试
- 确保所有测试通过
- 更新相关文档

## 🏷️ 分支管理

- `main` - 主分支，稳定版本
- `develop` - 开发分支
- `feature/*` - 功能分支
- `bugfix/*` - Bug修复分支
- `release/*` - 发布分支

## 📋 Pull Request 流程

1. 确保代码通过所有测试
2. 更新相关文档
3. 添加适当的测试用例
4. 遵循代码审查流程
5. 等待维护者合并

## 🐛 Bug 报告模板

```markdown
## 问题描述
[清晰描述遇到的问题]

## 复现步骤
1. [第一步]
2. [第二步]
3. [...]

## 预期行为
[期望的正常行为]

## 实际行为
[实际观察到的行为]

## 环境信息
- 操作系统: [例如 Windows 11]
- 版本: [例如 v1.0.0]
- 其他相关信息
```

## 💡 功能建议模板

```markdown
## 功能描述
[详细描述建议的功能]

## 使用场景
[在什么情况下会使用这个功能]

## 预期效果
[功能实现后的预期效果]

## 替代方案
[是否有其他实现方式]
```

## 🔒 安全报告

如果您发现安全漏洞，请通过以下方式报告：
- 发送邮件至 security@mindsync.com
- 不要公开披露漏洞细节

## 🙏 致谢

感谢所有为项目做出贡献的开发者！您的每一份贡献都让项目变得更好。

---

*最后更新: 2024年12月*