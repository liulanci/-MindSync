package com.mindsync.android.data.database

import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import android.content.Context
import com.mindsync.android.data.model.ClipboardItem
import com.mindsync.android.data.dao.ClipboardItemDao

@Database(
    entities = [ClipboardItem::class],
    version = 1,
    exportSchema = false
)
abstract class AppDatabase : RoomDatabase() {
    
    abstract fun clipboardItemDao(): ClipboardItemDao
    
    companion object {
        @Volatile
        private var INSTANCE: AppDatabase? = null
        
        fun getInstance(context: Context): AppDatabase {
            return INSTANCE ?: synchronized(this) {
                val instance = Room.databaseBuilder(
                    context.applicationContext,
                    AppDatabase::class.java,
                    "clipboard_sync_database"
                ).build()
                INSTANCE = instance
                instance
            }
        }
    }
}