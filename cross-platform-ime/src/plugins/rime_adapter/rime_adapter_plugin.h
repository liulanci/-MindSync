#pragma once

#include "ime/core/plugin_manager.h"
#include "ime/core/dictionary.h"
#include "ime/core/input_context.h"
#include "ime/core/candidate.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ime {
namespace plugins {
namespace rime {

/**
 * @brief Rime引擎适配器插件
 * 
 * 将小狼毫(Rime)输入法引擎适配为统一的插件接口
 * 支持自定义输入方案和云词库
 */
class RimeAdapterPlugin : public ime::core::IEnginePlugin {
public:
    RimeAdapterPlugin();
    ~RimeAdapterPlugin() override;

    const ime::core::PluginInfo& getInfo() const override;
    bool initialize(const ime::core::Config& config) override;
    bool processInput(ime::core::InputContext& context) override;
    std::vector<ime::core::Candidate> filterCandidates(
        const std::vector<ime::core::Candidate>& candidates,
        const ime::core::InputContext& context) override;
    void shutdown() override;
    bool isInitialized() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;

    std::vector<ime::core::Candidate> getCandidates(
        const ime::core::InputContext& context) override;
    bool commitCandidate(const ime::core::Candidate& candidate,
        ime::core::InputContext& context) override;
    std::string getSchemaId() const override;
    std::string getSchemaName() const override;

    /**
     * @brief 设置Rime用户数据目录
     */
    bool setUserDataDir(const std::string& path);

    /**
     * @brief 设置Rime共享数据目录
     */
    bool setSharedDataDir(const std::string& path);

    /**
     * @brief 切换输入方案
     */
    bool switchSchema(const std::string& schemaId);

    /**
     * @brief 获取可用输入方案列表
     */
    std::vector<std::string> getAvailableSchemas() const;

    /**
     * @brief 触发部署
     */
    bool deploy();

    /**
     * @brief 同步用户数据
     */
    bool syncUserData();

    /**
     * @brief 获取Rime实例
     */
    void* getRimeInstance() const;

private:
    bool initializeRime();
    void cleanupRime();
    std::vector<ime::core::Candidate> convertRimeCandidates();
    bool processRimeKey(int keycode, int mask);

    ime::core::PluginInfo info_;
    void* rimeApi_ = nullptr;
    void* rimeInstance_ = nullptr;
    bool initialized_ = false;
    bool enabled_ = true;
    std::string userDataDir_;
    std::string sharedDataDir_;
    std::string currentSchemaId_;
    std::string currentSchemaName_;
};

} // namespace rime
} // namespace plugins
} // namespace ime