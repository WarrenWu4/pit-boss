// display virtual desktop information
#include "resource.h"
#include <windows.h>
#include <vector>
#include <string>

class DesktopWindow {
    private:
        HINSTANCE hInstance;
        HWND hwnd = nullptr;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    public:
        DesktopWindow(HINSTANCE hInstance);
        ~DesktopWindow();
};