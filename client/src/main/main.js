const { app, BrowserWindow, Tray, Menu, ipcMain, clipboard, nativeImage, globalShortcut, Notification } = require('electron');
const path = require('path');
const ElectronStore = require('electron-store');
const { v4: uuidv4 } = require('uuid');
const SyncClient = require('../shared/syncClient');
const ClipboardService = require('./clipboardService');

const store = new ElectronStore({
  defaults: {
    serverUrl: 'http://localhost:3000',
    deviceId: uuidv4(),
    deviceName: require('os').hostname(),
    deviceType: process.platform === 'darwin' ? 'mac' : 'windows',
    autoStart: true,
    syncText: true,
    syncImages: true,
    showNotifications: true,
    maxHistorySize: 1000,
  },
});

let mainWindow = null;
let tray = null;
let syncClient = null;
let clipboardService = null;
let lastClipboardContent = null;
let lastClipboardImage = null;
let clipboardWatcherInterval = null;

function createMainWindow() {
  mainWindow = new BrowserWindow({
    width: 900,
    height: 700,
    minWidth: 700,
    minHeight: 500,
    title: '剪贴板同步器',
    icon: path.join(__dirname, '../../assets/icon.png'),
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
    show: false,
  });

  mainWindow.loadFile(path.join(__dirname, '../renderer/index.html'));

  mainWindow.once('ready-to-show', () => {
    mainWindow.show();
  });

  mainWindow.on('close', (event) => {
    if (!app.isQuitting) {
      event.preventDefault();
      mainWindow.hide();
    }
  });
}

function createTray() {
  const iconPath = path.join(__dirname, '../../assets/icon.png');
  let trayIcon;
  try {
    trayIcon = nativeImage.createFromPath(iconPath);
    if (trayIcon.isEmpty()) {
      trayIcon = nativeImage.createEmpty();
    }
  } catch {
    trayIcon = nativeImage.createEmpty();
  }

  tray = new Tray(trayIcon);
  const contextMenu = Menu.buildFromTemplate([
    { label: '显示主窗口', click: () => { mainWindow.show(); mainWindow.focus(); } },
    { type: 'separator' },
    { label: '同步状态', id: 'sync-status', enabled: false },
    { type: 'separator' },
    { label: '退出', click: () => { app.isQuitting = true; app.quit(); } },
  ]);

  tray.setContextMenu(contextMenu);
  tray.setToolTip('剪贴板同步器');
  tray.on('click', () => { mainWindow.show(); mainWindow.focus(); });
}

function initClipboardService() {
  clipboardService = new ClipboardService();

  clipboardService.on('text-changed', (text) => {
    if (syncClient && syncClient.isConnected() && store.get('syncText')) {
      lastClipboardContent = text;
      syncClient.pushClipboard({
        content_type: 'text',
        content: text,
      });
    }
  });

  clipboardService.on('image-changed', (imageData) => {
    if (syncClient && syncClient.isConnected() && store.get('syncImages')) {
      lastClipboardImage = imageData;
      syncClient.pushClipboard({
        content_type: 'image',
        content: imageData,
      });
    }
  });

  clipboardService.on('text-injected', (text) => {
    showNotification('剪贴板注入成功', '文本已注入剪贴板');
  });

  clipboardService.on('image-injected', (imageData) => {
    showNotification('剪贴板注入成功', '图片已注入剪贴板');
  });
}

