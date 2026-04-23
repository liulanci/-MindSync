const api = window.electronAPI;

let currentView = 'clipboard';
let clipboardItems = [];
let isConnected = false;

function $(selector) { return document.querySelector(selector); }
function $$(selector) { return document.querySelectorAll(selector); }

function showToast(message, type = 'info') {
  const container = $('#toast-container');
  const toast = document.createElement('div');
  toast.className = `toast ${type}`;
  toast.textContent = message;
  container.appendChild(toast);
  setTimeout(() => { toast.remove(); }, 3000);
}

function showModal(title, bodyHtml) {
  $('#modal-title').textContent = title;
  $('#modal-body').innerHTML = bodyHtml;
  $('#modal-overlay').classList.remove('hidden');
}

function hideModal() {
  $('#modal-overlay').classList.add('hidden');
}

function formatTime(dateStr) {
  const date = new Date(dateStr);
  const now = new Date();
  const diff = now - date;
  if (diff < 60000) return '刚刚';
  if (diff < 3600000) return `${Math.floor(diff / 60000)}分钟前`;
  if (diff < 86400000) return `${Math.floor(diff / 3600000)}小时前`;
  return date.toLocaleDateString('zh-CN', { month: 'short', day: 'numeric', hour: '2-digit', minute: '2-digit' });
}

function getDeviceIcon(type) {
  const icons = { windows: '🖥', mac: '💻', linux: '🐧', ios: '📱', android: '🤖', harmonyos: '🔷' };
  return icons[type] || '📟';
}

function truncateText(text, maxLen = 200) {
  if (!text) return '';
  return text.length > maxLen ? text.substring(0, maxLen) + '...' : text;
}

async function init() {
  setupAuthTabs();
  setupForms();
  setupNavigation();
  setupClipboardActions();
  setupDeviceActions();
  setupBackupActions();
  setupSettings();
  setupSyncListeners();

  const token = await api.store.get('authToken');
  if (token) {
    showMainView();
    await loadUserInfo();
    await connectSync();
    await loadClipboardItems();
  }
}

function setupAuthTabs() {
  $$('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      $$('.tab-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      const tab = btn.dataset.tab;
      if (tab === 'login') {
        $('#login-form').classList.remove('hidden');
        $('#register-form').classList.add('hidden');
      } else {
        $('#login-form').classList.add('hidden');
        $('#register-form').classList.remove('hidden');
      }
    });
  });
}

function setupForms() {
  $('#login-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = $('#login-email').value;
    const password = $('#login-password').value;
    const result = await api.auth.login({ email, password });
    if (result.success) {
      showToast('登录成功', 'success');
      showMainView();
      await loadUserInfo();
      await connectSync();
      await loadClipboardItems();
    } else {
      $('#login-error').textContent = result.error;
    }
  });

  $('#register-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    const username = $('#reg-username').value;
    const email = $('#reg-email').value;
    const password = $('#reg-password').value;
    const confirm = $('#reg-confirm').value;

    if (password !== confirm) {
      $('#register-error').textContent = '两次密码输入不一致';
      return;
    }

    const result = await api.auth.register({ username, email, password });
    if (result.success) {
      showToast('注册成功，请登录', 'success');
      $$('.tab-btn')[0].click();
      $('#login-email').value = email;
    } else {
      $('#register-error').textContent = result.error;
    }
  });
}

function setupNavigation() {
  $$('.nav-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      $$('.nav-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      const view = btn.dataset.view;
      switchView(view);
    });
  });

  $('#btn-logout').addEventListener('click', async () => {
    await api.auth.logout();
    showLoginView();
    showToast('已退出登录', 'info');
  });
}

function switchView(view) {
  currentView = view;
  $$('.panel').forEach(p => p.classList.add('hidden'));
  $(`#${view}-panel`).classList.remove('hidden');

  if (view === 'clipboard') loadClipboardItems();
  else if (view === 'devices') loadDevices();
  else if (view === 'backups') loadBackups();
  else if (view === 'settings') loadSettings();
}

function setupClipboardActions() {
  $('#refresh-btn').addEventListener('click', loadClipboardItems);
  $('#search-btn').addEventListener('click', searchClipboard);
  $('#search-input').addEventListener('keypress', (e) => {
    if (e.key === 'Enter') searchClipboard();
  });
  $('#filter-type').addEventListener('change', loadClipboardItems);
  $('#clear-btn').addEventListener('click', async () => {
    if (confirm('确定要清除所有未固定的剪贴板记录吗？')) {
      const result = await api.clipboard.clear();
      if (result.success) {
        showToast(`已清除 ${result.data.deleted_count} 条记录`, 'success');
        loadClipboardItems();
      }
    }
  });
}

function setupDeviceActions() {
  $('#refresh-devices-btn').addEventListener('click', loadDevices);
}

function setupBackupActions() {
  $('#create-backup-btn').addEventListener('click', async () => {
    const name = `备份 ${new Date().toLocaleString('zh-CN')}`;
    const result = await api.backup.create(name);
    if (result.success) {
      showToast('备份创建成功', 'success');
      loadBackups();
    } else {
      showToast(result.error || '备份失败', 'error');
    }
  });
}

