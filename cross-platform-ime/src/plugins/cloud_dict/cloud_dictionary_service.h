#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <chrono>

namespace ime {
namespace plugins {
namespace cloud_dict {

/**
 * @brief 云词库条目
 */
struct CloudDictEntry {
    std::string word;
    std::string code;
    double weight;
    std::string pinyin;
    std::string category;
    uint64_t version;
    std::chrono::system_clock::time_point updatedAt;
};

/**
 * @brief 云词库同步配置
 */
struct CloudDictSyncConfig {
    std::string serverUrl;
    std::string apiToken;
    std::string userId;
    std::string deviceId;
    uint32_t syncIntervalSeconds = 300;
    uint32_t retryIntervalSeconds = 30;
    uint32_t maxRetries = 3;
    bool enableAutoSync = true;
    bool enableIncrementalSync = true;
    bool enableCompression = true;
    bool enableEncryption = true;
    std::string encryptionKey;
    size_t maxLocalCacheSize = 100000;
    size_t maxBatchSize = 1000;
};

/**
 * @brief 云词库同步结果
 */
struct CloudDictSyncResult {
    bool success = false;
    std::string errorMessage;
    uint32_t itemsUploaded = 0;
    uint32_t itemsDownloaded = 0;
    uint32_t itemsUpdated = 0;
    uint32_t itemsDeleted = 0;
    uint32_t conflicts = 0;
    std::chrono::milliseconds duration{0};
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief 云词库查询结果
 */
struct CloudDictQueryResult {
    std::vector<CloudDictEntry> entries;
    uint32_t totalCount = 0;
    bool fromCache = false;
    std::chrono::milliseconds queryTime{0};
};

/**
 * @brief 小狼毫定制镜像云词库服务
 * 
 * 提供与Rime输入法兼容的云词库服务，支持：
 * - 实时词库同步与更新
 * - 增量同步减少数据传输
 * - 本地缓存加速查询
 * - 端到端加密保护隐私
 * - 多设备词库一致性
 * - 离线模式支持
 */
class CloudDictionaryService {
public:
    CloudDictionaryService();
    ~CloudDictionaryService();

    /**
     * @brief 初始化云词库服务
     */
    bool initialize(const CloudDictSyncConfig& config);

    /**
     * @brief 关闭云词库服务
     */
    void shutdown();

    /**
     * @brief 执行完整同步
     */
    CloudDictSyncResult fullSync();

    /**
     * @brief 执行增量同步
     */
    CloudDictSyncResult incrementalSync();

    /**
     * @brief 上传用户词库变更
     */
    CloudDictSyncResult uploadChanges(const std::vector<CloudDictEntry>& entries);

    /**
     * @brief 下载云端词库更新
     */
    CloudDictSyncResult downloadUpdates(uint64_t sinceVersion);

    /**
     * @brief 查询云端候选词
     */
    CloudDictQueryResult query(const std::string& code, size_t maxResults = 10);

    /**
     * @brief 模糊查询
     */
    CloudDictQueryResult fuzzyQuery(const std::string& code, size_t maxResults = 10);

    /**
     * @brief 前缀查询
     */
    CloudDictQueryResult prefixQuery(const std::string& prefix, size_t maxResults = 10);

    /**
     * @brief 获取同步状态
     */
    bool isSyncing() const;

    /**
     * @brief 获取上次同步时间
     */
    std::chrono::system_clock::time_point getLastSyncTime() const;

    /**
     * @brief 获取本地版本号
     */
    uint64_t getLocalVersion() const;

    /**
     * @brief 获取云端版本号
     */
    uint64_t getRemoteVersion() const;

    /**
     * @brief 设置同步回调
     */
    void setSyncCallback(std::function<void(const CloudDictSyncResult&)> callback);

    /**
     * @brief 设置查询回调
     */
    void setQueryCallback(std::function<void(const CloudDictQueryResult&)> callback);

    /**
     * @brief 清除本地缓存
     */
    bool clearLocalCache();

    /**
     * @brief 获取缓存统计
     */
    struct CacheStats {
        size_t entryCount = 0;
        size_t totalSize = 0;
        double hitRate = 0.0;
    };
    CacheStats getCacheStats() const;

    /**
     * @brief 导出为Rime词库格式
     */
    bool exportToRimeFormat(const std::string& outputPath);

    /**
     * @brief 从Rime词库格式导入
     */
    bool importFromRimeFormat(const std::string& inputPath);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cloud_dict
} // namespace plugins
} // namespace ime