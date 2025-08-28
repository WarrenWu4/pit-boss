#pragma once
#include <windows.h>
#include <string>

class TopBar {
public:
    TopBar(HINSTANCE hInstance);
    ~TopBar();
    bool Initialize();
    void UpdateWorkArea();
    void RestoreWorkArea();
    void Show();
    void Hide();
private:
    HWND hwnd_ = nullptr;
    HINSTANCE hInstance_;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    int barHeight = 20;
    bool isVisible = true;
    RECT originalWorkArea;
};
