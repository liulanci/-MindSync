#pragma once

#include "ime/core/platform_adapter.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace ime {
namespace platform {
namespace linux_os {

/**
 * @brief Linux IBus平台适配器
 * 
 * 基于IBus输入法框架实现的Linux平台适配器
 * 支持Ubuntu 18.04+, Fedora 30+, Debian 10+
 */
class IbusPlatformAdapter : public ime::core::PlatformAdapter {
public:
    IbusPlatformAdapter();
    ~IbusPlatformAdapter() override;

    bool initialize() override;
    bool commitText(const std::string& text) override;
    bool updatePreedit(const std::string& text, size_t cursorPos) override;
    bool showCandidateWindow(const std::vector<std::string>& candidates,
        size_t selectedIndex, size_t page, size_t totalPages) override;
    bool hideCandidateWindow() override;
    bool updateCandidateWindowPosition(int x, int y) override;
    bool showStatusBar() override;
    bool hideStatusBar() override;
    bool updateStatusBar(const std::string& status) override;
    bool getCursorPosition(int& x, int& y) override;
    bool getScreenSize(int& width, int& height) override;
    std::string getCurrentAppId() const override;
    std::string getCurrentAppName() const override;
    void setKeyEventHandler(
        std::function<bool(int keyCode, int modifiers, bool isKeyDown)> handler) override;
    void setFocusChangedHandler(
        std::function<void(const std::string& appId)> handler) override;
    void shutdown() override;
    std::string getPlatformName() const override;

    /**
     * @brief 获取IBus总线
     */
    void* getIbusBus() const;

    /**
     * @brief 获取IBus引擎
     */
    void* getIbusEngine() const;

    /**
     * @brief 设置IBus引擎
     */
    void setIbusEngine(void* engine);

private:
    void* ibusBus_;
    void* ibusEngine_;
    bool initialized_;
    std::string currentAppId_;
    std::string currentAppName_;
    std::function<bool(int keyCode, int modifiers, bool isKeyDown)> keyHandler_;
    std::function<void(const std::string& appId)> focusHandler_;
};

} // namespace linux_os
} // namespace platform
} // namespace ime