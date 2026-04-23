#include "ime/core/plugin_manager.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <mutex>

namespace ime {
namespace core {

class PluginManagerImpl : public PluginManager {
public:
    PluginManagerImpl() = default;
    ~PluginManagerImpl() override { shutdownAll(); }

    bool initialize(const std::string& pluginPath, const Config& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        pluginPath_ = pluginPath;
        config_ = std::make_unique<Config>(config);
        return true;
    }

    bool loadPlugin(const std::string& path) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto factoryIt = factories_.find(path);
        if (factoryIt != factories_.end()) {
            auto plugin = factoryIt->second();
            if (plugin && plugin->initialize(*config_)) {
                plugins_[plugin->getInfo().id] = plugin;
                return true;
            }
        }

        return false;
    }

    bool unloadPlugin(const std::string& pluginId) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = plugins_.find(pluginId);
        if (it != plugins_.end()) {
            it->second->shutdown();
            plugins_.erase(it);
            return true;
        }
        return false;
    }

    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginId) const override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = plugins_.find(pluginId);
        if (it != plugins_.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<IPlugin>> getAllPlugins() const override {
        std::lock_guard<std::mutex> lock(mutex_);

        std::vector<std::shared_ptr<IPlugin>> result;
        result.reserve(plugins_.size());
        for (const auto& [id, plugin] : plugins_) {
            result.push_back(plugin);
        }
        return result;
    }

    std::vector<std::shared_ptr<IPlugin>> getPluginsByType(PluginType type) const override {
        std::lock_guard<std::mutex> lock(mutex_);

        std::vector<std::shared_ptr<IPlugin>> result;
        for (const auto& [id, plugin] : plugins_) {
            if (plugin->getInfo().type == type) {
                result.push_back(plugin);
            }
        }
        return result;
    }

    bool enablePlugin(const std::string& pluginId) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = plugins_.find(pluginId);
        if (it != plugins_.end()) {
            it->second->setEnabled(true);
            return true;
        }
        return false;
    }

    bool disablePlugin(const std::string& pluginId) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = plugins_.find(pluginId);
        if (it != plugins_.end()) {
            it->second->setEnabled(false);
            return true;
        }
        return false;
    }

    bool processInput(InputContext& context) override {
        std::lock_guard<std::mutex> lock(mutex_);

        bool handled = false;
        for (auto& [id, plugin] : plugins_) {
            if (plugin->isEnabled() && plugin->processInput(context)) {
                handled = true;
                break;
            }
        }
        return handled;
    }

    std::vector<Candidate> filterCandidates(
        const std::vector<Candidate>& candidates,
        const InputContext& context) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto filtered = candidates;
        for (auto& [id, plugin] : plugins_) {
            if (plugin->isEnabled()) {
                filtered = plugin->filterCandidates(filtered, context);
            }
        }
        return filtered;
    }

    void shutdownAll() override {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& [id, plugin] : plugins_) {
            plugin->shutdown();
        }
        plugins_.clear();
    }

    size_t getPluginCount() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return plugins_.size();
    }

    void registerFactory(const std::string& type,
        std::function<std::shared_ptr<IPlugin>()> factory) override {
        std::lock_guard<std::mutex> lock(mutex_);
        factories_[type] = std::move(factory);
    }

private:
    std::string pluginPath_;
    std::unique_ptr<Config> config_;
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> plugins_;
    std::unordered_map<std::string, std::function<std::shared_ptr<IPlugin>()>> factories_;
    mutable std::mutex mutex_;
};

std::shared_ptr<PluginManager> createPluginManager() {
    return std::make_shared<PluginManagerImpl>();
}

} // namespace core
} // namespace ime