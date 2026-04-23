const deviceService = require('../services/deviceService');

async function registerDevice(req, res) {
  try {
    const { device_name, device_type, device_fingerprint } = req.validatedBody || req.body;
    const result = await deviceService.registerDevice(
      req.user.userId,
      device_name,
      device_type,
      device_fingerprint
    );
    return res.status(201).json({ message: '设备注册成功', data: result });
  } catch (err) {
    return res.status(500).json({ error: '设备注册失败' });
  }
}

async function authorizeDevice(req, res) {
  try {
    const { deviceId } = req.params;
    const result = await deviceService.authorizeDevice(req.user.userId, deviceId);
    return res.json({ message: '设备授权成功', data: result });
  } catch (err) {
    if (err.message.includes('未找到')) {
      return res.status(404).json({ error: err.message });
    }
    return res.status(500).json({ error: '设备授权失败' });
  }
}

async function revokeDevice(req, res) {
  try {
    const { deviceId } = req.params;
    await deviceService.revokeDevice(req.user.userId, deviceId);
    return res.json({ message: '设备授权已撤销' });
  } catch (err) {
    if (err.message.includes('未找到')) {
      return res.status(404).json({ error: err.message });
    }
    return res.status(500).json({ error: '撤销授权失败' });
  }
}

async function getDevices(req, res) {
  try {
    const devices = await deviceService.getUserDevices(req.user.userId);
    return res.json({ data: devices });
  } catch (err) {
    return res.status(500).json({ error: '获取设备列表失败' });
  }
}

module.exports = { registerDevice, authorizeDevice, revokeDevice, getDevices };