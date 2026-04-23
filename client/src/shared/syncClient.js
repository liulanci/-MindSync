const { io } = require('socket.io-client');
const axios = require('axios');
const EventEmitter = require('events');

class SyncClient extends EventEmitter {
  constructor(serverUrl) {
    super();
    this.serverUrl = serverUrl;
    this.socket = null;
    this.token = null;
    this.deviceId = null;
    this.api = axios.create({ baseURL: `${serverUrl}/api` });
  }

  async register(username, email, password) {
    const response = await this.api.post('/auth/register', { username, email, password });
    return response.data.data;
  }

  async login(email, password) {
    const response = await this.api.post('/auth/login', { email, password });
    const data = response.data.data;
    this.token = data.token;
    this.api.defaults.headers.common['Authorization'] = `Bearer ${this.token}`;
    return data;
  }

  async registerDevice(userId, deviceName, deviceType, deviceFingerprint) {
    const response = await this.api.post('/devices', {
      device_name: deviceName,
      device_type: deviceType,
      device_fingerprint: deviceFingerprint,
    });
    return response.data.data;
  }

  async checkDeviceAuthorized(userId, deviceId) {
    const devices = await this.getDevices();
    const device = devices.find(d => d.id === deviceId);
    return device && device.authorized;
  }

  async getDevices() {
    const response = await this.api.get('/devices');
    return response.data.data;
  }

  async authorizeDevice(deviceId) {
    const response = await this.api.put(`/devices/${deviceId}/authorize`);
    return response.data;
  }

  async revokeDevice(deviceId) {
    const response = await this.api.delete(`/devices/${deviceId}/authorize`);
    return response.data;
  }

  connect(token, deviceId) {
    this.token = token;
    this.deviceId = deviceId;
    this.api.defaults.headers.common['Authorization'] = `Bearer ${token}`;
    this.api.defaults.headers.common['x-device-id'] = deviceId;

    if (this.socket) {
      this.socket.disconnect();
    }

    this.socket = io(this.serverUrl, {
      auth: { token, deviceId },
      transports: ['websocket', 'polling'],
      reconnection: true,
      reconnectionDelay: 1000,
      reconnectionDelayMax: 30000,
      reconnectionAttempts: Infinity,
    });

    this.socket.on('connect', () => {
      this.emit('connected');
    });

    this.socket.on('disconnect', (reason) => {
      this.emit('disconnected', reason);
    });

    this.socket.on('connect_error', (err) => {
      this.emit('error', err);
    });

    this.socket.on('clipboard:new', (data) => {
      this.emit('clipboard:new', data);
    });

    this.socket.on('clipboard:deleted', (data) => {
      this.emit('clipboard:deleted', data);
    });
  }

  disconnect() {
    if (this.socket) {
      this.socket.disconnect();
      this.socket = null;
    }
  }

  isConnected() {
    return this.socket && this.socket.connected;
  }

  pushClipboard(data) {
    if (this.socket && this.socket.connected) {
      this.socket.emit('clipboard:push', data);
    }
  }

  async pullClipboard(options = {}) {
    const params = new URLSearchParams();
    if (options.after_timestamp) params.append('after_timestamp', options.after_timestamp);
    if (options.content_type) params.append('content_type', options.content_type);
    if (options.limit) params.append('limit', options.limit);
    const response = await this.api.get(`/clipboard/pull?${params.toString()}`);
    return response.data.data;
  }

  async deleteClipboardItem(itemId) {
    const response = await this.api.delete(`/clipboard/${itemId}`);
    return response.data;
  }

  async searchClipboard(keyword) {
    const response = await this.api.get(`/clipboard/search?keyword=${encodeURIComponent(keyword)}`);
    return response.data.data;
  }

  async togglePin(itemId) {
    const response = await this.api.put(`/clipboard/${itemId}/pin`);
    return response.data.data;
  }

  async clearHistory(beforeDate) {
    const params = beforeDate ? `?before_date=${beforeDate}` : '';
    const response = await this.api.delete(`/clipboard/history${params}`);
    return response.data;
  }

  async createBackup(name) {
    const response = await this.api.post('/backups', { backup_name: name });
    return response.data.data;
  }

  async getBackups() {
    const response = await this.api.get('/backups');
    return response.data.data;
  }

  async restoreBackup(backupId) {
    const response = await this.api.post(`/backups/${backupId}/restore`);
    return response.data.data;
  }

  async deleteBackup(backupId) {
    const response = await this.api.delete(`/backups/${backupId}`);
    return response.data;
  }
}

module.exports = SyncClient;
