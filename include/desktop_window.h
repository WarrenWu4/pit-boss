// display virtual desktop information
#pragma once

#include "resource.h"
#include <windows.h>
#include <vector>
#include <string>

struct Point {
    int x;
    int y;
};

class DesktopWindow {
    private:
        HINSTANCE hInstance;
        HWND hwnd = nullptr;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        void CalculateLayout(std::vector<std::wstring> desktops);
        void DrawContainer(HDC hdc);
        void DrawCloseButton(HDC hdc);
        void DrawDesktopNames(HDC hdc);
        void DrawDraggableButton(HDC hdc);

        std::vector<std::wstring> desktopNames;
        int currentDesktopIndex;

        RECT container = {};
        RECT closeRect = {};
        RECT dragRect = {};
        std::vector<RECT> desktopRects;

        Point containerPadding = {10, 8};
        Point textPadding = {8, 4};
        int buttonSize = 32;
        int borderRadius = 8;
        int gap = 8;

        HFONT hFont = nullptr;
        HANDLE hFontRes = nullptr;
    public:
        DesktopWindow(HINSTANCE hInstance);
        ~DesktopWindow();
        void setDesktopNames(std::vector<std::wstring> names);
        void setCurrentDesktopIndex(int index);
};