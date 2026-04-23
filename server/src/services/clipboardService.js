const { v4: uuidv4 } = require('uuid');
const db = require('../config/database');
const logger = require('../config/logger');
const { encrypt, decrypt, hashContent } = require('../utils/crypto');

async function pushClipboardItem(userId, deviceId, itemData) {
  const itemId = uuidv4();
  let content = itemData.content || null;
  let isEncrypted = 0;

  if (content && itemData.content_type === 'text' && content.length > 0) {
    content = encrypt(content);
    isEncrypted = 1;
  }

  const contentHash = content ? hashContent(itemData.content || '') : null;

  await db.insert(
    `INSERT INTO clipboard_items (id, user_id, content_type, content, content_hash, file_path, file_size, source_device_id, is_encrypted, pinned)
     VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
    [
      itemId,
      userId,
      itemData.content_type,
      content,
      contentHash,
      itemData.file_path || null,
      itemData.file_size || 0,
      deviceId,
      isEncrypted,
      itemData.pinned ? 1 : 0,
    ]
  );

  await logSync(userId, deviceId, itemId, 'push', 'success');

  logger.info(`剪贴板推送: 用户=${userId}, 类型=${itemData.content_type}, 设备=${deviceId}`);
  return { id: itemId, content_type: itemData.content_type, content_hash: contentHash, created_at: new Date().toISOString() };
}

async function pullClipboardItems(userId, options = {}) {
  let sql = 'SELECT * FROM clipboard_items WHERE user_id = ?';
  const params = [userId];

  if (options.after_timestamp) {
    sql += ' AND created_at > ?';
    params.push(options.after_timestamp);
  }

  if (options.content_type) {
    sql += ' AND content_type = ?';
    params.push(options.content_type);
  }

  sql += ' ORDER BY created_at DESC LIMIT ?';
  params.push(options.limit || 50);

  const items = await db.query(sql, params);

  return items.map(item => {
    if (item.is_encrypted && item.content) {
      try {
        item.content = decrypt(item.content);
      } catch (e) {
        logger.warn(`解密失败: item=${item.id}`);
      }
    }
    return item;
  });
}

async function getClipboardItem(userId, itemId) {
  const item = await db.queryOne(
    'SELECT * FROM clipboard_items WHERE id = ? AND user_id = ?',
    [itemId, userId]
  );

  if (!item) return null;

  if (item.is_encrypted && item.content) {
    try {
      item.content = decrypt(item.content);
    } catch (e) {
      logger.warn(`解密失败: item=${item.id}`);
    }
  }

  return item;
}

async function deleteClipboardItem(userId, itemId) {
  const item = await db.queryOne(
    'SELECT * FROM clipboard_items WHERE id = ? AND user_id = ?',
    [itemId, userId]
  );

  if (!item) throw new Error('剪贴板条目未找到');

  await db.remove('DELETE FROM clipboard_items WHERE id = ?', [itemId]);
  logger.info(`剪贴板删除: item=${itemId}, 用户=${userId}`);
  return true;
}

async function togglePinItem(userId, itemId) {
  const item = await db.queryOne(
    'SELECT pinned FROM clipboard_items WHERE id = ? AND user_id = ?',
    [itemId, userId]
  );

  if (!item) throw new Error('剪贴板条目未找到');

  const newPinned = item.pinned ? 0 : 1;
  await db.update('UPDATE clipboard_items SET pinned = ? WHERE id = ?', [newPinned, itemId]);
  return { pinned: newPinned };
}

async function searchClipboardItems(userId, keyword, options = {}) {
  const limit = options.limit || 20;
  const items = await db.query(
    `SELECT * FROM clipboard_items
     WHERE user_id = ? AND content_type = 'text' AND content LIKE ?
     ORDER BY created_at DESC LIMIT ?`,
    [userId, `%${keyword}%`, limit]
  );

  return items.map(item => {
    if (item.is_encrypted && item.content) {
      try {
        item.content = decrypt(item.content);
      } catch (e) {}
    }
    return item;
  });
}

async function clearClipboardHistory(userId, beforeDate = null) {
  if (beforeDate) {
    const result = await db.remove(
      'DELETE FROM clipboard_items WHERE user_id = ? AND pinned = 0 AND created_at < ?',
      [userId, beforeDate]
    );
    return result.affectedRows;
  }

  const result = await db.remove(
    'DELETE FROM clipboard_items WHERE user_id = ? AND pinned = 0',
    [userId]
  );
  return result.affectedRows;
}

async function logSync(userId, deviceId, clipboardItemId, action, status, errorMessage = null) {
  const logId = uuidv4();
  await db.insert(
    'INSERT INTO sync_logs (id, user_id, device_id, clipboard_item_id, action, status, error_message) VALUES (?, ?, ?, ?, ?, ?, ?)',
    [logId, userId, deviceId, clipboardItemId, action, status, errorMessage]
  );
}

module.exports = {
  pushClipboardItem,
  pullClipboardItems,
  getClipboardItem,
  deleteClipboardItem,
  togglePinItem,
  searchClipboardItems,
  clearClipboardHistory,
  logSync,
};
