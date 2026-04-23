const clipboardService = require('../services/clipboardService');
const multer = require('multer');
const path = require('path');
const config = require('../config');
const { v4: uuidv4 } = require('uuid');

const storage = multer.diskStorage({
  destination: (req, file, cb) => {
    const uploadDir = path.join(config.upload.dir, 'clipboard');
    if (!require('fs').existsSync(uploadDir)) {
      require('fs').mkdirSync(uploadDir, { recursive: true });
    }
    cb(null, uploadDir);
  },
  filename: (req, file, cb) => {
    const ext = path.extname(file.originalname);
    cb(null, `${uuidv4()}${ext}`);
  },
});

const upload = multer({
  storage,
  limits: { fileSize: config.upload.maxSize },
  fileFilter: (req, file, cb) => {
    const allowedTypes = /jpeg|jpg|png|gif|bmp|webp|svg\+xml/;
    const ext = path.extname(file.originalname).toLowerCase();
    const mimetype = file.mimetype;
    if (allowedTypes.test(mimetype) || allowedTypes.test(ext)) {
      cb(null, true);
    } else {
      cb(new Error('不支持的文件类型'));
    }
  },
});

async function pushItem(req, res) {
  try {
    const itemData = req.validatedBody;
    const deviceId = req.headers['x-device-id'];
    const result = await clipboardService.pushClipboardItem(req.user.userId, deviceId, itemData);
    return res.status(201).json({ message: '推送成功', data: result });
  } catch (err) {
    return res.status(500).json({ error: '推送失败' });
  }
}

async function pushFile(req, res) {
  try {
    if (!req.file) {
      return res.status(400).json({ error: '请上传文件' });
    }

    const deviceId = req.headers['x-device-id'];
    const itemData = {
      content_type: 'image',
      content: null,
      file_path: req.file.path,
      file_size: req.file.size,
    };

    const result = await clipboardService.pushClipboardItem(req.user.userId, deviceId, itemData);
    return res.status(201).json({ message: '文件推送成功', data: result });
  } catch (err) {
    return res.status(500).json({ error: '文件推送失败' });
  }
}

async function pullItems(req, res) {
  try {
    const options = {
      after_timestamp: req.query.after_timestamp,
      content_type: req.query.content_type,
      limit: parseInt(req.query.limit, 10) || 50,
    };
    const items = await clipboardService.pullClipboardItems(req.user.userId, options);
    return res.json({ data: items });
  } catch (err) {
    return res.status(500).json({ error: '拉取失败' });
  }
}

async function getItem(req, res) {
  try {
    const item = await clipboardService.getClipboardItem(req.user.userId, req.params.itemId);
    if (!item) {
      return res.status(404).json({ error: '条目未找到' });
    }
    return res.json({ data: item });
  } catch (err) {
    return res.status(500).json({ error: '获取条目失败' });
  }
}

async function deleteItem(req, res) {
  try {
    await clipboardService.deleteClipboardItem(req.user.userId, req.params.itemId);
    return res.json({ message: '删除成功' });
  } catch (err) {
    if (err.message.includes('未找到')) {
      return res.status(404).json({ error: err.message });
    }
    return res.status(500).json({ error: '删除失败' });
  }
}

async function togglePin(req, res) {
  try {
    const result = await clipboardService.togglePinItem(req.user.userId, req.params.itemId);
    return res.json({ data: result });
  } catch (err) {
    if (err.message.includes('未找到')) {
      return res.status(404).json({ error: err.message });
    }
    return res.status(500).json({ error: '操作失败' });
  }
}

async function searchItems(req, res) {
  try {
    const { keyword } = req.query;
    if (!keyword) {
      return res.status(400).json({ error: '请提供搜索关键词' });
    }
    const items = await clipboardService.searchClipboardItems(req.user.userId, keyword, {
      limit: parseInt(req.query.limit, 10) || 20,
    });
    return res.json({ data: items });
  } catch (err) {
    return res.status(500).json({ error: '搜索失败' });
  }
}

async function clearHistory(req, res) {
  try {
    const { before_date } = req.query;
    const deletedCount = await clipboardService.clearClipboardHistory(req.user.userId, before_date);
    return res.json({ message: `已清除 ${deletedCount} 条记录`, deleted_count: deletedCount });
  } catch (err) {
    return res.status(500).json({ error: '清除历史失败' });
  }
}

module.exports = { pushItem, pushFile, pullItems, getItem, deleteItem, togglePin, searchItems, clearHistory, upload };