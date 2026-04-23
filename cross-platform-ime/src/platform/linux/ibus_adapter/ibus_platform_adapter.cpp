#include "ibus_platform_adapter.h"
#include <ibus.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>

namespace ime {
namespace platform {
namespace linux_os {

IbusPlatformAdapter::IbusPlatformAdapter()
    : ibusBus_(nullptr)
    , ibusEngine_(nullptr)
    , initialized_(false) {}

IbusPlatformAdapter::~IbusPlatformAdapter() {
    shutdown();
}

bool IbusPlatformAdapter::initialize() {
    if (initialized_) {
        return true;
    }

    ibus_init();

    IBusBus* bus = ibus_bus_new();
    if (!ibus_bus_is_connected(bus)) {
        g_object_unref(bus);
        return false;
    }

    ibusBus_ = bus;
    initialized_ = true;
    return true;
}

bool IbusPlatformAdapter::commitText(const std::string& text) {
    if (!ibusEngine_) {
        return false;
    }

    IBusEngine* engine = static_cast<IBusEngine*>(ibusEngine_);
    IBusText* ibusText = ibus_text_new_from_string(text.c_str());
    ibus_engine_commit_text(engine, ibusText);
    g_object_unref(ibusText);
    return true;
}

bool IbusPlatformAdapter::updatePreedit(const std::string& text, size_t cursorPos) {
    if (!ibusEngine_) {
        return false;
    }

    IBusEngine* engine = static_cast<IBusEngine*>(ibusEngine_);
    IBusText* ibusText = ibus_text_new_from_string(text.c_str());
    ibus_engine_update_preedit_text(engine, ibusText,
        static_cast<guint>(cursorPos), TRUE);
    g_object_unref(ibusText);
    return true;
}

bool IbusPlatformAdapter::showCandidateWindow(const std::vector<std::string>& candidates,
    size_t selectedIndex, size_t page, size_t totalPages) {
    if (!ibusEngine_) {
        return false;
    }

    IBusEngine* engine = static_cast<IBusEngine*>(ibusEngine_);
    IBusLookupTable* table = ibus_lookup_table_new(
        5, static_cast<guint>(selectedIndex), TRUE, TRUE);

    for (const auto& candidate : candidates) {
        IBusText* text = ibus_text_new_from_string(candidate.c_str());
        ibus_lookup_table_append_candidate(table, text);
        g_object_unref(text);
    }

    ibus_engine_update_lookup_table(engine, table, TRUE);
    g_object_unref(table);
    return true;
}

bool IbusPlatformAdapter::hideCandidateWindow() {
    if (!ibusEngine_) {
        return false;
    }

    IBusEngine* engine = static_cast<IBusEngine*>(ibusEngine_);
    ibus_engine_hide_lookup_table(engine);
    return true;
}

bool IbusPlatformAdapter::updateCandidateWindowPosition(int x, int y) {
    return true;
}

bool IbusPlatformAdapter::showStatusBar() {
    return true;
}

bool IbusPlatformAdapter::hideStatusBar() {
    return true;
}

bool IbusPlatformAdapter::updateStatusBar(const std::string& status) {
    return true;
}

bool IbusPlatformAdapter::getCursorPosition(int& x, int& y) {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        return false;
    }

    Window root = DefaultRootWindow(display);
    Window child;
    int winX, winY;
    unsigned int mask;

    XQueryPointer(display, root, &root, &child, &x, &y, &winX, &winY, &mask);
    XCloseDisplay(display);
    return true;
}

bool IbusPlatformAdapter::getScreenSize(int& width, int& height) {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        return false;
    }

    Screen* screen = DefaultScreenOfDisplay(display);
    width = WidthOfScreen(screen);
    height = HeightOfScreen(screen);
    XCloseDisplay(display);
    return true;
}

std::string IbusPlatformAdapter::getCurrentAppId() const {
    return currentAppId_;
}

std::string IbusPlatformAdapter::getCurrentAppName() const {
    return currentAppName_;
}

void IbusPlatformAdapter::setKeyEventHandler(
    std::function<bool(int keyCode, int modifiers, bool isKeyDown)> handler) {
    keyHandler_ = std::move(handler);
}

void IbusPlatformAdapter::setFocusChangedHandler(
    std::function<void(const std::string& appId)> handler) {
    focusHandler_ = std::move(handler);
}

void IbusPlatformAdapter::shutdown() {
    if (!initialized_) {
        return;
    }

    if (ibusBus_) {
        g_object_unref(ibusBus_);
        ibusBus_ = nullptr;
    }

    initialized_ = false;
}

std::string IbusPlatformAdapter::getPlatformName() const {
    return "Linux IBus";
}

void* IbusPlatformAdapter::getIbusBus() const {
    return ibusBus_;
}

void* IbusPlatformAdapter::getIbusEngine() const {
    return ibusEngine_;
}

void IbusPlatformAdapter::setIbusEngine(void* engine) {
    ibusEngine_ = engine;
}

} // namespace linux_os
} // namespace platform
} // namespace ime