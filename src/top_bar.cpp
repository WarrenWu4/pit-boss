#include "top_bar.h"
#include <fstream>
#include <shlobj.h>

#define TOP_BAR_CLASS L"PitBossTopBarClass"
#define TOP_BAR_TITLE L""
    
TopBar::TopBar(HINSTANCE hInstance) : hInstance_(hInstance) {
    // stores original work area
    SystemParametersInfo(SPI_GETWORKAREA, 0, &originalWorkArea, 0);
}

TopBar::~TopBar() {
    if (hwnd_) { 
        RestoreWorkArea();
        DestroyWindow(hwnd_); 
    }
}

LRESULT CALLBACK TopBar::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    TopBar* pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (TopBar*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->hwnd_ = hwnd;
    } else {
        pThis = (TopBar*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT TopBar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd_, &ps);
            RECT rect;
            GetClientRect(hwnd_, &rect);
            // Create gradient background
            HBRUSH hBrush = CreateSolidBrush(RGB(45, 45, 48)); // Dark gray
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            // Draw border
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            MoveToEx(hdc, 0, rect.bottom - 1, NULL);
            LineTo(hdc, rect.right, rect.bottom - 1);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
            // Draw sample text
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
            DrawText(hdc, L"Custom Top Bar - Virtual Desktops & Windows", -1, &rect,
                     DT_SINGLELINE | DT_VCENTER | DT_CENTER);
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            EndPaint(hwnd_, &ps);
        }
        default: {
            return DefWindowProc(hwnd_, uMsg, wParam, lParam);
        }
    }
}

bool TopBar::Initialize() {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = TOP_BAR_CLASS;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
    {
        return false;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    hwnd_ = CreateWindowEx(
        WS_EX_TOPMOST |           // Always on top
        WS_EX_TOOLWINDOW |        // Don't show in taskbar/Alt+Tab
        WS_EX_NOACTIVATE,         // Don't steal focus when shown
        TOP_BAR_CLASS,
        L"TopBar",
        WS_POPUP |                // No title bar, borders
        WS_VISIBLE,               // Initially visible
        0, 0,                     // Position at top-left
        screenWidth, barHeight, // Full width, custom height
        nullptr,                  // No parent
        nullptr,                  // No menu
        hInstance_,
        this                      // Pass this pointer for WM_NCCREATE
    );
    if (!hwnd_) {
        return false;
    }
    SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    UpdateWorkArea();
    isVisible = true;
    return true;
}

void TopBar::UpdateWorkArea() {
    if (!isVisible) { return; }
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    workArea.top = barHeight;
    SystemParametersInfo(SPI_SETWORKAREA, 0, &workArea, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

}

void TopBar::RestoreWorkArea() {
    SystemParametersInfo(SPI_SETWORKAREA, 0, &originalWorkArea, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}

void TopBar::Show() {
    if (hwnd_) {
        ShowWindow(hwnd_, SW_SHOW);
        isVisible = true;
        UpdateWorkArea();
    }
}

void TopBar::Hide() {
    if (hwnd_) {
        ShowWindow(hwnd_, SW_HIDE);
        isVisible = false;
        RestoreWorkArea();
    }
}