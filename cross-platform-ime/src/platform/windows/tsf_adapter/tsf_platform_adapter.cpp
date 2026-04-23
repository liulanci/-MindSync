#include "tsf_platform_adapter.h"
#include <windows.h>
#include <msctf.h>
#include <algorithm>
#include <codecvt>
#include <locale>

namespace ime {
namespace platform {
namespace windows {

static const wchar_t* kCandidateWindowClass = L"ImeCandidateWindow";
static const wchar_t* kStatusBarWindowClass = L"ImeStatusBar";

TsfPlatformAdapter::TsfPlatformAdapter()
    : instance_(GetModuleHandle(nullptr)) {}

TsfPlatformAdapter::~TsfPlatformAdapter() {
    shutdown();
}

bool TsfPlatformAdapter::initialize() {
    if (initialized_) {
        return true;
    }

    if (!initializeTsf()) {
        return false;
    }

    if (!createCandidateWindow()) {
        cleanupTsf();
        return false;
    }

    initialized_ = true;
    return true;
}

bool TsfPlatformAdapter::commitText(const std::string& text) {
    if (!context_) {
        return false;
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);

    ITfRange* range = nullptr;
    HRESULT hr = context_->GetSelection(ecSel, 0, 1, &range, nullptr);
    if (SUCCEEDED(hr) && range) {
        VARIANT var;
        var.vt = VT_BSTR;
        var.bstrVal = SysAllocString(wtext.c_str());
        range->SetText(ecSel, 0, wtext.c_str(), static_cast<LONG>(wtext.size()));
        range->Release();
        SysFreeString(var.bstrVal);
        return true;
    }

    return false;
}

bool TsfPlatformAdapter::updatePreedit(const std::string& text, size_t cursorPos) {
    if (!context_) {
        return false;
    }

    ITfComposition* composition = nullptr;
    ITfCompositionSink* compositionSink = nullptr;

    ITfContextComposition* contextComposition = nullptr;
    HRESULT hr = context_->QueryInterface(IID_ITfContextComposition,
        reinterpret_cast<void**>(&contextComposition));
    if (FAILED(hr)) {
        return false;
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);

    if (composition) {
        ITfRange* compRange = nullptr;
        if (SUCCEEDED(composition->GetRange(&compRange)) && compRange) {
            compRange->SetText(ecSel, 0, wtext.c_str(), static_cast<LONG>(wtext.size()));
            compRange->Release();
        }
    }

    contextComposition->Release();
    return true;
}

bool TsfPlatformAdapter::showCandidateWindow(const std::vector<std::string>& candidates,
    size_t selectedIndex, size_t page, size_t totalPages) {
    if (!candidateWindow_) {
        return false;
    }

    updateCandidateWindowContent(candidates, selectedIndex);
    ShowWindow(candidateWindow_, SW_SHOWNOACTIVATE);
    return true;
}

bool TsfPlatformAdapter::hideCandidateWindow() {
    if (!candidateWindow_) {
        return false;
    }
    ShowWindow(candidateWindow_, SW_HIDE);
    return true;
}

bool TsfPlatformAdapter::updateCandidateWindowPosition(int x, int y) {
    if (!candidateWindow_) {
        return false;
    }
    SetWindowPos(candidateWindow_, nullptr, x, y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    return true;
}

bool TsfPlatformAdapter::showStatusBar() {
    return true;
}

bool TsfPlatformAdapter::hideStatusBar() {
    return true;
}

bool TsfPlatformAdapter::updateStatusBar(const std::string& status) {
    return true;
}

bool TsfPlatformAdapter::getCursorPosition(int& x, int& y) {
    POINT pt;
    if (GetCursorPos(&pt)) {
        x = pt.x;
        y = pt.y;
        return true;
    }
    return false;
}

bool TsfPlatformAdapter::getScreenSize(int& width, int& height) {
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
    return true;
}

std::string TsfPlatformAdapter::getCurrentAppId() const {
    return currentAppId_;
}

std::string TsfPlatformAdapter::getCurrentAppName() const {
    return currentAppName_;
}

void TsfPlatformAdapter::setKeyEventHandler(
    std::function<bool(int keyCode, int modifiers, bool isKeyDown)> handler) {
    keyHandler_ = std::move(handler);
}

void TsfPlatformAdapter::setFocusChangedHandler(
    std::function<void(const std::string& appId)> handler) {
    focusHandler_ = std::move(handler);
}

void TsfPlatformAdapter::shutdown() {
    if (!initialized_) {
        return;
    }

    destroyCandidateWindow();
    cleanupTsf();
    initialized_ = false;
}

std::string TsfPlatformAdapter::getPlatformName() const {
    return "Windows TSF";
}

bool TsfPlatformAdapter::registerTextService() {
    return true;
}

bool TsfPlatformAdapter::unregisterTextService() {
    return true;
}

ITfThreadMgr* TsfPlatformAdapter::getThreadManager() const {
    return threadMgr_;
}

ITfDocumentMgr* TsfPlatformAdapter::getDocumentManager() const {
    return docMgr_;
}

ITfContext* TsfPlatformAdapter::getCurrentContext() const {
    return context_;
}

bool TsfPlatformAdapter::initializeTsf() {
    HRESULT hr = CoCreateInstance(CLSID_TF_ThreadMgr, nullptr, CLSCTX_INPROC_SERVER,
        IID_ITfThreadMgr, reinterpret_cast<void**>(&threadMgr_));
    if (FAILED(hr)) {
        return false;
    }

    hr = threadMgr_->Activate(&clientId_);
    if (FAILED(hr)) {
        threadMgr_->Release();
        threadMgr_ = nullptr;
        return false;
    }

    hr = threadMgr_->CreateDocumentMgr(&docMgr_);
    if (FAILED(hr)) {
        threadMgr_->Deactivate();
        threadMgr_->Release();
        threadMgr_ = nullptr;
        return false;
    }

    return true;
}

void TsfPlatformAdapter::cleanupTsf() {
    if (context_) {
        context_->Release();
        context_ = nullptr;
    }

    if (docMgr_) {
        docMgr_->Release();
        docMgr_ = nullptr;
    }

    if (threadMgr_) {
        threadMgr_->Deactivate();
        threadMgr_->Release();
        threadMgr_ = nullptr;
    }
}

bool TsfPlatformAdapter::createCandidateWindow() {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = candidateWindowProc;
    wc.hInstance = instance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kCandidateWindowClass;

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    candidateWindow_ = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kCandidateWindowClass,
        L"",
        WS_POPUP | WS_BORDER,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
        nullptr, nullptr, instance_, this);

    return candidateWindow_ != nullptr;
}

void TsfPlatformAdapter::destroyCandidateWindow() {
    if (candidateWindow_) {
        DestroyWindow(candidateWindow_);
        candidateWindow_ = nullptr;
    }
    UnregisterClassW(kCandidateWindowClass, instance_);
}

void TsfPlatformAdapter::updateCandidateWindowContent(
    const std::vector<std::string>& candidates, size_t selectedIndex) {
    if (!candidateWindow_) {
        return;
    }

    InvalidateRect(candidateWindow_, nullptr, TRUE);
    UpdateWindow(candidateWindow_);
}

LRESULT CALLBACK TsfPlatformAdapter::candidateWindowProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND: {
            return 1;
        }
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

} // namespace windows
} // namespace platform
} // namespace ime