function initSyncClient() {
  const serverUrl = store.get('serverUrl');
  syncClient = new SyncClient(serverUrl);

  syncClient.on('connected', () => {
    updateTrayStatus('已连接');
    sendToRenderer('sync:status', { connected: true });
    if (clipboardService) {
      clipboardService.startMonitoring();
    }
  });

  syncClient.on('disconnected', () => {
    updateTrayStatus('已断开');
    sendToRenderer('sync:status', { connected: false });
    if (clipboardService) {
      clipboardService.stopMonitoring();
    }
  });

  syncClient.on('clipboard:new', (data) => {
    if (data.content_type === 'text' && store.get('syncText')) {
      const currentContent = clipboard.readText();
      if (currentContent !== data.content) {
        if (clipboardService) {
          clipboardService.injectText(data.content);
        } else {
          clipboard.writeText(data.content);
        }
        lastClipboardContent = data.content;
        showNotification('收到新剪贴板内容', `来自 ${data.source_device_name || '其他设备'}`);
      }
    } else if (data.content_type === 'image' && store.get('syncImages')) {
      if (data.content) {
        if (clipboardService) {
          clipboardService.injectImage(data.content);
        } else {
          const image = nativeImage.createFromDataURL(data.content);
          if (!image.isEmpty()) {
            clipboard.writeImage(image);
          }
        }
        lastClipboardImage = data.content;
        showNotification('收到新剪贴板图片', `来自 ${data.source_device_name || '其他设备'}`);
      }
    }
    sendToRenderer('clipboard:new', data);
  });

  syncClient.on('clipboard:deleted', (data) => {
    sendToRenderer('clipboard:deleted', data);
  });
}

function startClipboardWatcher() {
  if (clipboardWatcherInterval) clearInterval(clipboardWatcherInterval);

  clipboardWatcherInterval = setInterval(() => {
    if (!syncClient || !syncClient.isConnected()) return;

    if (store.get('syncText')) {
      const currentText = clipboard.readText();
      if (currentText && currentText !== lastClipboardContent) {
        lastClipboardContent = currentText;
        syncClient.pushClipboard({
          content_type: 'text',
          content: currentText,
        });
      }
    }

    if (store.get('syncImages')) {
      try {
        const currentImage = clipboard.readImage();
        if (!currentImage.isEmpty()) {
          const dataUrl = currentImage.toDataURL();
          if (dataUrl !== lastClipboardImage) {
            lastClipboardImage = dataUrl;
            syncClient.pushClipboard({
              content_type: 'image',
              content: dataUrl,
            });
          }
        }
      } catch {}
    }
  }, 500);
}

function stopClipboardWatcher() {
  if (clipboardWatcherInterval) {
    clearInterval(clipboardWatcherInterval);
    clipboardWatcherInterval = null;
  }
}

function updateTrayStatus(status) {
  if (tray) {
    const contextMenu = Menu.buildFromTemplate([
      { label: '显示主窗口', click: () => { mainWindow.show(); mainWindow.focus(); } },
      { type: 'separator' },
      { label: `同步状态: ${status}`, enabled: false },
      { type: 'separator' },
      { label: '退出', click: () => { app.isQuitting = true; app.quit(); } },
    ]);
    tray.setContextMenu(contextMenu);
  }
}

function showNotification(title, body) {
  if (store.get('showNotifications')) {
    new Notification({ title, body }).show();
  }
}

function sendToRenderer(channel, data) {
  if (mainWindow && !mainWindow.isDestroyed()) {
    mainWindow.webContents.send(channel, data);
  }
}

