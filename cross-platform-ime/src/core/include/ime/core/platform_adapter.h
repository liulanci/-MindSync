#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace ime {
namespace core {

/**
 * @brief 平台适配器接口
 */
class PlatformAdapter {
public:
    virtual ~PlatformAdapter() = default;

    /**
     * @brief 初始化平台适配器
     */
    virtual bool initialize() = 0;

    /**
     * @brief 提交文本到目标应用
     */
    virtual bool commitText(const std::string& text) = 0;

    /**
     * @brief 更新预编辑文本
     */
    virtual bool updatePreedit(const std::string& text, size_t cursorPos) = 0;

    /**
     * @brief 显示候选词窗口
     */
    virtual bool showCandidateWindow(const std::vector<std::string>& candidates,
        size_t selectedIndex, size_t page, size_t totalPages) = 0;

    /**
     * @brief 隐藏候选词窗口
     */
    virtual bool hideCandidateWindow() = 0;

    /**
     * @brief 更新候选词窗口位置
     */
    virtual bool updateCandidateWindowPosition(int x, int y) = 0;

    /**
     * @brief 显示状态栏
     */
    virtual bool showStatusBar() = 0;

    /**
     * @brief 隐藏状态栏
     */
    virtual bool hideStatusBar() = 0;

    /**
     * @brief 更新状态栏
     */
    virtual bool updateStatusBar(const std::string& status) = 0;

    /**
     * @brief 获取光标位置
     */
    virtual bool getCursorPosition(int& x, int& y) = 0;

    /**
     * @brief 获取屏幕尺寸
     */
    virtual bool getScreenSize(int& width, int& height) = 0;

    /**
     * @brief 获取当前应用信息
     */
    virtual std::string getCurrentAppId() const = 0;

    /**
     * @brief 获取当前应用名称
     */
    virtual std::string getCurrentAppName() const = 0;

    /**
     * @brief 设置按键事件回调
     */
    virtual void setKeyEventHandler(
        std::function<bool(int keyCode, int modifiers, bool isKeyDown)> handler) = 0;

    /**
     * @brief 设置焦点变化回调
     */
    virtual void setFocusChangedHandler(
        std::function<void(const std::string& appId)> handler) = 0;

    /**
     * @brief 关闭平台适配器
     */
    virtual void shutdown() = 0;

    /**
     * @brief 获取平台名称
     */
    virtual std::string getPlatformName() const = 0;
};

/**
 * @brief 创建平台适配器
 */
std::shared_ptr<PlatformAdapter> createPlatformAdapter();

} // namespace core
} // namespace ime