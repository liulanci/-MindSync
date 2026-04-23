#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <cstdint>

namespace ime {
namespace core {

/**
 * @brief 配置管理
 */
class Config {
public:
    Config() = default;
    virtual ~Config() = default;

    /**
     * @brief 从文件加载配置
     */
    virtual bool loadFromFile(const std::string& path) = 0;

    /**
     * @brief 保存配置到文件
     */
    virtual bool saveToFile(const std::string& path) const = 0;

    /**
     * @brief 获取字符串值
     */
    virtual std::string getString(const std::string& key, const std::string& defaultValue = "") const = 0;

    /**
     * @brief 获取整数值
     */
    virtual int getInt(const std::string& key, int defaultValue = 0) const = 0;

    /**
     * @brief 获取浮点数值
     */
    virtual double getDouble(const std::string& key, double defaultValue = 0.0) const = 0;

    /**
     * @brief 获取布尔值
     */
    virtual bool getBool(const std::string& key, bool defaultValue = false) const = 0;

    /**
     * @brief 设置字符串值
     */
    virtual void setString(const std::string& key, const std::string& value) = 0;

    /**
     * @brief 设置整数值
     */
    virtual void setInt(const std::string& key, int value) = 0;

    /**
     * @brief 设置浮点数值
     */
    virtual void setDouble(const std::string& key, double value) = 0;

    /**
     * @brief 设置布尔值
     */
    virtual void setBool(const std::string& key, bool value) = 0;

    /**
     * @brief 检查键是否存在
     */
    virtual bool hasKey(const std::string& key) const = 0;

    /**
     * @brief 删除键
     */
    virtual bool removeKey(const std::string& key) = 0;

    /**
     * @brief 获取所有键
     */
    virtual std::vector<std::string> getKeys() const = 0;

    /**
     * @brief 清空配置
     */
    virtual void clear() = 0;

    /**
     * @brief 合并配置
     */
    virtual void merge(const Config& other) = 0;

    /**
     * @brief 获取配置路径
     */
    virtual std::string getConfigPath() const = 0;

    /**
     * @brief 重新加载配置
     */
    virtual bool reload() = 0;
};

/**
 * @brief 引擎配置
 */
struct EngineConfig {
    std::string dataPath;              // 数据目录
    std::string userPath;              // 用户数据目录
    std::string syncPath;              // 同步目录
    std::string logPath;               // 日志目录
    std::string cachePath;             // 缓存目录
    std::string pluginPath;            // 插件目录

    std::string defaultSchema;         // 默认输入方案
    std::vector<std::string> schemas;  // 启用的输入方案列表

    size_t candidatePageSize = 5;      // 候选词每页数量
    bool showComment = true;           // 显示注释
    bool showPinyin = true;            // 显示拼音
    bool fuzzyPinyin = false;          // 模糊拼音
    bool autoSelect = false;           // 自动选择
    bool inlinePreedit = true;         // 行内预编辑

    bool enableCloudSync = false;      // 启用云同步
    std::string syncServer;            // 同步服务器地址
    std::string syncToken;             // 同步令牌
    uint32_t syncInterval = 300;       // 同步间隔(秒)

    bool enableLearning = true;        // 启用学习功能
    uint32_t maxUserWords = 50000;     // 最大用户词数
    uint32_t maxHistoryItems = 1000;   // 最大历史记录数

    bool enableSecurity = true;        // 启用安全功能
    bool encryptUserData = false;      // 加密用户数据
    std::string encryptionKey;         // 加密密钥

    std::string theme;                 // 主题名称
    std::string fontName;              // 字体名称
    uint32_t fontSize = 14;            // 字体大小

    static EngineConfig fromConfig(const Config& config);
    void toConfig(Config& config) const;
};

/**
 * @brief 创建配置实例
 */
std::shared_ptr<Config> createConfig();

/**
 * @brief 创建默认引擎配置
 */
EngineConfig createDefaultEngineConfig();

} // namespace core
} // namespace ime