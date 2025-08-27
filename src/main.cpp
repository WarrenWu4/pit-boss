#ifndef UNICODE
#define UNICODE
#endif

#include "desktop_manager.h"
#include "logger.h"
#include <iostream>
#include <windows.h>
#include <cassert>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SETTINGS 1002
#define ID_TRAY_EXIT 1001
NOTIFYICONDATA nid = {};
HMENU hTrayMenu = NULL;
HICON hIcon = NULL;

HHOOK g_hHook = NULL;
DesktopManager desktopManager;
Logger errorLog(L"build/error.log");

void cleanup() {
    UnhookWindowsHookEx(g_hHook);
    Shell_NotifyIcon(NIM_DELETE, &nid);
    DestroyMenu(hTrayMenu);
    DestroyIcon(hIcon);
    PostQuitMessage(0);
}

void hotKeyHandler(int number) {
    errorLog.LogMessageToFile(L"Hotkey pressed: " + std::to_wstring(number));
    assert(number >= 0 && number < 9);
    assert(desktopManager.isLoaded());
    desktopManager.switchToDesktop(number);
}

LRESULT CALLBACK ShortcutProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vk = p->vkCode;
        // check if win key is pressed
        bool winPressed = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
        if (winPressed && vk >= '1' && vk <= '9')
        {
            int number = vk - '0';
            hotKeyHandler(number-1); // 0-indexed
            // suppress the original windows behavior
            return 1;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TRAYICON: {
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hTrayMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            }
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                cleanup();
                return 0;
            } else if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
                return 0;
            }
            break;
        }
        case WM_DESTROY: {
            cleanup();
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // make sure desktop manager functions are loaded
    if (!desktopManager.isLoaded()) {
        errorLog.LogMessageToFile(L"Failed to load VirtualDesktopAccessor.dll");
        return 1;
    }

    // install low-level keyboard hook
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, ShortcutProc, GetModuleHandle(NULL), 0);
    if (!g_hHook) {
        errorLog.LogMessageToFile(L"Failed to install keyboard hook");
        return 1;
    } 

    // register window class
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Pit Boss";
    if (!RegisterClass(&wc)) {
        errorLog.LogMessageToFile(L"Failed to register window class");
        return 1;
    }

    // create dummy window
    HWND hwnd = CreateWindow(L"Pit Boss", L"Pit Boss", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), NULL);
    if (!hwnd) {
        errorLog.LogMessageToFile(L"Failed to create window");
        return 1;
    }

    // adding system tray
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    hIcon = (HICON)LoadImage(NULL, L"../resources/logo.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if (hIcon) {
        nid.hIcon = hIcon;
    } else {
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    wcscpy_s(nid.szTip, L"Pit Boss");
    Shell_NotifyIcon(NIM_ADD, &nid);
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_SETTINGS, L"Settings");
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    // msg loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // cleanup
    cleanup();
    return 0;
}