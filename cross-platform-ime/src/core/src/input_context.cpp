#include "ime/core/input_context.h"
#include <random>
#include <chrono>

namespace ime {
namespace core {

InputContext::InputContext()
    : cursorPosition_(0)
    , selectionStart_(0)
    , selectionEnd_(0)
    , contextId_(generateContextId())
    , creationTime_(std::chrono::system_clock::now())
    , lastActivityTime_(creationTime_)
    , inputMode_(InputMode::Chinese)
    , languageCode_("zh-CN") {}

const std::string& InputContext::getInputText() const {
    return inputText_;
}

void InputContext::setInputText(const std::string& text) {
    inputText_ = text;
    cursorPosition_ = text.size();
    updateActivityTime();
}

void InputContext::appendInputText(const std::string& text) {
    inputText_.insert(cursorPosition_, text);
    cursorPosition_ += text.size();
    updateActivityTime();
}

void InputContext::clearInputText() {
    inputText_.clear();
    cursorPosition_ = 0;
    selectionStart_ = 0;
    selectionEnd_ = 0;
    updateActivityTime();
}

size_t InputContext::getCursorPosition() const {
    return cursorPosition_;
}

void InputContext::setCursorPosition(size_t position) {
    cursorPosition_ = std::min(position, inputText_.size());
    updateActivityTime();
}

size_t InputContext::getSelectionStart() const {
    return selectionStart_;
}

size_t InputContext::getSelectionEnd() const {
    return selectionEnd_;
}

void InputContext::setSelection(size_t start, size_t end) {
    selectionStart_ = std::min(start, inputText_.size());
    selectionEnd_ = std::min(end, inputText_.size());
    updateActivityTime();
}

bool InputContext::hasSelection() const {
    return selectionStart_ != selectionEnd_;
}

const std::vector<KeyEvent>& InputContext::getKeyEventHistory() const {
    return keyEventHistory_;
}

void InputContext::addKeyEvent(const KeyEvent& event) {
    keyEventHistory_.push_back(event);
    if (keyEventHistory_.size() > 100) {
        keyEventHistory_.erase(keyEventHistory_.begin());
    }
    updateActivityTime();
}

const KeyEvent* InputContext::getLastKeyEvent() const {
    if (keyEventHistory_.empty()) {
        return nullptr;
    }
    return &keyEventHistory_.back();
}

uint64_t InputContext::getContextId() const {
    return contextId_;
}

std::chrono::system_clock::time_point InputContext::getCreationTime() const {
    return creationTime_;
}

std::chrono::system_clock::time_point InputContext::getLastActivityTime() const {
    return lastActivityTime_;
}

void InputContext::updateActivityTime() {
    lastActivityTime_ = std::chrono::system_clock::now();
}

const std::string& InputContext::getUserId() const {
    return userId_;
}

void InputContext::setUserId(const std::string& userId) {
    userId_ = userId;
}

const std::string& InputContext::getDeviceId() const {
    return deviceId_;
}

void InputContext::setDeviceId(const std::string& deviceId) {
    deviceId_ = deviceId;
}

const std::string& InputContext::getAppId() const {
    return appId_;
}

void InputContext::setAppId(const std::string& appId) {
    appId_ = appId;
}

InputMode InputContext::getInputMode() const {
    return inputMode_;
}

void InputContext::setInputMode(InputMode mode) {
    inputMode_ = mode;
    updateActivityTime();
}

const std::string& InputContext::getLanguageCode() const {
    return languageCode_;
}

void InputContext::setLanguageCode(const std::string& code) {
    languageCode_ = code;
}

void InputContext::clearCustomData(const std::string& key) {
    auto it = customData_.find(key);
    if (it != customData_.end()) {
        if (it->second.deleter && it->second.data) {
            it->second.deleter(it->second.data.get());
        }
        customData_.erase(it);
    }
}

void InputContext::reset() {
    inputText_.clear();
    cursorPosition_ = 0;
    selectionStart_ = 0;
    selectionEnd_ = 0;
    keyEventHistory_.clear();
    inputMode_ = InputMode::Chinese;
    languageCode_ = "zh-CN";
    customData_.clear();
    updateActivityTime();
}

uint64_t InputContext::generateContextId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    return dis(gen);
}

} // namespace core
} // namespace ime