function setupSettings() {
  $('#save-settings-btn').addEventListener('click', async () => {
    const serverUrl = $('#setting-server-url').value;
    const syncText = $('#setting-sync-text').checked;
    const syncImages = $('#setting-sync-images').checked;
    const notifications = $('#setting-notifications').checked;
    const autostart = $('#setting-autostart').checked;
    const deviceName = $('#setting-device-name').value;

    await api.store.set('serverUrl', serverUrl);
    await api.store.set('syncText', syncText);
    await api.store.set('syncImages', syncImages);
    await api.store.set('showNotifications', notifications);
    await api.store.set('autoStart', autostart);
    await api.store.set('deviceName', deviceName);

    showToast('设置已保存', 'success');
  });
}

function setupSyncListeners() {
  api.sync.onStatus((data) => {
    isConnected = data.connected;
    updateSyncStatus();
  });

  api.clipboard.onNew((data) => {
    if (currentView === 'clipboard') {
      loadClipboardItems();
    }
  });

  api.clipboard.onDeleted((data) => {
    if (currentView === 'clipboard') {
      clipboardItems = clipboardItems.filter(i => i.id !== data.item_id);
      renderClipboardItems();
    }
  });
}

function showLoginView() {
  $('#login-view').classList.remove('hidden');
  $('#main-view').classList.add('hidden');
}

function showMainView() {
  $('#login-view').classList.add('hidden');
  $('#main-view').classList.remove('hidden');
}

async function loadUserInfo() {
  const username = await api.store.get('username');
  $('#username-display').textContent = username || '用户';
  $('#user-avatar').textContent = (username || 'U')[0].toUpperCase();
}

async function connectSync() {
  const result = await api.sync.connect();
  if (result.success) {
    isConnected = true;
    updateSyncStatus();
  } else {
    showToast(result.error || '连接失败', 'error');
  }
}

function updateSyncStatus() {
  const statusEl = $('#sync-status');
  const dot = statusEl.querySelector('.status-dot');
  if (isConnected) {
    dot.className = 'status-dot connected';
    statusEl.innerHTML = '<span class="status-dot connected"></span>已连接';
  } else {
    dot.className = 'status-dot disconnected';
    statusEl.innerHTML = '<span class="status-dot disconnected"></span>未连接';
  }
}

async function loadClipboardItems() {
  const filterType = $('#filter-type').value;
  const options = {};
  if (filterType) options.content_type = filterType;

  const result = await api.clipboard.pull(options);
  if (result.success) {
    clipboardItems = result.data || [];
    renderClipboardItems();
  }
}

function renderClipboardItems() {
  const list = $('#clipboard-list');
  if (clipboardItems.length === 0) {
    list.innerHTML = `
      <div class="empty-state">
        <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="#ccc" stroke-width="1.5">
          <rect x="8" y="2" width="8" height="4" rx="1"/>
          <path d="M16 4h2a2 2 0 012 2v14a2 2 0 01-2 2H6a2 2 0 01-2-2V6a2 2 0 012-2h2"/>
        </svg>
        <p>暂无剪贴板记录</p>
        <p class="hint">复制内容后将自动同步到此处</p>
      </div>`;
    return;
  }

  list.innerHTML = clipboardItems.map(item => {
    const isImage = item.content_type === 'image';
    const contentHtml = isImage
      ? (item.file_path ? `<img src="file://${item.file_path}" alt="图片">` : '<span style="color:var(--text-muted)">[图片]</span>')
      : `<div>${escapeHtml(truncateText(item.content, 300))}</div>`;

    return `
      <div class="clipboard-item ${item.pinned ? 'pinned' : ''}" data-id="${item.id}">
        <div class="clipboard-item-header">
          <span class="clipboard-item-type ${isImage ? 'image' : ''}">${isImage ? '图片' : '文本'}</span>
          <div class="clipboard-item-actions">
            ${!isImage ? `<button class="btn-icon" onclick="copyItem('${item.id}')" title="复制"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="9" y="9" width="13" height="13" rx="2"/><path d="M5 15H4a2 2 0 01-2-2V4a2 2 0 012-2h9a2 2 0 012 2v1"/></svg></button>` : ''}
            <button class="btn-icon" onclick="togglePin('${item.id}')" title="${item.pinned ? '取消固定' : '固定'}"><svg width="14" height="14" viewBox="0 0 24 24" fill="${item.pinned ? 'currentColor' : 'none'}" stroke="currentColor" stroke-width="2"><path d="M12 2l3.09 6.26L22 9.27l-5 4.87 1.18 6.88L12 17.77l-6.18 3.25L7 14.14 2 9.27l6.91-1.01L12 2z"/></svg></button>
            <button class="btn-icon btn-danger" onclick="deleteItem('${item.id}')" title="删除"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 01-2 2H7a2 2 0 01-2-2V6m3 0V4a2 2 0 012-2h4a2 2 0 012 2v2"/></svg></button>
          </div>
        </div>
        <div class="clipboard-item-content">${contentHtml}</div>
        <div class="clipboard-item-meta">
          <span>${formatTime(item.created_at)}</span>
          ${item.source_device_id ? `<span>来自设备</span>` : ''}
        </div>
      </div>`;
  }).join('');
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}

