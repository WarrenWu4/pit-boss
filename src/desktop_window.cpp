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
        this // additional application data
    );
    // SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    // SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA); // adjust transparency 0-255
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
    }

    // closeIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOSE_ICON));

    desktopNames = { L"Testing", L"testing2" };
    currentDesktopIndex = 0;
}

DesktopWindow::~DesktopWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

void DesktopWindow::DrawContainer(HDC hdc) {
    RECT parent;
    GetClientRect(hwnd, &parent);
    // RECT rect = {
    //     parent.left,
    //     parent.top,
    //     dragRect.right+containerPadding.x,
    //     parent.bottom
    // };
    RECT rect = parent;
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

void DesktopWindow::DrawDesktopNames(HDC hdc) {
    // grab position of parent and calculate next child position
    RECT parent;
    GetClientRect(hwnd, &parent);
    int rectX = closeRect.right + gap;
    int rectY = closeRect.top;

    // set text parameters
    SetTextColor(hdc, RGB(135, 141, 164));
    SetBkMode(hdc, TRANSPARENT);

    // iterate through desktop names and draw each
    for (size_t i = 0; i < desktopNames.size(); i++) {
        const std::wstring& name = desktopNames[i];
        SIZE size;
        GetTextExtentPoint32(hdc, name.c_str(), name.length(), &size);
        RECT textRect = { rectX, rectY, rectX+size.cx+2*textPadding.x, closeRect.bottom };
        HBRUSH brush = CreateSolidBrush(RGB(30, 30, 46));
        HPEN pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        RoundRect(hdc, textRect.left, textRect.top, textRect.right, textRect.bottom, borderRadius, borderRadius);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
        DrawText(hdc, name.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        rectX = textRect.right + gap;
    }
    finalDesktopRect = {rectX, rectY, rectX, rectY };
}

void DesktopWindow::DrawCloseButton(HDC hdc) {
    // calculate position relative to parent
    RECT parent;
    GetClientRect(hwnd, &parent);
    int rectX = parent.left+containerPadding.x;
    int rectY = parent.top+containerPadding.y;

    // create and draw new rect
    int rectWidth = 32;
    int rectHeight = 32;
    closeRect = {rectX, rectY, rectX+rectWidth, rectY+rectHeight};
    HBRUSH brush = CreateSolidBrush(RGB(30, 30, 46));
    HPEN pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    RoundRect(hdc, closeRect.left, closeRect.top, closeRect.right, closeRect.bottom, borderRadius, borderRadius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);

    // create and draw close icon
    HICON closeIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOSE_ICON));
    int iconWidth = 16;
    int iconHeight = 16;
    int iconX = rectX+(rectWidth-iconWidth)/2;
    int iconY = rectY+(rectHeight-iconHeight)/2;
    DrawIcon(hdc, iconX, iconY, closeIcon);
}

void DesktopWindow::DrawDraggableButton(HDC hdc) {
    RECT parent;
    GetClientRect(hwnd, &parent);
    int rectX = finalDesktopRect.right;
    int rectY = parent.top+containerPadding.y;
    int rectWidth = 32;
    dragRect = {rectX, rectY, rectX+rectWidth, rectY+rectWidth};
    HBRUSH brush = CreateSolidBrush(RGB(30, 30, 46));
    HPEN pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    RoundRect(hdc, dragRect.left, dragRect.top, dragRect.right, dragRect.bottom, borderRadius, borderRadius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

LRESULT CALLBACK DesktopWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    DesktopWindow* pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        pThis = reinterpret_cast<DesktopWindow *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->hwnd = hwnd;
    } else {
        pThis = reinterpret_cast<DesktopWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            if (PtInRect(&pThis->dragRect, pt)) {
                ReleaseCapture();
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
            if (PtInRect(&pThis->closeRect, pt)) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            pThis->DrawContainer(hdc);
            pThis->DrawCloseButton(hdc);
            pThis->DrawDesktopNames(hdc);
            pThis->DrawDraggableButton(hdc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
