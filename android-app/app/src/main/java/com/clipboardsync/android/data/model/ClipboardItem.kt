package com.mindsync.android.data.model

import androidx.room.Entity
import androidx.room.PrimaryKey
import java.util.Date
import java.util.UUID

@Entity(tableName = "clipboard_items")
data class ClipboardItem(
    @PrimaryKey
    val id: String = UUID.randomUUID().toString(),
    val content: String,
    val contentType: ContentType = ContentType.TEXT,
    val timestamp: Date = Date(),
    val deviceId: String,
    val isSynced: Boolean = false,
    val isPinned: Boolean = false,
    val syncTimestamp: Date? = null
) {
    enum class ContentType {
        TEXT, IMAGE, FILE
    }
    
    companion object {
        fun createTextItem(content: String, deviceId: String): ClipboardItem {
            return ClipboardItem(
                content = content,
                contentType = ContentType.TEXT,
                deviceId = deviceId
            )
        }
    }
}