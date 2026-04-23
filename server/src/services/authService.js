const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const { v4: uuidv4 } = require('uuid');
const db = require('../config/database');
const config = require('../config');
const logger = require('../config/logger');
const { generateToken } = require('../utils/crypto');

const SALT_ROUNDS = 12;

async function register(username, email, password) {
  const existingUser = await db.queryOne(
    'SELECT id FROM users WHERE username = ? OR email = ?',
    [username, email]
  );

  if (existingUser) {
    throw new Error('用户名或邮箱已存在');
  }

  const salt = await bcrypt.genSalt(SALT_ROUNDS);
  const passwordHash = await bcrypt.hash(password, salt);
  const userId = uuidv4();

  await db.insert(
    'INSERT INTO users (id, username, email, password_hash, salt) VALUES (?, ?, ?, ?, ?)',
    [userId, username, email, passwordHash, salt]
  );

  const token = jwt.sign(
    { userId, username, email },
    config.jwt.secret,
    { expiresIn: config.jwt.expiresIn }
  );

  logger.info(`用户注册成功: ${username}`);
  return { userId, username, email, token };
}

async function login(email, password) {
  const user = await db.queryOne(
    'SELECT * FROM users WHERE email = ?',
    [email]
  );

  if (!user) {
    throw new Error('邮箱或密码错误');
  }

  const isPasswordValid = await bcrypt.compare(password, user.password_hash);
  if (!isPasswordValid) {
    throw new Error('邮箱或密码错误');
  }

  const token = jwt.sign(
    { userId: user.id, username: user.username, email: user.email },
    config.jwt.secret,
    { expiresIn: config.jwt.expiresIn }
  );

  logger.info(`用户登录成功: ${user.username}`);
  return { userId: user.id, username: user.username, email: user.email, token };
}

async function getUserProfile(userId) {
  const user = await db.queryOne(
    'SELECT id, username, email, created_at, updated_at FROM users WHERE id = ?',
    [userId]
  );
  if (!user) throw new Error('用户不存在');
  return user;
}

async function changePassword(userId, oldPassword, newPassword) {
  const user = await db.queryOne('SELECT * FROM users WHERE id = ?', [userId]);
  if (!user) throw new Error('用户不存在');

  const isPasswordValid = await bcrypt.compare(oldPassword, user.password_hash);
  if (!isPasswordValid) throw new Error('原密码错误');

  const salt = await bcrypt.genSalt(SALT_ROUNDS);
  const passwordHash = await bcrypt.hash(newPassword, salt);

  await db.update(
    'UPDATE users SET password_hash = ?, salt = ?, updated_at = NOW() WHERE id = ?',
    [passwordHash, salt, userId]
  );

  logger.info(`用户修改密码: ${user.username}`);
  return true;
}

module.exports = { register, login, getUserProfile, changePassword };
