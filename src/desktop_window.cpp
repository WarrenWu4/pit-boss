#include "desktop_window.h"

DesktopWindow::DesktopWindow(HINSTANCE hInstance) : hInstance(hInstance) {
    // create window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = DesktopWindow::WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DesktopWindowClass";
    RegisterClass(&wc);

    // initialize window
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, // optional window styles
        L"DesktopWindowClass", // window class
        L"Virtual Desktops", // window text
        WS_POPUP | WS_VISIBLE, // window style
        400, 400, // window position
        400, 48, // window size 
        nullptr, // parent window
        nullptr, // menu
        hInstance, // instance handle
        nullptr // additional application data
    );
    // SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    // SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA); // adjust transparency 0-255
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
    }

    // closeIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOSE_ICON));
}

DesktopWindow::~DesktopWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

void drawContainer(HWND hwnd, HDC hdc) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    HBRUSH brush = CreateSolidBrush(RGB(24, 24, 37));
    HPEN hPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 8, 8);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(hPen);
}

void drawCloseButton(HINSTANCE hInstance, HDC hdc) {
    HICON closeIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOSE_ICON));
    ICONINFO iconInfo;
    GetIconInfo(closeIcon, &iconInfo);
    BITMAP bm;
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
    int iconWidth = bm.bmWidth;
    int iconHeight = bm.bmHeight;
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    int iconX = 10+(32-iconWidth)/2;
    int iconY = 8+(32-iconHeight)/2;
    DrawIcon(hdc, iconX, iconY, closeIcon);
}

LRESULT CALLBACK DesktopWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RECT closeRect = {370, 10, 390, 30};
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            if (pt.x < 30) {
                ReleaseCapture();
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
            if (PtInRect(&closeRect, pt)) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            drawContainer(hwnd, hdc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
