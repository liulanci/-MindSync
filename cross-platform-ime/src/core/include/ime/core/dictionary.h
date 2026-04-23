#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <cstdint>

namespace ime {
namespace core {

/**
 * @brief 词库条目
 */
struct DictEntry {
    std::string word;           // 词
    std::string code;           // 编码
    double weight = 0.0;        // 权重
    std::string pinyin;         // 拼音
    std::string category;       // 分类
    uint64_t frequency = 0;     // 频率
    std::chrono::system_clock::time_point lastUpdated; // 最后更新时间
};

/**
 * @brief 词库查询选项
 */
struct DictQueryOptions {
    size_t maxResults = 50;     // 最大结果数
    double minWeight = 0.0;     // 最小权重
    std::string category;       // 分类过滤
    bool fuzzyMatch = false;    // 模糊匹配
    bool includeUserWords = true; // 包含用户词
    size_t offset = 0;          // 偏移量
};

/**
 * @brief 词库查询结果
 */
struct DictQueryResult {
    std::vector<DictEntry> entries;
    size_t totalCount = 0;
    bool hasMore = false;
    std::chrono::milliseconds queryTime{0};
};

/**
 * @brief 词库接口
 */
class Dictionary {
public:
    virtual ~Dictionary() = default;

    /**
     * @brief 初始化词库
     */
    virtual bool initialize(const std::string& path) = 0;

    /**
     * @brief 查询词库
     */
    virtual DictQueryResult query(const std::string& code, const DictQueryOptions& options = {}) = 0;

    /**
     * @brief 精确查询
     */
    virtual std::vector<DictEntry> exactQuery(const std::string& code) = 0;

    /**
     * @brief 前缀查询
     */
    virtual std::vector<DictEntry> prefixQuery(const std::string& prefix, size_t maxResults = 50) = 0;

    /**
     * @brief 添加词库条目
     */
    virtual bool addEntry(const DictEntry& entry) = 0;

    /**
     * @brief 删除词库条目
     */
    virtual bool removeEntry(const std::string& word, const std::string& code) = 0;

    /**
     * @brief 更新词库条目
     */
    virtual bool updateEntry(const DictEntry& entry) = 0;

    /**
     * @brief 获取词库名称
     */
    virtual std::string getName() const = 0;

    /**
     * @brief 获取词库类型
     */
    virtual std::string getType() const = 0;

    /**
     * @brief 获取词库大小
     */
    virtual size_t size() const = 0;

    /**
     * @brief 保存词库
     */
    virtual bool save() = 0;

    /**
     * @brief 重新加载词库
     */
    virtual bool reload() = 0;

    /**
     * @brief 关闭词库
     */
    virtual void close() = 0;
};

/**
 * @brief 用户词库
 */
class UserDictionary : public Dictionary {
public:
    virtual ~UserDictionary() = default;

    /**
     * @brief 记录用户输入
     */
    virtual void recordInput(const std::string& word, const std::string& code) = 0;

    /**
     * @brief 获取用户词频
     */
    virtual uint64_t getFrequency(const std::string& word) const = 0;

    /**
     * @brief 获取最近使用的词
     */
    virtual std::vector<DictEntry> getRecentWords(size_t count = 20) const = 0;

    /**
     * @brief 获取最常用的词
     */
    virtual std::vector<DictEntry> getTopWords(size_t count = 20) const = 0;

    /**
     * @brief 清除学习数据
     */
    virtual void clearLearningData() = 0;
};

/**
 * @brief 词库管理器
 */
class DictionaryManager {
public:
    virtual ~DictionaryManager() = default;

    /**
     * @brief 初始化词库管理器
     */
    virtual bool initialize(const std::string& dataPath) = 0;

    /**
     * @brief 加载系统词库
     */
    virtual bool loadSystemDictionary(const std::string& name, const std::string& path) = 0;

    /**
     * @brief 加载用户词库
     */
    virtual bool loadUserDictionary(const std::string& path) = 0;

    /**
     * @brief 加载扩展词库
     */
    virtual bool loadExtensionDictionary(const std::string& name, const std::string& path) = 0;

    /**
     * @brief 卸载词库
     */
    virtual bool unloadDictionary(const std::string& name) = 0;

    /**
     * @brief 获取词库
     */
    virtual std::shared_ptr<Dictionary> getDictionary(const std::string& name) const = 0;

    /**
     * @brief 获取用户词库
     */
    virtual std::shared_ptr<UserDictionary> getUserDictionary() const = 0;

    /**
     * @brief 综合查询所有词库
     */
    virtual DictQueryResult queryAll(const std::string& code, const DictQueryOptions& options = {}) = 0;

    /**
     * @brief 获取所有词库名称
     */
    virtual std::vector<std::string> getDictionaryNames() const = 0;

    /**
     * @brief 保存所有词库
     */
    virtual bool saveAll() = 0;

    /**
     * @brief 关闭所有词库
     */
    virtual void closeAll() = 0;
};

/**
 * @brief 创建词库实例
 */
std::shared_ptr<Dictionary> createDictionary(const std::string& type);

/**
 * @brief 创建用户词库实例
 */
std::shared_ptr<UserDictionary> createUserDictionary();

/**
 * @brief 创建词库管理器
 */
std::shared_ptr<DictionaryManager> createDictionaryManager();

} // namespace core
} // namespace ime