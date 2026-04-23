#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <cstdint>

namespace ime {
namespace core {

/**
 * @brief 云词库同步状态
 */
enum class SyncStatus {
    Idle,           // 空闲
    Syncing,        // 同步中
    Success,        // 同步成功
    Error,          // 同步错误
    Conflict,       // 存在冲突
    NetworkError,   // 网络错误
    AuthError       // 认证错误
};

/**
 * @brief 同步结果
 */
struct SyncResult {
    SyncStatus status = SyncStatus::Idle;
    std::string message;
    uint32_t uploadedItems = 0;
    uint32_t downloadedItems = 0;
    uint32_t conflictItems = 0;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief 云词库服务接口
 */
class CloudDictionaryService {
public:
    virtual ~CloudDictionaryService() = default;

    /**
     * @brief 初始化云服务
     */
    virtual bool initialize(const std::string& serverUrl, const std::string& token) = 0;

    /**
     * @brief 同步用户词库
     */
    virtual SyncResult syncUserDictionary() = 0;

    /**
     * @brief 上传用户词库
     */
    virtual SyncResult uploadUserDictionary(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief 下载用户词库
     */
    virtual SyncResult downloadUserDictionary(std::vector<uint8_t>& data) = 0;

    /**
     * @brief 获取云词库更新
     */
    virtual SyncResult getCloudUpdates(const std::string& sinceVersion,
        std::vector<uint8_t>& data) = 0;

    /**
     * @brief 查询云端候选词
     */
    virtual std::vector<Candidate> queryCloudCandidates(const std::string& code,
        size_t maxResults = 10) = 0;

    /**
     * @brief 获取同步状态
     */
    virtual SyncStatus getSyncStatus() const = 0;

    /**
     * @brief 设置同步回调
     */
    virtual void setSyncCallback(std::function<void(const SyncResult&)> callback) = 0;

    /**
     * @brief 取消同步
     */
    virtual bool cancelSync() = 0;

    /**
     * @brief 关闭云服务
     */
    virtual void shutdown() = 0;
};

/**
 * @brief 创建云词库服务
 */
std::shared_ptr<CloudDictionaryService> createCloudDictionaryService();

} // namespace core
} // namespace ime