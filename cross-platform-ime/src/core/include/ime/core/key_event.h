#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <vector>

namespace ime {
namespace core {

/**
 * @brief 按键事件
 * 
 * 表示一个键盘按键事件
 */
struct KeyEvent {
    uint32_t keyCode = 0;           // 物理键码
    uint32_t virtualKey = 0;        // 虚拟键码
    uint32_t scanCode = 0;          // 扫描码
    uint32_t unicodeChar = 0;       // Unicode字符
    
    bool isKeyDown = false;         // 按键按下
    bool isKeyUp = false;           // 按键释放
    bool isRepeat = false;          // 重复按键
    
    // 修饰键状态
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    bool win = false;
    bool capsLock = false;
    bool numLock = false;
    bool scrollLock = false;
    
    std::chrono::system_clock::time_point timestamp; // 时间戳
    
    // 来源信息
    uint64_t sourceDeviceId = 0;    // 来源设备ID
    std::string sourceApp;          // 来源应用程序
    
    /**
     * @brief 获取按键名称
     */
    std::string getName() const;
    
    /**
     * @brief 获取修饰键组合名称
     */
    std::string getModifierString() const;
    
    /**
     * @brief 获取完整事件描述
     */
    std::string toString() const;
    
    /**
     * @brief 检查是否为字符键
     */
    bool isCharacter() const;
    
    /**
     * @brief 检查是否为功能键
     */
    bool isFunctionKey() const;
    
    /**
     * @brief 检查是否为导航键
     */
    bool isNavigationKey() const;
    
    /**
     * @brief 检查是否为编辑键
     */
    bool isEditingKey() const;
    
    /**
     * @brief 检查是否为数字键
     */
    bool isNumericKey() const;
    
    /**
     * @brief 检查是否为字母键
     */
    bool isAlphaKey() const;
    
    /**
     * @brief 检查是否为符号键
     */
    bool isSymbolKey() const;
    
    /**
     * @brief 检查是否为组合键
     */
    bool isCombination() const;
    
    /**
     * @brief 检查是否匹配指定键
     */
    bool matches(uint32_t key, bool checkModifiers = false) const;
};

/**
 * @brief 按键事件序列
 */
using KeyEventSequence = std::vector<KeyEvent>;

/**
 * @brief 按键事件监听器接口
 */
class KeyEventListener {
public:
    virtual ~KeyEventListener() = default;
    
    /**
     * @brief 处理按键事件
     * @param event 按键事件
     * @return 是否已处理（true表示已处理，不再传递给其他监听器）
     */
    virtual bool onKeyEvent(const KeyEvent& event) = 0;
    
    /**
     * @brief 处理按键事件序列
     * @param sequence 按键事件序列
     */
    virtual void onKeyEventSequence(const KeyEventSequence& sequence) = 0;
};

/**
 * @brief 按键事件过滤器接口
 */
class KeyEventFilter {
public:
    virtual ~KeyEventFilter() = default;
    
    /**
     * @brief 过滤按键事件
     * @param event 按键事件
     * @return 过滤后的按键事件（返回nullptr表示丢弃事件）
     */
    virtual KeyEvent* filterKeyEvent(KeyEvent& event) = 0;
};

/**
 * @brief 常用按键常量
 */
namespace KeyCodes {
    // 功能键
    constexpr uint32_t ESCAPE = 0x1B;
    constexpr uint32_t F1 = 0x70;
    constexpr uint32_t F2 = 0x71;
    constexpr uint32_t F3 = 0x72;
    constexpr uint32_t F4 = 0x73;
    constexpr uint32_t F5 = 0x74;
    constexpr uint32_t F6 = 0x75;
    constexpr uint32_t F7 = 0x76;
    constexpr uint32_t F8 = 0x77;
    constexpr uint32_t F9 = 0x78;
    constexpr uint32_t F10 = 0x79;
    constexpr uint32_t F11 = 0x7A;
    constexpr uint32_t F12 = 0x7B;
    
    // 导航键
    constexpr uint32_t HOME = 0x24;
    constexpr uint32_t END = 0x23;
    constexpr uint32_t PAGE_UP = 0x21;
    constexpr uint32_t PAGE_DOWN = 0x22;
    constexpr uint32_t UP = 0x26;
    constexpr uint32_t DOWN = 0x28;
    constexpr uint32_t LEFT = 0x25;
    constexpr uint32_t RIGHT = 0x27;
    
    // 编辑键
    constexpr uint32_t INSERT = 0x2D;
    constexpr uint32_t DELETE = 0x2E;
    constexpr uint32_t BACKSPACE = 0x08;
    constexpr uint32_t TAB = 0x09;
    constexpr uint32_t ENTER = 0x0D;
    constexpr uint32_t SPACE = 0x20;
    
    // 修饰键
    constexpr uint32_t SHIFT = 0x10;
    constexpr uint32_t CONTROL = 0x11;
    constexpr uint32_t ALT = 0x12;
    constexpr uint32_t WIN = 0x5B;
    constexpr uint32_t CAPS_LOCK = 0x14;
    constexpr uint32_t NUM_LOCK = 0x90;
    constexpr uint32_t SCROLL_LOCK = 0x91;
    
    // 数字键（主键盘区）
    constexpr uint32_t KEY_0 = 0x30;
    constexpr uint32_t KEY_1 = 0x31;
    constexpr uint32_t KEY_2 = 0x32;
    constexpr uint32_t KEY_3 = 0x33;
    constexpr uint32_t KEY_4 = 0x34;
    constexpr uint32_t KEY_5 = 0x35;
    constexpr uint32_t KEY_6 = 0x36;
    constexpr uint32_t KEY_7 = 0x37;
    constexpr uint32_t KEY_8 = 0x38;
    constexpr uint32_t KEY_9 = 0x39;
    
    // 字母键
    constexpr uint32_t KEY_A = 0x41;
    constexpr uint32_t KEY_B = 0x42;
    constexpr uint32_t KEY_C = 0x43;
    constexpr uint32_t KEY_D = 0x44;
    constexpr uint32_t KEY_E = 0x45;
    constexpr uint32_t KEY_F = 0x46;
    constexpr uint32_t KEY_G = 0x47;
    constexpr uint32_t KEY_H = 0x48;
    constexpr uint32_t KEY_I = 0x49;
    constexpr uint32_t KEY_J = 0x4A;
    constexpr uint32_t KEY_K = 0x4B;
    constexpr uint32_t KEY_L = 0x4C;
    constexpr uint32_t KEY_M = 0x4D;
    constexpr uint32_t KEY_N = 0x4E;
    constexpr uint32_t KEY_O = 0x4F;
    constexpr uint32_t KEY_P = 0x50;
    constexpr uint32_t KEY_Q = 0x51;
    constexpr uint32_t KEY_R = 0x52;
    constexpr uint32_t KEY_S = 0x53;
    constexpr uint32_t KEY_T = 0x54;
    constexpr uint32_t KEY_U = 0x55;
    constexpr uint32_t KEY_V = 0x56;
    constexpr uint32_t KEY_W = 0x57;
    constexpr uint32_t KEY_X = 0x58;
    constexpr uint32_t KEY_Y = 0x59;
    constexpr uint32_t KEY_Z = 0x5A;
}

} // namespace core
} // namespace ime