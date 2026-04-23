package com.mindsync.android.service

import android.app.Service
import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import android.os.IBinder
import androidx.core.app.NotificationCompat
import com.mindsync.android.R
import com.mindsync.android.data.repository.ClipboardRepository
import com.mindsync.android.data.repository.SyncRepository
import com.mindsync.android.util.NotificationHelper
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import javax.inject.Inject

@AndroidEntryPoint
class ClipboardSyncService : Service() {
    
    @Inject
    lateinit var clipboardRepository: ClipboardRepository
    
    @Inject
    lateinit var syncRepository: SyncRepository
    
    private val serviceScope = CoroutineScope(Dispatchers.IO)
    private var clipboardJob: Job? = null
    private var lastClipboardContent: String? = null
    
    override fun onCreate() {
        super.onCreate()
        startForegroundService()
        startClipboardMonitoring()
    }
    
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        return START_STICKY
    }
    
    override fun onBind(intent: Intent?): IBinder? = null
    
    private fun startForegroundService() {
        val notification = NotificationHelper.createSyncNotification(this)
        startForeground(NOTIFICATION_ID, notification)
    }
    
    private fun startClipboardMonitoring() {
        clipboardJob = serviceScope.launch {
            val clipboardManager = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
            
            // 监听剪贴板变化
            clipboardManager.addPrimaryClipChangedListener {
                val clip = clipboardManager.primaryClip
                if (clip != null && clip.itemCount > 0) {
                    val text = clip.getItemAt(0).text?.toString()
                    if (text != null && text != lastClipboardContent) {
                        lastClipboardContent = text
                        handleClipboardChange(text)
                    }
                }
            }
        }
    }
    
    private fun handleClipboardChange(content: String) {
        serviceScope.launch {
            try {
                // 保存到本地数据库
                val clipboardItem = clipboardRepository.saveClipboardItem(content)
                
                // 同步到服务器
                syncRepository.syncClipboardItem(clipboardItem)
                
                // 更新通知
                updateNotification("已同步: ${content.take(50)}...")
                
            } catch (e: Exception) {
                // 处理同步失败
                updateNotification("同步失败: ${e.message}")
            }
        }
    }
    
    private fun updateNotification(message: String) {
        val notification = NotificationHelper.createSyncNotification(this, message)
        val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as android.app.NotificationManager
        notificationManager.notify(NOTIFICATION_ID, notification)
    }
    
    override fun onDestroy() {
        super.onDestroy()
        clipboardJob?.cancel()
        stopForeground(true)
    }
    
    companion object {
        private const val NOTIFICATION_ID = 1001
        
        fun startService(context: Context) {
            val intent = Intent(context, ClipboardSyncService::class.java)
            context.startForegroundService(intent)
        }
        
        fun stopService(context: Context) {
            val intent = Intent(context, ClipboardSyncService::class.java)
            context.stopService(intent)
        }
    }
}