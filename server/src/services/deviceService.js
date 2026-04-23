const { v4: uuidv4 } = require('uuid');
const db = require('../config/database');
const logger = require('../config/logger');
const { generateToken } = require('../utils/crypto');

async function registerDevice(userId, deviceName, deviceType, deviceFingerprint) {
  const existing = await db.queryOne(
    'SELECT * FROM devices WHERE user_id = ? AND device_fingerprint = ?',
    [userId, deviceFingerprint]
  );

  if (existing) {
    if (existing.authorized) {
      const authToken = generateToken();
      await db.update(
        'UPDATE devices SET auth_token = ?, last_active_at = NOW() WHERE id = ?',
        [authToken, existing.id]
      );
      return { ...existing, auth_token: authToken, already_authorized: true };
    }
    return { ...existing, already_exists: true };
  }

  const deviceId = uuidv4();
  const authToken = generateToken();

  await db.insert(
    'INSERT INTO devices (id, user_id, device_name, device_type, device_fingerprint, authorized, auth_token) VALUES (?, ?, ?, ?, ?, 0, ?)',
    [deviceId, userId, deviceName, deviceType, deviceFingerprint, authToken]
  );

  logger.info(`设备注册: ${deviceName} (${deviceType}) - 用户: ${userId}`);
  return { id: deviceId, device_name: deviceName, device_type: deviceType, authorized: false, auth_token: authToken };
}

async function authorizeDevice(userId, deviceId) {
  const device = await db.queryOne(
    'SELECT * FROM devices WHERE id = ? AND user_id = ?',
    [deviceId, userId]
  );

  if (!device) throw new Error('设备未找到');

  const authToken = generateToken();
  await db.update(
    'UPDATE devices SET authorized = 1, auth_token = ?, last_active_at = NOW() WHERE id = ?',
    [authToken, deviceId]
  );

  logger.info(`设备授权: ${device.device_name} - 用户: ${userId}`);
  return { ...device, authorized: true, auth_token: authToken };
}

async function revokeDevice(userId, deviceId) {
  const device = await db.queryOne(
    'SELECT * FROM devices WHERE id = ? AND user_id = ?',
    [deviceId, userId]
  );

  if (!device) throw new Error('设备未找到');

  await db.update(
    'UPDATE devices SET authorized = 0, auth_token = NULL WHERE id = ?',
    [deviceId]
  );

  logger.info(`设备撤销授权: ${device.device_name} - 用户: ${userId}`);
  return true;
}

async function getUserDevices(userId) {
  return db.query(
    'SELECT id, device_name, device_type, device_fingerprint, authorized, last_active_at, created_at FROM devices WHERE user_id = ? ORDER BY last_active_at DESC',
    [userId]
  );
}

async function verifyDeviceAuthToken(deviceId, authToken) {
  const device = await db.queryOne(
    'SELECT * FROM devices WHERE id = ? AND auth_token = ? AND authorized = 1',
    [deviceId, authToken]
  );
  return device || null;
}

module.exports = { registerDevice, authorizeDevice, revokeDevice, getUserDevices, verifyDeviceAuthToken };
