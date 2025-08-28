#pragma once
#include <windows.h>
#include <string>

class SettingsWindow {
public:
    SettingsWindow(HINSTANCE hInstance);
    ~SettingsWindow();
    void Show(HWND parent);
    static std::wstring GetConfigPath();
    static void SaveConfig(const std::wstring& configData);
    static std::wstring LoadConfig();
private:
    HWND hwnd_ = nullptr;
    HINSTANCE hInstance_;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CreateWindowClass();
    void CreateWindowInstance(HWND parent);
};
