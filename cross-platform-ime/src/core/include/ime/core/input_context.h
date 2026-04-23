#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cstdint>
#include "key_event.h"

namespace ime {
namespace core {

/**
 * @brief 输入上下文
 * 
 * 包含当前输入状态的所有信息
 */
class InputContext {
public:
    InputContext();
    virtual ~InputContext() = default;

    /**
     * @brief 获取当前输入文本
     */
    const std::string& getInputText() const;

    /**
     * @brief 设置输入文本
     */
    void setInputText(const std::string& text);

    /**
     * @brief 追加输入文本
     */
    void appendInputText(const std::string& text);

    /**
     * @brief 清空输入文本
     */
    void clearInputText();

    /**
     * @brief 获取光标位置
     */
    size_t getCursorPosition() const;

    /**
     * @brief 设置光标位置
     */
    void setCursorPosition(size_t position);

    /**
     * @brief 获取选区开始位置
     */
    size_t getSelectionStart() const;

    /**
     * @brief 获取选区结束位置
     */
    size_t getSelectionEnd() const;

    /**
     * @brief 设置选区
     */
    void setSelection(size_t start, size_t end);

    /**
     * @brief 是否有选区
     */
    bool hasSelection() const;

    /**
     * @brief 获取按键事件历史
     */
    const std::vector<KeyEvent>& getKeyEventHistory() const;

    /**
     * @brief 添加按键事件
     */
    void addKeyEvent(const KeyEvent& event);

    /**
     * @brief 获取最后按键事件
     */
    const KeyEvent* getLastKeyEvent() const;

    /**
     * @brief 获取上下文ID
     */
    uint64_t getContextId() const;

    /**
     * @brief 获取创建时间
     */
    std::chrono::system_clock::time_point getCreationTime() const;

    /**
     * @brief 获取最后活动时间
     */
    std::chrono::system_clock::time_point getLastActivityTime() const;

    /**
     * @brief 更新活动时间
     */
    void updateActivityTime();

    /**
     * @brief 获取用户ID
     */
    const std::string& getUserId() const;

    /**
     * @brief 设置用户ID
     */
    void setUserId(const std::string& userId);

    /**
     * @brief 获取设备ID
     */
    const std::string& getDeviceId() const;

    /**
     * @brief 设置设备ID
     */
    void setDeviceId(const std::string& deviceId);

    /**
     * @brief 获取应用程序ID
     */
    const std::string& getAppId() const;

    /**
     * @brief 设置应用程序ID
     */
    void setAppId(const std::string& appId);

    /**
     * @brief 获取输入法模式
     */
    InputMode getInputMode() const;

    /**
     * @brief 设置输入法模式
     */
    void setInputMode(InputMode mode);

    /**
     * @brief 获取语言代码
     */
    const std::string& getLanguageCode() const;

    /**
     * @brief 设置语言代码
     */
    void setLanguageCode(const std::string& code);

    /**
     * @brief 获取自定义数据
     */
    template<typename T>
    std::shared_ptr<T> getCustomData(const std::string& key) const;

    /**
     * @brief 设置自定义数据
     */
    template<typename T>
    void setCustomData(const std::string& key, std::shared_ptr<T> data);

    /**
     * @brief 清除自定义数据
     */
    void clearCustomData(const std::string& key);

    /**
     * @brief 重置上下文
     */
    void reset();

private:
    std::string inputText_;
    size_t cursorPosition_ = 0;
    size_t selectionStart_ = 0;
    size_t selectionEnd_ = 0;
    std::vector<KeyEvent> keyEventHistory_;
    uint64_t contextId_;
    std::chrono::system_clock::time_point creationTime_;
    std::chrono::system_clock::time_point lastActivityTime_;
    std::string userId_;
    std::string deviceId_;
    std::string appId_;
    InputMode inputMode_ = InputMode::Chinese;
    std::string languageCode_ = "zh-CN";
    
    struct CustomData {
        std::shared_ptr<void> data;
        std::function<void(void*)> deleter;
    };
    std::unordered_map<std::string, CustomData> customData_;
};

/**
 * @brief 输入法模式
 */
enum class InputMode {
    Chinese,      // 中文输入
    English,      // 英文输入
    Number,       // 数字输入
    Symbol,       // 符号输入
    Pinyin,       // 拼音输入
    Wubi,         // 五笔输入
    Cangjie,      // 仓颉输入
    Bopomofo,     // 注音输入
    Custom        // 自定义输入方案
};

} // namespace core
} // namespace ime