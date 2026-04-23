const authService = require('../services/authService');

async function register(req, res) {
  try {
    const { username, email, password } = req.validatedBody || req.body;
    const result = await authService.register(username, email, password);
    return res.status(201).json({ message: '注册成功', data: result });
  } catch (err) {
    if (err.message.includes('已存在')) {
      return res.status(409).json({ error: err.message });
    }
    return res.status(500).json({ error: '注册失败，请稍后重试' });
  }
}

async function login(req, res) {
  try {
    const { email, password } = req.validatedBody || req.body;
    const result = await authService.login(email, password);
    return res.json({ message: '登录成功', data: result });
  } catch (err) {
    if (err.message.includes('错误')) {
      return res.status(401).json({ error: err.message });
    }
    return res.status(500).json({ error: '登录失败，请稍后重试' });
  }
}

async function getProfile(req, res) {
  try {
    const profile = await authService.getUserProfile(req.user.userId);
    return res.json({ data: profile });
  } catch (err) {
    return res.status(404).json({ error: err.message });
  }
}

async function changePassword(req, res) {
  try {
    const { old_password, new_password } = req.body;
    if (!old_password || !new_password) {
      return res.status(400).json({ error: '请提供原密码和新密码' });
    }
    await authService.changePassword(req.user.userId, old_password, new_password);
    return res.json({ message: '密码修改成功' });
  } catch (err) {
    if (err.message.includes('错误')) {
      return res.status(400).json({ error: err.message });
    }
    return res.status(500).json({ error: '密码修改失败' });
  }
}

module.exports = { register, login, getProfile, changePassword };