const backupService = require('../services/backupService');

async function createBackup(req, res) {
  try {
    const { backup_name } = req.validatedBody || req.body;
    const result = await backupService.createBackup(req.user.userId, backup_name);
    return res.status(201).json({ message: '备份创建成功', data: result });
  } catch (err) {
    if (err.message.includes('没有可备份')) {
      return res.status(400).json({ error: err.message });
    }
    return res.status(500).json({ error: '备份创建失败' });
  }
}

async function restoreBackup(req, res) {
  try {
    const result = await backupService.restoreBackup(req.user.userId, req.params.backupId);
    return res.json({ message: '备份恢复成功', data: result });
  } catch (err) {
    if (err.message.includes('未找到') || err.message.includes('不存在') || err.message.includes('无效')) {
      return res.status(400).json({ error: err.message });
    }
    return res.status(500).json({ error: '备份恢复失败' });
  }
}

async function getBackups(req, res) {
  try {
    const backups = await backupService.getBackups(req.user.userId);
    return res.json({ data: backups });
  } catch (err) {
    return res.status(500).json({ error: '获取备份列表失败' });
  }
}

async function deleteBackup(req, res) {
  try {
    await backupService.deleteBackup(req.user.userId, req.params.backupId);
    return res.json({ message: '备份删除成功' });
  } catch (err) {
    if (err.message.includes('未找到')) {
      return res.status(404).json({ error: err.message });
    }
    return res.status(500).json({ error: '备份删除失败' });
  }
}

module.exports = { createBackup, restoreBackup, getBackups, deleteBackup };