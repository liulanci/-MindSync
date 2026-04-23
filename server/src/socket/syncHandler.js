const jwt = require('jsonwebtoken');
const config = require('../config');
const db = require('../config/database');
const clipboardService = require('../services/clipboardService');
const deviceService = require('../services/deviceService');
const logger = require('../config/logger');

const userSockets = new Map();

function setupSocketIO(io) {
  io.use(async (socket, next) => {
    const token = socket.handshake.auth.token;
    const deviceId = socket.handshake.auth.deviceId;

    if (!token || !deviceId) {
      return next(new Error('认证信息不完整'));
    }

    try {
      const decoded = jwt.verify(token, config.jwt.secret);
      socket.user = decoded;

      const device = await db.queryOne(
        'SELECT * FROM devices WHERE id = ? AND user_id = ? AND authorized = 1',
        [deviceId, decoded.userId]
      );

      if (!device) {
        return next(new Error('设备未授权'));
      }

      socket.device = device;
      next();
    } catch (err) {
      if (err.name === 'TokenExpiredError') {
        return next(new Error('令牌已过期'));
      }
      next(new Error('认证失败'));
    }
  });

  io.on('connection', (socket) => {
    const userId = socket.user.userId;
    const deviceId = socket.device.id;

    logger.info(`WebSocket连接: 用户=${userId}, 设备=${socket.device.device_name}`);

    if (!userSockets.has(userId)) {
      userSockets.set(userId, new Map());
    }
    userSockets.get(userId).set(deviceId, socket);

    socket.join(`user:${userId}`);

    socket.on('clipboard:push', async (data) => {
      try {
        const result = await clipboardService.pushClipboardItem(userId, deviceId, data);

        socket.to(`user:${userId}`).emit('clipboard:new', {
          id: result.id,
          content_type: data.content_type,
          content: data.content,
          content_hash: result.content_hash,
          source_device_id: deviceId,
          source_device_name: socket.device.device_name,
          created_at: result.created_at,
        });

        socket.emit('clipboard:push:ack', { success: true, data: result });
      } catch (err) {
        logger.error(`剪贴板推送错误: ${err.message}`);
        socket.emit('clipboard:push:ack', { success: false, error: err.message });
      }
    });

    socket.on('clipboard:pull', async (data) => {
      try {
        const items = await clipboardService.pullClipboardItems(userId, data || {});
        socket.emit('clipboard:pull:result', { success: true, data: items });
      } catch (err) {
        socket.emit('clipboard:pull:result', { success: false, error: err.message });
      }
    });

    socket.on('clipboard:delete', async (itemId) => {
      try {
        await clipboardService.deleteClipboardItem(userId, itemId);
        socket.to(`user:${userId}`).emit('clipboard:deleted', { item_id: itemId });
        socket.emit('clipboard:delete:ack', { success: true, item_id: itemId });
      } catch (err) {
        socket.emit('clipboard:delete:ack', { success: false, error: err.message });
      }
    });

    socket.on('device:list', async () => {
      try {
        const devices = await deviceService.getUserDevices(userId);
        const onlineDevices = userSockets.has(userId)
          ? Array.from(userSockets.get(userId).keys())
          : [];
        socket.emit('device:list:result', {
          success: true,
          data: devices.map(d => ({
            ...d,
            online: onlineDevices.includes(d.id),
          })),
        });
      } catch (err) {
        socket.emit('device:list:result', { success: false, error: err.message });
      }
    });

    socket.on('disconnect', () => {
      logger.info(`WebSocket断开: 用户=${userId}, 设备=${socket.device.device_name}`);
      if (userSockets.has(userId)) {
        userSockets.get(userId).delete(deviceId);
        if (userSockets.get(userId).size === 0) {
          userSockets.delete(userId);
        }
      }
    });
  });

  logger.info('Socket.IO 实时同步服务已启动');
}

function getOnlineDevices(userId) {
  if (!userSockets.has(userId)) return [];
  return Array.from(userSockets.get(userId).keys());
}

module.exports = { setupSocketIO, getOnlineDevices, userSockets };
