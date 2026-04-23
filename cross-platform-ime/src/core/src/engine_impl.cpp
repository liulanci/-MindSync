#include "ime/core/engine.h"
#include "ime/core/plugin_manager.h"
#include "ime/core/dictionary.h"
#include "ime/core/platform_adapter.h"
#include "ime/core/cloud_service.h"
#include <algorithm>
#include <chrono>
#include <mutex>

namespace ime {
namespace core {

class EngineImpl : public Engine {
public:
    EngineImpl()
        : status_{}
        , running_{false} {}

    ~EngineImpl() override {
        shutdown();
    }

    bool initialize(const Config& config) override {
        std::lock_guard<std::mutex> lock(mutex_);

        if (status_.initialized) {
            return true;
        }

        engineConfig_ = EngineConfig::fromConfig(config);

        dictManager_ = createDictionaryManager();
        if (!dictManager_->initialize(engineConfig_.dataPath)) {
            status_.lastError = "Failed to initialize dictionary manager";
            return false;
        }

        pluginManager_ = createPluginManager();
        if (!pluginManager_->initialize(engineConfig_.pluginPath, config)) {
            status_.lastError = "Failed to initialize plugin manager";
            return false;
        }

        if (engineConfig_.enableCloudSync && !engineConfig_.syncServer.empty()) {
            cloudService_ = createCloudDictionaryService();
            if (!cloudService_->initialize(engineConfig_.syncServer, engineConfig_.syncToken)) {
                status_.lastError = "Failed to initialize cloud service";
            }
        }

        status_.initialized = true;
        status_.pluginCount = pluginManager_->getPluginCount();
        status_.dictionaryCount = dictManager_->getDictionaryNames().size();
        running_ = true;

        return true;
    }

    ProcessResult processInput(InputContext& context) override {
        if (!status_.initialized) {
            return ProcessResult::Error;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        status_.processing = true;
        status_.totalInputs++;

        bool handled = pluginManager_->processInput(context);

        status_.processing = false;

        if (!handled) {
            return ProcessResult::Rejected;
        }

        if (context.getInputText().empty()) {
            return ProcessResult::NeedMoreInput;
        }

        return ProcessResult::Accepted;
    }

    std::vector<Candidate> getCandidates(const InputContext& context) override {
        if (!status_.initialized) {
            return {};
        }

        std::vector<Candidate> candidates;

        auto userDict = dictManager_->getUserDictionary();
        if (userDict) {
            auto userResult = userDict->query(context.getInputText());
            for (const auto& entry : userResult.entries) {
                Candidate c;
                c.text = entry.word;
                c.code = entry.code;
                c.weight = entry.weight;
                c.isUserWord = true;
                c.useCount = entry.frequency;
                candidates.push_back(c);
            }
        }

        auto sysResult = dictManager_->queryAll(context.getInputText());
        for (const auto& entry : sysResult.entries) {
            Candidate c;
            c.text = entry.word;
            c.code = entry.code;
            c.weight = entry.weight;
            c.source = entry.category;
            candidates.push_back(c);
        }

        if (cloudService_ && engineConfig_.enableCloudSync) {
            auto cloudCandidates = cloudService_->queryCloudCandidates(context.getInputText());
            candidates.insert(candidates.end(), cloudCandidates.begin(), cloudCandidates.end());
        }

        candidates = pluginManager_->filterCandidates(candidates, context);

        std::sort(candidates.begin(), candidates.end());

        return candidates;
    }

    void commitCandidate(const Candidate& candidate, InputContext& context) override {
        if (!status_.initialized) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        status_.totalCommits++;

        auto userDict = dictManager_->getUserDictionary();
        if (userDict) {
            userDict->recordInput(candidate.text, candidate.code);
        }
    }

    std::shared_ptr<PluginManager> getPluginManager() override {
        return pluginManager_;
    }

    std::shared_ptr<DictionaryManager> getDictionaryManager() override {
        return dictManager_;
    }

    void reloadConfig(const Config& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        engineConfig_ = EngineConfig::fromConfig(config);
    }

    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!status_.initialized) {
            return;
        }

        if (cloudService_) {
            cloudService_->shutdown();
            cloudService_.reset();
        }

        if (pluginManager_) {
            pluginManager_->shutdownAll();
            pluginManager_.reset();
        }

        if (dictManager_) {
            dictManager_->saveAll();
            dictManager_->closeAll();
            dictManager_.reset();
        }

        status_.initialized = false;
        running_ = false;
    }

    EngineStatus getStatus() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return status_;
    }

private:
    EngineStatus status_;
    EngineConfig engineConfig_;
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<DictionaryManager> dictManager_;
    std::shared_ptr<CloudDictionaryService> cloudService_;
    std::shared_ptr<PlatformAdapter> platformAdapter_;
    std::mutex mutex_;
    bool running_;
};

