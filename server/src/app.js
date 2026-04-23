require('dotenv').config();
const fs = require('fs');
const path = require('path');
const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const cors = require('cors');
const helmet = require('helmet');
const compression = require('compression');
const rateLimit = require('express-rate-limit');

const config = require('./config');
const logger = require('./config/logger');
const { setupSocketIO } = require('./socket/syncHandler');

const authRoutes = require('./routes/auth');
const deviceRoutes = require('./routes/devices');
const clipboardRoutes = require('./routes/clipboard');
const backupRoutes = require('./routes/backups');

const app = express();
const server = http.createServer(app);

const io = new Server(server, {
  cors: {
    origin: config.isDev ? '*' : false,
    methods: ['GET', 'POST', 'PUT', 'DELETE'],
    credentials: true,
  },
  pingTimeout: 60000,
  pingInterval: 25000,
});

app.use(helmet());
app.use(compression());
app.use(cors({
  origin: config.isDev ? '*' : false,
  credentials: true,
}));
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

const limiter = rateLimit({
  windowMs: config.rateLimit.windowMs,
  max: config.rateLimit.max,
  message: { error: '请求过于频繁，请稍后重试' },
});
app.use('/api/', limiter);

const uploadDir = config.upload.dir;
if (!fs.existsSync(uploadDir)) {
  fs.mkdirSync(uploadDir, { recursive: true });
}
app.use('/uploads', express.static(path.join(__dirname, '..', uploadDir)));

app.use('/api/auth', authRoutes);
app.use('/api/devices', deviceRoutes);
app.use('/api/clipboard', clipboardRoutes);
app.use('/api/backups', backupRoutes);

app.get('/api/health', (req, res) => {
  return res.json({ status: 'ok', version: '1.0.0', timestamp: new Date().toISOString() });
});

app.use((err, req, res, _next) => {
  if (err.name === 'MulterError') {
    if (err.code === 'LIMIT_FILE_SIZE') {
      return res.status(413).json({ error: '文件大小超出限制' });
    }
    return res.status(400).json({ error: `文件上传错误: ${err.message}` });
  }

  logger.error('未处理的错误:', err);
  return res.status(500).json({ error: '服务器内部错误' });
});

app.use((req, res) => {
  return res.status(404).json({ error: '接口不存在' });
});

setupSocketIO(io);

const PORT = config.port;

async function start() {
  try {
    const db = require('./config/database');
    await db.query('SELECT 1');
    logger.info('数据库连接成功');

    server.listen(PORT, () => {
      logger.info(`剪贴板同步服务已启动: http://localhost:${PORT}`);
      logger.info(`环境: ${config.nodeEnv}`);
      logger.info(`WebSocket: ws://localhost:${PORT}`);
    });
  } catch (err) {
    logger.error('启动失败:', err);
    logger.error('请确保MySQL服务已启动且配置正确');
    throw err;
  }
}

start();

module.exports = { app, server, io };