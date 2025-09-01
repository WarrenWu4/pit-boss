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
    CalculateLayout(desktopNames);
}

DesktopWindow::~DesktopWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

void DesktopWindow::CalculateLayout(std::vector<std::wstring> desktops) {
    // set initial container rect to be top left of screen
    GetClientRect(hwnd, &container);
    // calculate close rect
    closeRect = {
        container.left + containerPadding.x,
        container.top + containerPadding.y,
        container.left + containerPadding.x + buttonSize,
        container.top + containerPadding.y + buttonSize
    };
    // calculate desktop rects
    int currentX = closeRect.right + gap;
    for (size_t i = 0; i < desktops.size(); i++) {
        const std::wstring& name = desktops.at(i);
        SIZE size;
        HDC hdc = GetDC(hwnd);
        GetTextExtentPoint32(hdc, name.c_str(), name.length(), &size);
        RECT desktopRect = {
            currentX,
            closeRect.top,
            currentX + size.cx + 2 * textPadding.x,
            closeRect.bottom
        };
        desktopRects.push_back(desktopRect); 
        currentX += size.cx + 2 * textPadding.x + gap;
    }
    // calculate draggable rect
    dragRect = {
        currentX,
        closeRect.top,
        currentX + buttonSize,
        closeRect.bottom
    };
    // fit container width to all elements
    container.right = dragRect.right + containerPadding.x;
    MoveWindow(hwnd, container.left, container.top, container.right - container.left, container.bottom - container.top, TRUE);
}

void DesktopWindow::DrawContainer(HDC hdc) {
    HBRUSH brush = CreateSolidBrush(RGB(24, 24, 37));
    HPEN hPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    RoundRect(hdc, container.left, container.top, container.right, container.bottom, borderRadius, borderRadius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(hPen);
}

void DesktopWindow::DrawDesktopNames(HDC hdc) {
    // set text parameters
    SetTextColor(hdc, RGB(135, 141, 164));
    SetBkMode(hdc, TRANSPARENT);
    // iterate through desktop names and draw each
    for (size_t i = 0; i < desktopRects.size(); i++) {
        const std::wstring& name = desktopNames[i];
        RECT rect = desktopRects[i];
        HBRUSH brush = CreateSolidBrush(RGB(30, 30, 46));
        HPEN pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, borderRadius, borderRadius);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
        DrawText(hdc, name.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void DesktopWindow::DrawCloseButton(HDC hdc) {
    HBRUSH brush = CreateSolidBrush(RGB(30, 30, 46));
    HPEN pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    RoundRect(hdc, closeRect.left, closeRect.top, closeRect.right, closeRect.bottom, borderRadius, borderRadius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

void DesktopWindow::DrawDraggableButton(HDC hdc) {
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
