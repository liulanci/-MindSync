const jwt = require('jsonwebtoken');
const config = require('../config');
const db = require('../config/database');
const logger = require('../config/logger');

function authMiddleware(req, res, next) {
  const authHeader = req.headers.authorization;
  if (!authHeader || !authHeader.startsWith('Bearer ')) {
    return res.status(401).json({ error: '未提供认证令牌' });
  }

  const token = authHeader.split(' ')[1];
  try {
    const decoded = jwt.verify(token, config.jwt.secret);
    req.user = decoded;
    return next();
  } catch (err) {
    if (err.name === 'TokenExpiredError') {
      return res.status(401).json({ error: '令牌已过期，请重新登录' });
    }
    return res.status(401).json({ error: '无效的认证令牌' });
  }
}

async function deviceAuthMiddleware(req, res, next) {
  const deviceId = req.headers['x-device-id'];
  if (!deviceId) {
    return res.status(400).json({ error: '缺少设备标识' });
  }

  try {
    const device = await db.queryOne(
      'SELECT * FROM devices WHERE id = ? AND user_id = ?',
      [deviceId, req.user.userId]
    );

    if (!device) {
      return res.status(404).json({ error: '设备未找到' });
    }

    if (!device.authorized) {
      return res.status(403).json({ error: '设备未授权，请先完成设备授权' });
    }

    // eslint-disable-next-line require-atomic-updates
    req.device = device;

    await db.update(
      'UPDATE devices SET last_active_at = NOW() WHERE id = ?',
      [deviceId]
    );

    return next();
  } catch (err) {
    logger.error('设备认证中间件错误:', err);
    return res.status(500).json({ error: '服务器内部错误' });
  }
}

module.exports = { authMiddleware, deviceAuthMiddleware };