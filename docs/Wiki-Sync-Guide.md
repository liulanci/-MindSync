# GitHub Wiki 同步指南 | GitHub Wiki Sync Guide

## 问题描述 | Problem Description

GitCode 上的 Wiki 页面 (`https://gitcode.com/lanci888/MindSync/wiki`) 目前为空，这是因为该 Wiki 是从 GitHub 自动同步的，而 GitHub 仓库的 Wiki 功能尚未启用或内容为空。

## 解决方案 | Solution

### 步骤1：启用 GitHub 仓库的 Wiki 功能
1. 访问 GitHub 仓库：`https://github.com/liulanci/-MindSync`
2. 点击仓库顶部的 **Settings** 选项卡
3. 在左侧菜单中找到 **Features** 部分
4. 找到 **Wikis** 选项，确保其处于启用状态（勾选复选框）
5. 点击 **Save** 保存设置

### 步骤2：将本地 Wiki 文档推送到 GitHub Wiki
启用 Wiki 功能后，GitHub 会自动创建一个专用的 Wiki 仓库。您可以使用以下命令将本地 `docs/` 目录中的文档推送到 Wiki 仓库：

```bash
# 克隆 Wiki 仓库（如果尚未克隆）
git clone https://github.com/liulanci/-MindSync.wiki.git

# 进入 Wiki 仓库目录
cd -MindSync.wiki

# 将本地 docs 目录中的所有 .md 文件复制到 Wiki 仓库
# 注意：确保不覆盖现有的 .git 目录
cp -r ../docs/*.md .

# 添加、提交并推送更改
git add .
git commit -m "导入完整的 Wiki 文档，包括论文式技术白皮书"
git push origin master
```

**注意**：如果 Wiki 仓库使用 `main` 分支而非 `master` 分支，请将上述命令中的 `master` 替换为 `main`。

### 步骤3：验证 GitCode 同步
GitCode 通常会在几分钟内自动同步 GitHub Wiki 的更改。同步后，您可以在 `https://gitcode.com/lanci888/MindSync/wiki` 查看完整的 Wiki 内容。

## 文档结构 | Document Structure

本地 `docs/` 目录包含以下 Wiki 文档：

| 文件 | 说明 |
|------|------|
| `Home.md` | Wiki 首页，提供导航和项目概述 |
| `Technical-Paper.md` | 论文式技术白皮书，详细阐述系统设计与实现 |
| `Getting-Started.md` | 快速入门指南 |
| `Installation-Guide.md` | 详细安装步骤 |
| `User-Manual.md` | 用户手册 |
| `API-Documentation.md` | API 接口文档 |
| `Architecture-Design.md` | 系统架构设计 |
| `Technical-Specifications.md` | 技术规范 |
| `Brand-Slogans.md` | 品牌口号系统 |
| `Verification-Report.md` | 代码质量验证报告 |

## 故障排除 | Troubleshooting

1. **Wiki 仓库不存在**：确保已按照步骤1启用了 Wiki 功能。
2. **推送权限不足**：确保您对 GitHub 仓库有写入权限。
3. **GitCode 未同步**：GitCode 同步可能有延迟，请等待几分钟后刷新页面。
4. **文件冲突**：如果 Wiki 仓库中已有文件，请先备份再覆盖。

## 自动化脚本（可选）| Automation Script (Optional)

您可以使用以下 PowerShell 脚本自动执行上述步骤（需要 Git 和 GitHub CLI）：

```powershell
# 启用 Wiki 功能（需要 GitHub CLI 和认证）
gh api -X PATCH /repos/liulanci/-MindSync -f has_wiki=true

# 克隆 Wiki 仓库
git clone https://github.com/liulanci/-MindSync.wiki.git
cd -MindSync.wiki

# 复制文档
Copy-Item ..\docs\*.md .

# 提交并推送
git add .
git commit -m "导入 Wiki 文档"
git push origin master
```

**注意**：使用 GitHub CLI 需要先通过 `gh auth login` 登录。

## 结论 | Conclusion

按照上述步骤操作后，GitHub Wiki 将包含完整的项目文档，GitCode 会自动同步这些内容，用户即可通过 `https://gitcode.com/lanci888/MindSync/wiki` 访问到“像论文一样的 Wiki”文档。

如有任何问题，请参考 GitHub 官方文档或联系仓库维护者。