function setupIpcHandlers() {
  ipcMain.handle('store:get', (event, key) => store.get(key));
  ipcMain.handle('store:set', (event, key, value) => { store.set(key, value); return true; });
  ipcMain.handle('store:getAll', () => store.store);

  ipcMain.handle('auth:login', async (event, { email, password }) => {
    try {
      const result = await syncClient.login(email, password);
      store.set('authToken', result.token);
      store.set('userId', result.userId);
      store.set('username', result.username);
      return { success: true, data: result };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('auth:register', async (event, data) => {
    try {
      const result = await syncClient.register(data.username, data.email, data.password);
      return { success: true, data: result };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('auth:logout', () => {
    store.delete('authToken');
    store.delete('userId');
    store.delete('username');
    if (syncClient) syncClient.disconnect();
    stopClipboardWatcher();
    sendToRenderer('auth:logout');
    return true;
  });

  ipcMain.handle('sync:connect', async () => {
    const token = store.get('authToken');
    const deviceId = store.get('deviceId');
    if (!token) return { success: false, error: '未登录' };

    try {
      await syncClient.registerDevice(
        store.get('userId'),
        store.get('deviceName'),
        store.get('deviceType'),
        deviceId
      );

      const authorized = await syncClient.checkDeviceAuthorized(store.get('userId'), deviceId);
      if (!authorized) {
        return { success: false, error: '设备未授权，请在其他已授权设备上确认授权' };
      }

      syncClient.connect(token, deviceId);
      startClipboardWatcher();
      return { success: true };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('sync:disconnect', () => {
    if (syncClient) syncClient.disconnect();
    stopClipboardWatcher();
    return true;
  });

  ipcMain.handle('clipboard:pull', async (event, options) => {
    try {
      const items = await syncClient.pullClipboard(options);
      return { success: true, data: items };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('clipboard:delete', async (event, itemId) => {
    try {
      await syncClient.deleteClipboardItem(itemId);
      return { success: true };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('clipboard:search', async (event, keyword) => {
    try {
      const items = await syncClient.searchClipboard(keyword);
      return { success: true, data: items };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('clipboard:pin', async (event, itemId) => {
    try {
      const result = await syncClient.togglePin(itemId);
      return { success: true, data: result };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('clipboard:clear', async (event, beforeDate) => {
    try {
      const result = await syncClient.clearHistory(beforeDate);
      return { success: true, data: result };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('device:list', async () => {
    try {
      const devices = await syncClient.getDevices();
      return { success: true, data: devices };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('device:authorize', async (event, deviceId) => {
    try {
      await syncClient.authorizeDevice(deviceId);
      return { success: true };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('device:revoke', async (event, deviceId) => {
    try {
      await syncClient.revokeDevice(deviceId);
      return { success: true };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('backup:create', async (event, name) => {
    try {
      const result = await syncClient.createBackup(name);
      return { success: true, data: result };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('backup:list', async () => {
    try {
      const backups = await syncClient.getBackups();
      return { success: true, data: backups };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('backup:restore', async (event, backupId) => {
    try {
      const result = await syncClient.restoreBackup(backupId);
      return { success: true, data: result };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('backup:delete', async (event, backupId) => {
    try {
      await syncClient.deleteBackup(backupId);
      return { success: true };
    } catch (err) {
      return { success: false, error: err.message };
    }
  });

  ipcMain.handle('clipboard:readText', () => clipboard.readText());
  ipcMain.handle('clipboard:writeText', (event, text) => { clipboard.writeText(text); return true; });

  ipcMain.handle('clipboard:injectText', (event, text) => {
    if (clipboardService) {
      return clipboardService.injectText(text);
    }
    return false;
  });

  ipcMain.handle('clipboard:injectImage', (event, imageData) => {
    if (clipboardService) {
      return clipboardService.injectImage(imageData);
    }
    return false;
  });

  ipcMain.handle('clipboard:enableInjection', () => {
    if (clipboardService) {
      clipboardService.enableInjection();
      return true;
    }
    return false;
  });

  ipcMain.handle('clipboard:disableInjection', () => {
    if (clipboardService) {
      clipboardService.disableInjection();
      return true;
    }
    return false;
  });

  ipcMain.handle('clipboard:getStatus', () => {
    if (clipboardService) {
      return clipboardService.getStatus();
    }
    return { isMonitoring: false, injectionEnabled: false };
  });
}

app.whenReady().then(() => {
  createMainWindow();
  createTray();
  initClipboardService();
  initSyncClient();
  setupIpcHandlers();

  const token = store.get('authToken');
  if (token) {
    const deviceId = store.get('deviceId');
    syncClient.connect(token, deviceId);
  }
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('before-quit', () => {
  app.isQuitting = true;
  stopClipboardWatcher();
  if (syncClient) syncClient.disconnect();
});

app.on('activate', () => {
  if (mainWindow) {
    mainWindow.show();
  } else {
    createMainWindow();
  }
});
