const { v4: uuidv4 } = require('uuid');
const fs = require('fs');
const path = require('path');
const db = require('../config/database');
const config = require('../config');
const logger = require('../config/logger');
const { encrypt, decrypt } = require('../utils/crypto');

async function createBackup(userId, backupName) {
  const items = await db.query(
    'SELECT * FROM clipboard_items WHERE user_id = ?',
    [userId]
  );

  if (items.length === 0) {
    throw new Error('没有可备份的剪贴板数据');
  }

  const decryptedItems = items.map(item => {
    if (item.is_encrypted && item.content) {
      try {
        item.content = decrypt(item.content);
      } catch (e) {}
    }
    return item;
  });

  const backupData = JSON.stringify({
    version: '1.0',
    user_id: userId,
    backup_name: backupName,
    created_at: new Date().toISOString(),
    items: decryptedItems,
  });

  const backupId = uuidv4();
  const backupDir = path.join(config.upload.dir, 'backups', userId);

  if (!fs.existsSync(backupDir)) {
    fs.mkdirSync(backupDir, { recursive: true });
  }

  const fileName = `${backupId}.json`;
  const filePath = path.join(backupDir, fileName);
  const encryptedData = encrypt(backupData);
  fs.writeFileSync(filePath, encryptedData, 'utf8');

  const stats = fs.statSync(filePath);

  await db.insert(
    'INSERT INTO backup_records (id, user_id, backup_name, item_count, file_path, file_size) VALUES (?, ?, ?, ?, ?, ?)',
    [backupId, userId, backupName, decryptedItems.length, filePath, stats.size]
  );

  logger.info(`备份创建: 用户=${userId}, 名称=${backupName}, 条目数=${decryptedItems.length}`);
  return { id: backupId, backup_name: backupName, item_count: decryptedItems.length, file_size: stats.size };
}

async function restoreBackup(userId, backupId) {
  const backup = await db.queryOne(
    'SELECT * FROM backup_records WHERE id = ? AND user_id = ?',
    [backupId, userId]
  );

  if (!backup) throw new Error('备份记录未找到');

  if (!fs.existsSync(backup.file_path)) {
    throw new Error('备份文件不存在');
  }

  const encryptedData = fs.readFileSync(backup.file_path, 'utf8');
  const decryptedData = decrypt(encryptedData);
  const backupData = JSON.parse(decryptedData);

  if (!backupData.items || !Array.isArray(backupData.items)) {
    throw new Error('备份数据格式无效');
  }

  let restoredCount = 0;
  for (const item of backupData.items) {
    const existing = await db.queryOne(
      'SELECT id FROM clipboard_items WHERE id = ?',
      [item.id]
    );

    if (!existing) {
      let content = item.content;
      let isEncrypted = 0;
      if (content && item.content_type === 'text') {
        content = encrypt(content);
        isEncrypted = 1;
      }

      await db.insert(
        `INSERT INTO clipboard_items (id, user_id, content_type, content, content_hash, file_path, file_size, source_device_id, is_encrypted, pinned, created_at)
         VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
        [item.id, item.user_id, item.content_type, content, item.content_hash, item.file_path, item.file_size, item.source_device_id, isEncrypted, item.pinned || 0, item.created_at]
      );
      restoredCount++;
    }
  }

  logger.info(`备份恢复: 用户=${userId}, 备份=${backupId}, 恢复条目=${restoredCount}`);
  return { restored_count: restoredCount, total_items: backupData.items.length };
}

async function getBackups(userId) {
  return db.query(
    'SELECT id, backup_name, item_count, file_size, created_at FROM backup_records WHERE user_id = ? ORDER BY created_at DESC',
    [userId]
  );
}

async function deleteBackup(userId, backupId) {
  const backup = await db.queryOne(
    'SELECT * FROM backup_records WHERE id = ? AND user_id = ?',
    [backupId, userId]
  );

  if (!backup) throw new Error('备份记录未找到');

  if (fs.existsSync(backup.file_path)) {
    fs.unlinkSync(backup.file_path);
  }

  await db.remove('DELETE FROM backup_records WHERE id = ?', [backupId]);
  logger.info(`备份删除: 用户=${userId}, 备份=${backupId}`);
  return true;
}

module.exports = { createBackup, restoreBackup, getBackups, deleteBackup };
