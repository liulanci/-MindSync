#pragma once

#include "ime/core/platform_adapter.h"
#include <windows.h>
#include <msctf.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace ime {
namespace platform {
namespace windows {

/**
 * @brief Windows TSF平台适配器
 * 
 * 基于Windows文本服务框架(TSF)实现的输入法平台适配器
 * 支持Windows 10及更高版本
 */
class TsfPlatformAdapter : public ime::core::PlatformAdapter {
public:
    TsfPlatformAdapter();
    ~TsfPlatformAdapter() override;

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
     * @brief 注册TSF文本服务
     */
    bool registerTextService();

    /**
     * @brief 注销TSF文本服务
     */
    bool unregisterTextService();

    /**
     * @brief 获取TSF线程管理器
     */
    ITfThreadMgr* getThreadManager() const;

    /**
     * @brief 获取TSF文档管理器
     */
    ITfDocumentMgr* getDocumentManager() const;

    /**
     * @brief 获取当前上下文
     */
    ITfContext* getCurrentContext() const;

private:
    class TsfTextService;
    friend class TsfTextService;

    bool initializeTsf();
    void cleanupTsf();
    bool createCandidateWindow();
    void destroyCandidateWindow();
    void updateCandidateWindowContent(const std::vector<std::string>& candidates,
        size_t selectedIndex);
    static LRESULT CALLBACK candidateWindowProc(HWND hwnd, UINT msg,
        WPARAM wParam, LPARAM lParam);

    ITfThreadMgr* threadMgr_ = nullptr;
    ITfDocumentMgr* docMgr_ = nullptr;
    ITfContext* context_ = nullptr;
    TfClientId clientId_ = TF_CLIENTID_NULL;

    HWND candidateWindow_ = nullptr;
    HWND statusBarWindow_ = nullptr;
    HINSTANCE instance_ = nullptr;

    std::function<bool(int keyCode, int modifiers, bool isKeyDown)> keyHandler_;
    std::function<void(const std::string& appId)> focusHandler_;

    bool initialized_ = false;
    std::string currentAppId_;
    std::string currentAppName_;
};

} // namespace windows
} // namespace platform
} // namespace ime