#include "settings_window.h"
#include <fstream>
#include <shlobj.h>

#define SETTINGS_WINDOW_CLASS L"PitBossSettingsWindowClass"
#define SETTINGS_WINDOW_TITLE L"Pit Boss Settings"

SettingsWindow::SettingsWindow(HINSTANCE hInstance) : hInstance_(hInstance) {}
SettingsWindow::~SettingsWindow() {
    if (hwnd_) DestroyWindow(hwnd_);
}

void SettingsWindow::Show(HWND parent) {
    if (!hwnd_) {
        CreateWindowClass();
        CreateWindowInstance(parent);
    }
    ShowWindow(hwnd_, SW_SHOW);
    SetForegroundWindow(hwnd_);
}

void SettingsWindow::CreateWindowClass() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = SettingsWindow::WindowProc;
    wc.hInstance = hInstance_;
    wc.lpszClassName = SETTINGS_WINDOW_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);
}

void SettingsWindow::CreateWindowInstance(HWND parent) {
    hwnd_ = CreateWindowEx(0, SETTINGS_WINDOW_CLASS, SETTINGS_WINDOW_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        parent, NULL, hInstance_, NULL);
}

LRESULT CALLBACK SettingsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::wstring SettingsWindow::GetConfigPath() {
    wchar_t path[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path);
    std::wstring configPath = path;
    configPath += L"\\pitboss_config.txt";
    return configPath;
}

void SettingsWindow::SaveConfig(const std::wstring& configData) {
    std::wofstream ofs(GetConfigPath());
    ofs << configData;
}

std::wstring SettingsWindow::LoadConfig() {
    std::wifstream ifs(GetConfigPath());
    std::wstring data((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
    return data;
}