window.copyItem = async (itemId) => {
  const item = clipboardItems.find(i => i.id === itemId);
  if (item && item.content_type === 'text') {
    await api.clipboard.writeText(item.content);
    showToast('已复制到剪贴板', 'success');
  }
};

window.togglePin = async (itemId) => {
  const result = await api.clipboard.pin(itemId);
  if (result.success) {
    showToast(result.data.pinned ? '已固定' : '已取消固定', 'info');
    loadClipboardItems();
  }
};

window.deleteItem = async (itemId) => {
  if (confirm('确定要删除这条记录吗？')) {
    const result = await api.clipboard.delete(itemId);
    if (result.success) {
      showToast('已删除', 'success');
      loadClipboardItems();
    }
  }
};

async function searchClipboard() {
  const keyword = $('#search-input').value.trim();
  if (!keyword) {
    loadClipboardItems();
    return;
  }
  const result = await api.clipboard.search(keyword);
  if (result.success) {
    clipboardItems = result.data || [];
    renderClipboardItems();
  }
}

async function loadDevices() {
  const result = await api.device.list();
  const list = $('#devices-list');
  if (result.success && result.data && result.data.length > 0) {
    list.innerHTML = result.data.map(device => `
      <div class="device-card">
        <div class="device-info">
          <div class="device-icon">${getDeviceIcon(device.device_type)}</div>
          <div class="device-details">
            <h4>${escapeHtml(device.device_name)}</h4>
            <p>${device.device_type} · ${device.last_active_at ? formatTime(device.last_active_at) : '从未在线'}</p>
          </div>
        </div>
        <div class="device-status">
          <span class="badge ${device.authorized ? 'authorized' : 'unauthorized'}">
            ${device.authorized ? '已授权' : '未授权'}
          </span>
          ${device.authorized
            ? `<button class="btn-sm btn-delete" onclick="revokeDevice('${device.id}')">撤销</button>`
            : `<button class="btn-sm btn-restore" onclick="authorizeDevice('${device.id}')">授权</button>`
          }
        </div>
      </div>
    `).join('');
  } else {
    list.innerHTML = '<div class="empty-state"><p>暂无设备</p></div>';
  }
}

window.authorizeDevice = async (deviceId) => {
  const result = await api.device.authorize(deviceId);
  if (result.success) {
    showToast('设备已授权', 'success');
    loadDevices();
  } else {
    showToast(result.error || '授权失败', 'error');
  }
};

window.revokeDevice = async (deviceId) => {
  if (confirm('确定要撤销此设备的授权吗？')) {
    const result = await api.device.revoke(deviceId);
    if (result.success) {
      showToast('授权已撤销', 'success');
      loadDevices();
    }
  }
};

async function loadBackups() {
  const result = await api.backup.list();
  const list = $('#backups-list');
  if (result.success && result.data && result.data.length > 0) {
    list.innerHTML = result.data.map(backup => `
      <div class="backup-card">
        <div class="backup-info">
          <h4>${escapeHtml(backup.backup_name)}</h4>
          <p>${backup.item_count} 条记录 · ${(backup.file_size / 1024).toFixed(1)} KB · ${formatTime(backup.created_at)}</p>
        </div>
        <div class="backup-actions">
          <button class="btn-sm btn-restore" onclick="restoreBackup('${backup.id}')">恢复</button>
          <button class="btn-sm btn-delete" onclick="deleteBackup('${backup.id}')">删除</button>
        </div>
      </div>
    `).join('');
  } else {
    list.innerHTML = '<div class="empty-state"><p>暂无备份记录</p></div>';
  }
}

window.restoreBackup = async (backupId) => {
  if (confirm('确定要恢复此备份吗？这将合并备份数据到当前记录中。')) {
    const result = await api.backup.restore(backupId);
    if (result.success) {
      showToast(`恢复成功，共 ${result.data.restored_count} 条记录`, 'success');
    } else {
      showToast(result.error || '恢复失败', 'error');
    }
  }
};

window.deleteBackup = async (backupId) => {
  if (confirm('确定要删除此备份吗？')) {
    const result = await api.backup.delete(backupId);
    if (result.success) {
      showToast('备份已删除', 'success');
      loadBackups();
    }
  }
};

async function loadSettings() {
  const settings = await api.store.getAll();
  $('#setting-server-url').value = settings.serverUrl || 'http://localhost:3000';
  $('#setting-sync-text').checked = settings.syncText !== false;
  $('#setting-sync-images').checked = settings.syncImages !== false;
  $('#setting-notifications').checked = settings.showNotifications !== false;
  $('#setting-autostart').checked = settings.autoStart !== false;
  $('#setting-device-name').value = settings.deviceName || '';
  $('#setting-device-id').value = settings.deviceId || '';
}

$('#modal-close').addEventListener('click', hideModal);
$('#modal-overlay').addEventListener('click', (e) => {
  if (e.target === $('#modal-overlay')) hideModal();
});

init();
