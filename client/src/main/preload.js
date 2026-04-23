const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  store: {
    get: (key) => ipcRenderer.invoke('store:get', key),
    set: (key, value) => ipcRenderer.invoke('store:set', key, value),
    getAll: () => ipcRenderer.invoke('store:getAll'),
  },

  auth: {
    login: (credentials) => ipcRenderer.invoke('auth:login', credentials),
    register: (data) => ipcRenderer.invoke('auth:register', data),
    logout: () => ipcRenderer.invoke('auth:logout'),
  },

  sync: {
    connect: () => ipcRenderer.invoke('sync:connect'),
    disconnect: () => ipcRenderer.invoke('sync:disconnect'),
    onStatus: (callback) => ipcRenderer.on('sync:status', (_, data) => callback(data)),
  },

  clipboard: {
    pull: (options) => ipcRenderer.invoke('clipboard:pull', options),
    delete: (itemId) => ipcRenderer.invoke('clipboard:delete', itemId),
    search: (keyword) => ipcRenderer.invoke('clipboard:search', keyword),
    pin: (itemId) => ipcRenderer.invoke('clipboard:pin', itemId),
    clear: (beforeDate) => ipcRenderer.invoke('clipboard:clear', beforeDate),
    readText: () => ipcRenderer.invoke('clipboard:readText'),
    writeText: (text) => ipcRenderer.invoke('clipboard:writeText', text),
    injectText: (text) => ipcRenderer.invoke('clipboard:injectText', text),
    injectImage: (imageData) => ipcRenderer.invoke('clipboard:injectImage', imageData),
    enableInjection: () => ipcRenderer.invoke('clipboard:enableInjection'),
    disableInjection: () => ipcRenderer.invoke('clipboard:disableInjection'),
    getStatus: () => ipcRenderer.invoke('clipboard:getStatus'),
    onNew: (callback) => ipcRenderer.on('clipboard:new', (_, data) => callback(data)),
    onDeleted: (callback) => ipcRenderer.on('clipboard:deleted', (_, data) => callback(data)),
  },

  device: {
    list: () => ipcRenderer.invoke('device:list'),
    authorize: (deviceId) => ipcRenderer.invoke('device:authorize', deviceId),
    revoke: (deviceId) => ipcRenderer.invoke('device:revoke', deviceId),
  },

  backup: {
    create: (name) => ipcRenderer.invoke('backup:create', name),
    list: () => ipcRenderer.invoke('backup:list'),
    restore: (backupId) => ipcRenderer.invoke('backup:restore', backupId),
    delete: (backupId) => ipcRenderer.invoke('backup:delete', backupId),
  },
});