std::shared_ptr<Engine> createEngine() {
    return std::make_shared<EngineImpl>();
}

EngineConfig EngineConfig::fromConfig(const Config& config) {
    EngineConfig ec;
    ec.dataPath = config.getString("engine.dataPath", "./data");
    ec.userPath = config.getString("engine.userPath", "./user");
    ec.syncPath = config.getString("engine.syncPath", "./sync");
    ec.logPath = config.getString("engine.logPath", "./logs");
    ec.cachePath = config.getString("engine.cachePath", "./cache");
    ec.pluginPath = config.getString("engine.pluginPath", "./plugins");
    ec.defaultSchema = config.getString("engine.defaultSchema", "luna_pinyin_simp");
    ec.candidatePageSize = static_cast<size_t>(config.getInt("engine.candidatePageSize", 5));
    ec.showComment = config.getBool("engine.showComment", true);
    ec.showPinyin = config.getBool("engine.showPinyin", true);
    ec.fuzzyPinyin = config.getBool("engine.fuzzyPinyin", false);
    ec.autoSelect = config.getBool("engine.autoSelect", false);
    ec.inlinePreedit = config.getBool("engine.inlinePreedit", true);
    ec.enableCloudSync = config.getBool("engine.enableCloudSync", false);
    ec.syncServer = config.getString("engine.syncServer", "");
    ec.syncToken = config.getString("engine.syncToken", "");
    ec.syncInterval = static_cast<uint32_t>(config.getInt("engine.syncInterval", 300));
    ec.enableLearning = config.getBool("engine.enableLearning", true);
    ec.maxUserWords = static_cast<uint32_t>(config.getInt("engine.maxUserWords", 50000));
    ec.maxHistoryItems = static_cast<uint32_t>(config.getInt("engine.maxHistoryItems", 1000));
    ec.enableSecurity = config.getBool("engine.enableSecurity", true);
    ec.encryptUserData = config.getBool("engine.encryptUserData", false);
    ec.theme = config.getString("engine.theme", "default");
    ec.fontName = config.getString("engine.fontName", "Microsoft YaHei");
    ec.fontSize = static_cast<uint32_t>(config.getInt("engine.fontSize", 14));
    return ec;
}

void EngineConfig::toConfig(Config& config) const {
    config.setString("engine.dataPath", dataPath);
    config.setString("engine.userPath", userPath);
    config.setString("engine.syncPath", syncPath);
    config.setString("engine.logPath", logPath);
    config.setString("engine.cachePath", cachePath);
    config.setString("engine.pluginPath", pluginPath);
    config.setString("engine.defaultSchema", defaultSchema);
    config.setInt("engine.candidatePageSize", static_cast<int>(candidatePageSize));
    config.setBool("engine.showComment", showComment);
    config.setBool("engine.showPinyin", showPinyin);
    config.setBool("engine.fuzzyPinyin", fuzzyPinyin);
    config.setBool("engine.autoSelect", autoSelect);
    config.setBool("engine.inlinePreedit", inlinePreedit);
    config.setBool("engine.enableCloudSync", enableCloudSync);
    config.setString("engine.syncServer", syncServer);
    config.setString("engine.syncToken", syncToken);
    config.setInt("engine.syncInterval", static_cast<int>(syncInterval));
    config.setBool("engine.enableLearning", enableLearning);
    config.setInt("engine.maxUserWords", static_cast<int>(maxUserWords));
    config.setInt("engine.maxHistoryItems", static_cast<int>(maxHistoryItems));
    config.setBool("engine.enableSecurity", enableSecurity);
    config.setBool("engine.encryptUserData", encryptUserData);
    config.setString("engine.theme", theme);
    config.setString("engine.fontName", fontName);
    config.setInt("engine.fontSize", static_cast<int>(fontSize));
}

EngineConfig createDefaultEngineConfig() {
    EngineConfig config;
    config.dataPath = "./data";
    config.userPath = "./user";
    config.syncPath = "./sync";
    config.logPath = "./logs";
    config.cachePath = "./cache";
    config.pluginPath = "./plugins";
    config.defaultSchema = "luna_pinyin_simp";
    config.candidatePageSize = 5;
    config.showComment = true;
    config.showPinyin = true;
    config.fuzzyPinyin = false;
    config.autoSelect = false;
    config.inlinePreedit = true;
    config.enableCloudSync = false;
    config.enableLearning = true;
    config.maxUserWords = 50000;
    config.maxHistoryItems = 1000;
    config.enableSecurity = true;
    config.encryptUserData = false;
    config.theme = "default";
    config.fontName = "Microsoft YaHei";
    config.fontSize = 14;
    return config;
}

} // namespace core
} // namespace ime