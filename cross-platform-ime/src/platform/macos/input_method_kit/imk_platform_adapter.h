#pragma once

#include "ime/core/platform_adapter.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

#import <InputMethodKit/InputMethodKit.h>

namespace ime {
namespace platform {
namespace macos {

/**
 * @brief macOS Input Method Kit平台适配器
 * 
 * 基于macOS Input Method Kit框架实现的输入法平台适配器
 * 支持macOS 10.14 (Mojave)及更高版本
 */
class ImkPlatformAdapter : public ime::core::PlatformAdapter {
public:
    ImkPlatformAdapter();
    ~ImkPlatformAdapter() override;

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
     * @brief 设置IMK服务器
     */
    void setImkServer(IMKServer* server);

    /**
     * @brief 设置当前客户端
     */
    void setCurrentClient(id<IMKStateSetting> client);

    /**
     * @brief 获取IMK控制器
     */
    IMKInputController* getInputController() const;

private:
    void* imkServer_;
    void* currentClient_;
    void* inputController_;
    bool initialized_;
    std::string currentAppId_;
    std::string currentAppName_;
    std::function<bool(int keyCode, int modifiers, bool isKeyDown)> keyHandler_;
    std::function<void(const std::string& appId)> focusHandler_;
};

} // namespace macos
} // namespace platform
} // namespace ime