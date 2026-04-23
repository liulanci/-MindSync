const mysql = require('mysql2/promise');
const config = require('../config');
const logger = require('../config/logger');

const CREATE_DB_SQL = `CREATE DATABASE IF NOT EXISTS \`${config.db.database}\` CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci`;

const CREATE_USERS_TABLE = `
CREATE TABLE IF NOT EXISTS users (
  id VARCHAR(36) PRIMARY KEY,
  username VARCHAR(50) NOT NULL UNIQUE,
  email VARCHAR(100) NOT NULL UNIQUE,
  password_hash VARCHAR(255) NOT NULL,
  salt VARCHAR(64) NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  INDEX idx_username (username),
  INDEX idx_email (email)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci`;

const CREATE_DEVICES_TABLE = `
CREATE TABLE IF NOT EXISTS devices (
  id VARCHAR(36) PRIMARY KEY,
  user_id VARCHAR(36) NOT NULL,
  device_name VARCHAR(100) NOT NULL,
  device_type ENUM('windows', 'mac', 'linux', 'ios', 'android', 'harmonyos') NOT NULL,
  device_fingerprint VARCHAR(255) NOT NULL,
  authorized TINYINT(1) DEFAULT 0,
  auth_token VARCHAR(255) DEFAULT NULL,
  last_active_at TIMESTAMP NULL DEFAULT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
  UNIQUE KEY uk_user_fingerprint (user_id, device_fingerprint),
  INDEX idx_user_id (user_id),
  INDEX idx_authorized (authorized)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci`;

const CREATE_CLIPBOARD_ITEMS_TABLE = `
CREATE TABLE IF NOT EXISTS clipboard_items (
  id VARCHAR(36) PRIMARY KEY,
  user_id VARCHAR(36) NOT NULL,
  content_type ENUM('text', 'image', 'file') NOT NULL DEFAULT 'text',
  content MEDIUMTEXT,
  content_hash VARCHAR(64),
  file_path VARCHAR(500) DEFAULT NULL,
  file_size BIGINT DEFAULT 0,
  source_device_id VARCHAR(36) NOT NULL,
  is_encrypted TINYINT(1) DEFAULT 0,
  pinned TINYINT(1) DEFAULT 0,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
  FOREIGN KEY (source_device_id) REFERENCES devices(id) ON DELETE CASCADE,
  INDEX idx_user_id (user_id),
  INDEX idx_content_type (content_type),
  INDEX idx_created_at (created_at),
  INDEX idx_user_type (user_id, content_type),
  INDEX idx_user_pinned (user_id, pinned)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci`;

const CREATE_BACKUP_RECORDS_TABLE = `
CREATE TABLE IF NOT EXISTS backup_records (
  id VARCHAR(36) PRIMARY KEY,
  user_id VARCHAR(36) NOT NULL,
  backup_name VARCHAR(200) NOT NULL,
  item_count INT DEFAULT 0,
  file_path VARCHAR(500) NOT NULL,
  file_size BIGINT DEFAULT 0,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
  INDEX idx_user_id (user_id),
  INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci`;

const CREATE_SYNC_LOGS_TABLE = `
CREATE TABLE IF NOT EXISTS sync_logs (
  id VARCHAR(36) PRIMARY KEY,
  user_id VARCHAR(36) NOT NULL,
  device_id VARCHAR(36) NOT NULL,
  clipboard_item_id VARCHAR(36),
  action ENUM('push', 'pull', 'delete') NOT NULL,
  status ENUM('success', 'failed') NOT NULL DEFAULT 'success',
  error_message TEXT DEFAULT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
  FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE,
  INDEX idx_user_id (user_id),
  INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci`;

async function initDatabase() {
  let connection;
  try {
    connection = await mysql.createConnection({
      host: config.db.host,
      port: config.db.port,
      user: config.db.user,
      password: config.db.password,
    });

    logger.info('正在创建数据库...');
    await connection.query(CREATE_DB_SQL);
    await connection.changeUser({ database: config.db.database });

    logger.info('正在创建数据表...');
    await connection.query(CREATE_USERS_TABLE);
    logger.info('✓ users 表已创建');

    await connection.query(CREATE_DEVICES_TABLE);
    logger.info('✓ devices 表已创建');

    await connection.query(CREATE_CLIPBOARD_ITEMS_TABLE);
    logger.info('✓ clipboard_items 表已创建');

    await connection.query(CREATE_BACKUP_RECORDS_TABLE);
    logger.info('✓ backup_records 表已创建');

    await connection.query(CREATE_SYNC_LOGS_TABLE);
    logger.info('✓ sync_logs 表已创建');

    logger.info('数据库初始化完成！');
  } catch (err) {
    logger.error('数据库初始化失败:', err);
    throw err;
  } finally {
    if (connection) await connection.end();
  }
}

if (require.main === module) {
  initDatabase()
    .catch((err) => { throw err; });
}

module.exports = initDatabase;
