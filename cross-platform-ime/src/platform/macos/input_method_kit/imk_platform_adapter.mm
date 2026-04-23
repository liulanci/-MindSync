#include "imk_platform_adapter.h"
#include <ApplicationServices/ApplicationServices.h>

namespace ime {
namespace platform {
namespace macos {

ImkPlatformAdapter::ImkPlatformAdapter()
    : imkServer_(nullptr)
    , currentClient_(nullptr)
    , inputController_(nullptr)
    , initialized_(false) {}

ImkPlatformAdapter::~ImkPlatformAdapter() {
    shutdown();
}

bool ImkPlatformAdapter::initialize() {
    if (initialized_) {
        return true;
    }

    initialized_ = true;
    return true;
}

bool ImkPlatformAdapter::commitText(const std::string& text) {
    if (!currentClient_) {
        return false;
    }

    NSString* nsText = [NSString stringWithUTF8String:text.c_str()];
    id<IMKStateSetting> client = static_cast<id<IMKStateSetting>>(currentClient_);
    [client insertText:nsText replacementRange:NSMakeRange(NSNotFound, 0)];
    return true;
}

bool ImkPlatformAdapter::updatePreedit(const std::string& text, size_t cursorPos) {
    if (!currentClient_) {
        return false;
    }

    NSDictionary* attrs = @{
        NSMarkedClauseSegmentAttributeName: @0
    };

    NSString* nsText = [NSString stringWithUTF8String:text.c_str()];
    NSRange selectedRange = NSMakeRange(cursorPos, 0);
    id<IMKStateSetting> client = static_cast<id<IMKStateSetting>>(currentClient_);
    [client setMarkedText:nsText
           selectionRange:selectedRange
         replacementRange:NSMakeRange(NSNotFound, 0)];
    return true;
}

bool ImkPlatformAdapter::showCandidateWindow(const std::vector<std::string>& candidates,
    size_t selectedIndex, size_t page, size_t totalPages) {
    return true;
}

bool ImkPlatformAdapter::hideCandidateWindow() {
    return true;
}

bool ImkPlatformAdapter::updateCandidateWindowPosition(int x, int y) {
    return true;
}

bool ImkPlatformAdapter::showStatusBar() {
    return true;
}

bool ImkPlatformAdapter::hideStatusBar() {
    return true;
}

bool ImkPlatformAdapter::updateStatusBar(const std::string& status) {
    return true;
}

bool ImkPlatformAdapter::getCursorPosition(int& x, int& y) {
    NSPoint mouseLoc = [NSEvent mouseLocation];
    x = static_cast<int>(mouseLoc.x);
    y = static_cast<int>(mouseLoc.y);
    return true;
}

bool ImkPlatformAdapter::getScreenSize(int& width, int& height) {
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSRect frame = mainScreen.frame;
    width = static_cast<int>(frame.size.width);
    height = static_cast<int>(frame.size.height);
    return true;
}

std::string ImkPlatformAdapter::getCurrentAppId() const {
    return currentAppId_;
}

std::string ImkPlatformAdapter::getCurrentAppName() const {
    return currentAppName_;
}

void ImkPlatformAdapter::setKeyEventHandler(
    std::function<bool(int keyCode, int modifiers, bool isKeyDown)> handler) {
    keyHandler_ = std::move(handler);
}

void ImkPlatformAdapter::setFocusChangedHandler(
    std::function<void(const std::string& appId)> handler) {
    focusHandler_ = std::move(handler);
}

void ImkPlatformAdapter::shutdown() {
    if (!initialized_) {
        return;
    }
    initialized_ = false;
}

std::string ImkPlatformAdapter::getPlatformName() const {
    return "macOS Input Method Kit";
}

void ImkPlatformAdapter::setImkServer(IMKServer* server) {
    imkServer_ = server;
}

void ImkPlatformAdapter::setCurrentClient(id<IMKStateSetting> client) {
    currentClient_ = client;
}

IMKInputController* ImkPlatformAdapter::getInputController() const {
    return static_cast<IMKInputController*>(inputController_);
}

} // namespace macos
} // namespace platform
} // namespace ime