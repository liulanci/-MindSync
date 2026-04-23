# API文档

本文档提供灵犀/MindSync的完整API接口说明，适用于开发者集成和扩展功能。

## 📋 API概览

### 基础信息
- **API版本**: v1
- **基础URL**: `https://api.mindsync.com/v1`
- **认证方式**: JWT Bearer Token
- **数据格式**: JSON
- **编码**: UTF-8

### 响应格式

#### 成功响应
```json
{
  "success": true,
  "data": {},
  "message": "操作成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

#### 错误响应
```json
{
  "success": false,
  "error": {
    "code": "ERROR_CODE",
    "message": "错误描述",
    "details": {}
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

## 🔐 认证与授权

### 获取访问令牌

```http
POST /auth/token
Content-Type: application/json

{
  "email": "user@example.com",
  "password": "password123",
  "device_id": "device_unique_id"
}
```

**响应示例**:
```json
{
  "success": true,
  "data": {
    "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refresh_token": "def50200deabf...",
    "expires_in": 3600,
    "token_type": "Bearer"
  }
}
```

### 刷新令牌

```http
POST /auth/refresh
Authorization: Bearer {refresh_token}
```

## 📋 用户管理

### 获取用户信息

```http
GET /users/me
Authorization: Bearer {access_token}
```

**响应字段**:
- `id` - 用户ID
- `email` - 邮箱地址
- `username` - 用户名
- `created_at` - 创建时间
- `updated_at` - 更新时间

### 更新用户信息

```http
PUT /users/me
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "username": "新用户名",
  "preferences": {
    "language": "zh-CN",
    "theme": "dark"
  }
}
```

## 📱 设备管理

### 获取设备列表

```http
GET /devices
Authorization: Bearer {access_token}
```

**响应示例**:
```json
{
  "success": true,
  "data": {
    "devices": [
      {
        "id": "device_123",
        "name": "我的电脑",
        "type": "desktop",
        "platform": "windows",
        "last_seen": "2024-01-01T10:00:00Z",
        "is_online": true
      }
    ],
    "total_count": 5
  }
}
```

### 注册新设备

```http
POST /devices
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "name": "新设备名称",
  "type": "mobile",
  "platform": "android",
  "device_id": "unique_device_id"
}
```

### 更新设备信息

```http
PUT /devices/{device_id}
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "name": "更新后的设备名"
}
```

### 删除设备

```http
DELETE /devices/{device_id}
Authorization: Bearer {access_token}
```

## 📋 剪贴板管理

### 同步剪贴板内容

```http
POST /clipboard/sync
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "content": "要同步的文本内容",
  "type": "text",
  "metadata": {
    "mime_type": "text/plain",
    "size": 1024,
    "source_device": "device_123"
  }
}
```

**支持的内容类型**:
- `text` - 文本内容
- `image` - 图片内容（Base64编码）
- `file` - 文件路径
- `rich_text` - 富文本内容

### 获取剪贴板历史

```http
GET /clipboard/history
Authorization: Bearer {access_token}
```

**查询参数**:
- `page` - 页码（默认1）
- `limit` - 每页数量（默认20）
- `type` - 内容类型过滤
- `start_date` - 开始时间
- `end_date` - 结束时间

**响应示例**:
```json
{
  "success": true,
  "data": {
    "items": [
      {
        "id": "clip_123",
        "content": "同步的文本内容",
        "type": "text",
        "created_at": "2024-01-01T10:00:00Z",
        "source_device": "device_123",
        "metadata": {}
      }
    ],
    "pagination": {
      "page": 1,
      "limit": 20,
      "total_count": 150,
      "total_pages": 8
    }
  }
}
```

### 删除历史记录

```http
DELETE /clipboard/history/{item_id}
Authorization: Bearer {access_token}
```

### 批量删除历史记录

```http
POST /clipboard/history/batch-delete
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "item_ids": ["clip_123", "clip_456"]
}
```

## 🔄 实时同步

### WebSocket连接

**连接URL**: `wss://api.mindsync.com/v1/realtime`

**认证参数**:
```javascript
const ws = new WebSocket('wss://api.mindsync.com/v1/realtime', {
  headers: {
    'Authorization': 'Bearer ' + accessToken
  }
});
```

### 消息类型

#### 剪贴板更新通知
```json
{
  "type": "clipboard_update",
  "data": {
    "id": "clip_123",
    "content": "新内容",
    "type": "text",
    "source_device": "device_123",
    "timestamp": "2024-01-01T10:00:00Z"
  }
}
```

#### 设备状态更新
```json
{
  "type": "device_status",
  "data": {
    "device_id": "device_123",
    "status": "online",
    "last_seen": "2024-01-01T10:00:00Z"
  }
}
```

#### 同步确认
```json
{
  "type": "sync_ack",
  "data": {
    "clip_id": "clip_123",
    "status": "delivered",
    "target_devices": ["device_456", "device_789"]
  }
}
```

## ⚙️ 设置管理

### 获取用户设置

```http
GET /settings
Authorization: Bearer {access_token}
```

### 更新用户设置

```http
PUT /settings
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "sync": {
    "auto_sync": true,
    "sync_interval": 300,
    "max_history_size": 1000
  },
  "privacy": {
    "encryption_enabled": true,
    "auto_cleanup": true,
    "retention_days": 30
  },
  "notifications": {
    "enabled": true,
    "sound_enabled": false
  }
}
```

## 📊 统计信息

### 获取使用统计

```http
GET /stats/usage
Authorization: Bearer {access_token}
```

**查询参数**:
- `period` - 统计周期（day/week/month）
- `start_date` - 开始日期
- `end_date` - 结束日期

**响应示例**:
```json
{
  "success": true,
  "data": {
    "total_syncs": 1500,
    "devices_count": 5,
    "storage_used": "15.2 MB",
    "daily_stats": [
      {
        "date": "2024-01-01",
        "sync_count": 45,
        "data_transferred": "2.1 MB"
      }
    ]
  }
}
```

## 🔒 安全相关

### 获取加密密钥

```http
GET /security/keys
Authorization: Bearer {access_token}
```

### 轮换加密密钥

```http
POST /security/keys/rotate
Authorization: Bearer {access_token}
```

## 🐛 错误代码

### 通用错误
- `AUTH_REQUIRED` - 需要认证
- `INVALID_TOKEN` - 令牌无效
- `PERMISSION_DENIED` - 权限不足
- `RATE_LIMITED` - 请求频率限制

### 业务错误
- `DEVICE_NOT_FOUND` - 设备不存在
- `CLIPBOARD_ITEM_NOT_FOUND` - 剪贴板项不存在
- `SYNC_FAILED` - 同步失败
- `STORAGE_LIMIT_EXCEEDED` - 存储空间不足

### 系统错误
- `INTERNAL_ERROR` - 内部服务器错误
- `SERVICE_UNAVAILABLE` - 服务不可用
- `MAINTENANCE_MODE` - 维护模式

## 📚 客户端SDK

### JavaScript SDK

#### 安装
```bash
npm install mindsync-client
```

#### 使用示例
```javascript
import { MindSyncClient } from 'mindsync-client';

const client = new MindSyncClient({
  apiKey: 'your-api-key',
  server: 'https://api.mindsync.com'
});

// 同步剪贴板内容
await client.sync('Hello World!');

// 获取历史记录
const history = await client.getHistory();

// 实时同步监听
client.on('clipboard_update', (data) => {
  console.log('收到新内容:', data.content);
});
```

### Python SDK

#### 安装
```bash
pip install mindsync
```

#### 使用示例
```python
from mindsync import MindSyncClient

client = MindSyncClient(api_key='your-api-key')

# 同步内容
client.sync('Python同步测试')

# 获取设备列表
devices = client.get_devices()
```

## 🔄 Webhook集成

### Webhook配置

```http
POST /webhooks
Authorization: Bearer {access_token}
Content-Type: application/json

{
  "url": "https://your-server.com/webhook",
  "events": ["clipboard_update", "device_status"],
  "secret": "webhook_secret"
}
```

### Webhook事件

#### 剪贴板更新事件
```json
{
  "event": "clipboard_update",
  "data": {
    "id": "clip_123",
    "content": "更新的内容",
    "type": "text",
    "timestamp": "2024-01-01T10:00:00Z"
  },
  "signature": "hmac_signature"
}
```

## 📞 技术支持

### API状态检查

```http
GET /health
```

### 版本信息

```http
GET /version
```

---

## 🔗 相关资源

- [GitHub仓库](https://github.com/liulanci/-MindSync)
- [问题反馈](https://github.com/liulanci/-MindSync/issues)
- [开发者文档](Development-Setup.md)
- [更新日志](CHANGELOG.md)

---

*API版本: v1.0.0*  
*最后更新: 2024年12月*