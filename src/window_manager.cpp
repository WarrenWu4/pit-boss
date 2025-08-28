#include "window_manager.h"

BOOL CALLBACK WindowManager::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    WindowManager* manager = reinterpret_cast<WindowManager*>(lParam);
    return manager->ProcessWindow(hwnd);
}

BOOL WindowManager::ProcessWindow(HWND hwnd) {
    // skip invalid windows
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return TRUE;
    }
    // skip windows without titles
    int titleLength = GetWindowTextLength(hwnd);
    if (titleLength == 0)
    {
        return TRUE;
    }
    // skip tool windows and child windows
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) || GetParent(hwnd) != nullptr)
    {
        return TRUE;
    }
    // get window title
    std::vector<wchar_t> titleBuffer(titleLength + 1);
    GetWindowText(hwnd, titleBuffer.data(), titleLength + 1);
    std::wstring title = titleBuffer.data();
    // get window icon
    HICON icon = GetWindowIcon(hwnd);
    // add to list of windows
    windows.emplace_back(hwnd, title, icon);
    return TRUE;
}

HICON WindowManager::GetWindowIcon(HWND hwnd)
{
    HICON icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);
    if (!icon)
    {
        icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    }
    if (!icon)
    {
        icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
    }
    if (!icon)
    {
        icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    }
    if (!icon)
    {
        // Use default application icon as fallback
        icon = LoadIcon(nullptr, IDI_APPLICATION);
    }
    return icon;
}

WindowManager::WindowManager(): logger(L"build/error.log") {}

WindowManager::~WindowManager() {}

void WindowManager::RefreshWindows() {
    windows.clear();
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

std::vector<WindowInfo>& WindowManager::GetWindows() {
    return windows;
}

bool WindowManager::FocusAndResizeWindow(HWND hwnd)
{
    if (!IsWindow(hwnd))
    {
        return false;
    }
    WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
    GetWindowPlacement(hwnd, &wp);
    // If minimized, restore first
    if (wp.showCmd == SW_SHOWMINIMIZED)
    {
        ShowWindow(hwnd, SW_RESTORE);
    }
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate available workspace (accounting for top bar)
    int workspaceTop = topBarHeight;
    int workspaceHeight = screenHeight - topBarHeight;

    // Check if window needs resizing
    RECT currentRect;
    GetWindowRect(hwnd, &currentRect);

    bool needsResize = false;
    RECT newRect = currentRect;

    // If window extends above the workspace, move it down
    if (currentRect.top < workspaceTop)
    {
        int windowHeight = currentRect.bottom - currentRect.top;
        newRect.top = workspaceTop;
        newRect.bottom = workspaceTop + windowHeight;
        needsResize = true;
    }

    // If window is too tall for workspace, resize it
    int windowHeight = newRect.bottom - newRect.top;
    if (windowHeight > workspaceHeight)
    {
        newRect.bottom = newRect.top + workspaceHeight;
        needsResize = true;
    }

    // If window extends below workspace, move it up
    if (newRect.bottom > screenHeight)
    {
        int height = newRect.bottom - newRect.top;
        newRect.bottom = screenHeight;
        newRect.top = screenHeight - height;

        // Ensure it doesn't go above workspace
        if (newRect.top < workspaceTop)
        {
            newRect.top = workspaceTop;
        }
        needsResize = true;
    }

    // Apply resize if needed
    if (needsResize)
    {
        SetWindowPos(hwnd, HWND_TOP,
                     newRect.left, newRect.top,
                     newRect.right - newRect.left,
                     newRect.bottom - newRect.top,
                     SWP_SHOWWINDOW);
    }

    // Focus the window
    SetForegroundWindow(hwnd);

    return true;
}

void WindowManager::ResizeAllWindowsToWorkspace()
{
    RefreshWindows();
    logger.LogMessageToFile(L"There are currently " + std::to_wstring(windows.size()) + L" windows open.");
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int workspaceTop = topBarHeight;
    int workspaceHeight = screenHeight - topBarHeight;

    for (const auto &window : windows)
    {
        if (!IsWindowVisible(window.handle))
        {
            continue;
        }
        // Skip maximized windows - they'll be handled by work area
        // ^ this was a fucking lie, it is NOT handled by work area
        // current solution: force refresh
        WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
        GetWindowPlacement(window.handle, &wp);
        if (wp.showCmd == SW_SHOWMAXIMIZED)
        {
            logger.LogMessageToFile(L"Skipping maximized window: " + window.title);
            continue;
        }
        logger.LogMessageToFile(L"Changing window size: " + window.title);
        RECT rect;
        GetWindowRect(window.handle, &rect);
        bool needsResize = false;
        RECT newRect = rect;
        // Adjust if window overlaps with top bar
        if (rect.top < workspaceTop)
        {
            int windowHeight = rect.bottom - rect.top;
            // If window is taller than workspace, resize it
            if (windowHeight > workspaceHeight)
            {
                newRect.top = workspaceTop;
                newRect.bottom = workspaceTop + workspaceHeight;
            }
            else
            {
                // Just move it down
                newRect.top = workspaceTop;
                newRect.bottom = workspaceTop + windowHeight;
            }
            needsResize = true;
        }
        if (needsResize)
        {
            SetWindowPos(window.handle, HWND_TOP,
                         newRect.left, newRect.top,
                         newRect.right - newRect.left,
                         newRect.bottom - newRect.top,
                         SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }
}

const WindowInfo *WindowManager::FindWindow(HWND hwnd) const
{
    for (const auto &window : windows)
    {
        if (window.handle == hwnd)
        {
            return &window;
        }
    }
    return nullptr;
}

HWND WindowManager::GetActiveWindow() const
{
    return GetForegroundWindow();
}