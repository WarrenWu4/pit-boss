#pragma once
#include <windows.h>
#include <string>
#include <vector>

// very simple manager
// primarily used to resize windows
// to fit the top bar

struct WindowInfo {
    HWND handle;
    std::wstring title;
    HICON icon;

    WindowInfo(): handle(nullptr), icon(nullptr) {}
    WindowInfo(HWND h, const std::wstring& t, HICON i) : handle(h), title(t), icon(i) {}
};

class WindowManager {
private:
    std::vector<WindowInfo> windows;
    int topBarHeight = 20;
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    BOOL ProcessWindow(HWND hwnd);
    HICON GetWindowIcon(HWND hwnd);
public:
    WindowManager();
    ~WindowManager();
    void RefreshWindows();
    std::vector<WindowInfo>& GetWindows();
    bool FocusAndResizeWindow(HWND hwnd);
    void ResizeAllWindowsToWorkspace();
    const WindowInfo* FindWindow(HWND hwnd) const;
    HWND GetActiveWindow() const;
};
