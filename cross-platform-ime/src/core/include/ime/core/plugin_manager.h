#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include "input_context.h"
#include "candidate.h"
#include "config.h"

namespace ime {
namespace core {

/**
 * @brief 插件类型
 */
enum class PluginType {
    Engine,         // 输入法引擎插件
    Dictionary,     // 词库插件
    Filter,         // 过滤器插件
    Processor,      // 处理器插件
    Segmentor,      // 分词器插件
    Translator,     // 翻译器插件
    UI,             // 界面插件
    Utility         // 工具插件
};

/**
 * @brief 插件信息
 */
struct PluginInfo {
    std::string id;             // 插件ID
    std::string name;           // 插件名称
    std::string version;        // 版本号
    std::string description;    // 描述
    std::string author;         // 作者
    std::string homepage;       // 主页
    std::string license;        // 许可证
    PluginType type;            // 类型
    std::vector<std::string> dependencies; // 依赖
    std::unordered_map<std::string, std::string> metadata; // 元数据
};

/**
 * @brief 插件接口
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;

    /**
     * @brief 获取插件信息
     */
    virtual const PluginInfo& getInfo() const = 0;

    /**
     * @brief 初始化插件
     */
    virtual bool initialize(const Config& config) = 0;

    /**
     * @brief 处理输入
     */
    virtual bool processInput(InputContext& context) = 0;

    /**
     * @brief 过滤候选词
     */
    virtual std::vector<Candidate> filterCandidates(
        const std::vector<Candidate>& candidates,
        const InputContext& context) = 0;

    /**
     * @brief 关闭插件
     */
    virtual void shutdown() = 0;

    /**
     * @brief 是否已初始化
     */
    virtual bool isInitialized() const = 0;

    /**
     * @brief 是否已启用
     */
    virtual bool isEnabled() const = 0;

    /**
     * @brief 设置启用状态
     */
    virtual void setEnabled(bool enabled) = 0;
};

/**
 * @brief 输入法引擎插件接口
 */
class IEnginePlugin : public IPlugin {
public:
    virtual std::vector<Candidate> getCandidates(const InputContext& context) = 0;
    virtual bool commitCandidate(const Candidate& candidate, InputContext& context) = 0;
    virtual std::string getSchemaId() const = 0;
    virtual std::string getSchemaName() const = 0;
};

/**
 * @brief 词库插件接口
 */
class IDictionaryPlugin : public IPlugin {
public:
    virtual DictQueryResult query(const std::string& code, const DictQueryOptions& options) = 0;
    virtual std::vector<DictEntry> getAllEntries() = 0;
    virtual size_t getEntryCount() const = 0;
};

/**
 * @brief 插件管理器
 */
class PluginManager {
public:
    virtual ~PluginManager() = default;

    /**
     * @brief 初始化插件管理器
     */
    virtual bool initialize(const std::string& pluginPath, const Config& config) = 0;

    /**
     * @brief 加载插件
     */
    virtual bool loadPlugin(const std::string& path) = 0;

    /**
     * @brief 卸载插件
     */
    virtual bool unloadPlugin(const std::string& pluginId) = 0;

    /**
     * @brief 获取插件
     */
    virtual std::shared_ptr<IPlugin> getPlugin(const std::string& pluginId) const = 0;

    /**
     * @brief 获取所有插件
     */
    virtual std::vector<std::shared_ptr<IPlugin>> getAllPlugins() const = 0;

    /**
     * @brief 获取指定类型的插件
     */
    virtual std::vector<std::shared_ptr<IPlugin>> getPluginsByType(PluginType type) const = 0;

    /**
     * @brief 启用插件
     */
    virtual bool enablePlugin(const std::string& pluginId) = 0;

    /**
     * @brief 禁用插件
     */
    virtual bool disablePlugin(const std::string& pluginId) = 0;

    /**
     * @brief 处理输入（按优先级调用所有处理器插件）
     */
    virtual bool processInput(InputContext& context) = 0;

    /**
     * @brief 过滤候选词（按优先级调用所有过滤器插件）
     */
    virtual std::vector<Candidate> filterCandidates(
        const std::vector<Candidate>& candidates,
        const InputContext& context) = 0;

    /**
     * @brief 关闭所有插件
     */
    virtual void shutdownAll() = 0;

    /**
     * @brief 获取已加载插件数量
     */
    virtual size_t getPluginCount() const = 0;

    /**
     * @brief 注册插件工厂
     */
    virtual void registerFactory(const std::string& type,
        std::function<std::shared_ptr<IPlugin>()> factory) = 0;
};

/**
 * @brief 创建插件管理器
 */
std::shared_ptr<PluginManager> createPluginManager();

} // namespace core
} // namespace ime