#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "input_context.h"
#include "dictionary.h"
#include "candidate.h"
#include "config.h"

namespace ime {
namespace core {

class PluginManager;

/**
 * @brief 输入法引擎主接口
 * 
 * 负责协调输入处理流程，管理插件和词库
 */
class Engine {
public:
    virtual ~Engine() = default;

    /**
     * @brief 初始化引擎
     * @param config 配置信息
     * @return 是否初始化成功
     */
    virtual bool initialize(const Config& config) = 0;

    /**
     * @brief 处理输入事件
     * @param context 输入上下文
     * @return 处理结果
     */
    virtual ProcessResult processInput(InputContext& context) = 0;

    /**
     * @brief 获取候选词列表
     * @param context 输入上下文
     * @return 候选词列表
     */
    virtual std::vector<Candidate> getCandidates(const InputContext& context) = 0;

    /**
     * @brief 提交选中的候选词
     * @param candidate 候选词
     * @param context 输入上下文
     */
    virtual void commitCandidate(const Candidate& candidate, InputContext& context) = 0;

    /**
     * @brief 获取插件管理器
     * @return 插件管理器指针
     */
    virtual std::shared_ptr<PluginManager> getPluginManager() = 0;

    /**
     * @brief 获取词库管理器
     * @return 词库管理器指针
     */
    virtual std::shared_ptr<DictionaryManager> getDictionaryManager() = 0;

    /**
     * @brief 重新加载配置
     * @param config 新配置
     */
    virtual void reloadConfig(const Config& config) = 0;

    /**
     * @brief 关闭引擎，释放资源
     */
    virtual void shutdown() = 0;

    /**
     * @brief 获取引擎状态
     * @return 状态信息
     */
    virtual EngineStatus getStatus() const = 0;
};

/**
 * @brief 引擎状态
 */
struct EngineStatus {
    bool initialized = false;
    bool processing = false;
    size_t pluginCount = 0;
    size_t dictionaryCount = 0;
    std::string lastError;
    uint64_t totalInputs = 0;
    uint64_t totalCommits = 0;
};

/**
 * @brief 处理结果
 */
enum class ProcessResult {
    Accepted,      // 输入已接受并处理
    Rejected,      // 输入被拒绝
    NeedMoreInput, // 需要更多输入
    Completed,     // 输入完成
    Error          // 处理错误
};

/**
 * @brief 创建引擎实例
 * @return 引擎实例指针
 */
std::shared_ptr<Engine> createEngine();

} // namespace core
} // namespace ime