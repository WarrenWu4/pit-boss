// display virtual desktop information
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

        void DrawContainer(HDC hdc);
        void DrawCloseButton(HDC hdc);
        void DrawDesktopNames(HDC hdc);
        void DrawDraggableButton(HDC hdc);

        std::vector<std::wstring> desktopNames;
        int currentDesktopIndex;

        RECT closeRect = {};
        RECT finalDesktopRect = {};
        RECT dragRect = {};
        Point containerPadding = {10, 8};
        Point textPadding = {8, 4};
        int borderRadius = 8;
        int gap = 8;
    public:
        DesktopWindow(HINSTANCE hInstance);
        ~DesktopWindow();
};