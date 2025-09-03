#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
#include <cassert>

#include "desktop_manager.h"
#include "logger.h"
#include "settings_window.h"
#include "resource.h"
#include "icon_manager.h"
#include "desktop_window.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SETTINGS 1002
#define ID_TRAY_EXIT 1001

IconManager* iconManager = nullptr;
SettingsWindow* settingsWindow = nullptr;
DesktopWindow* desktopWindow = nullptr;
DesktopManager* desktopManager = nullptr;
Logger errorLog(L"build/error.log");

HHOOK g_hHook = NULL;

void cleanup() {
    UnhookWindowsHookEx(g_hHook);
    PostQuitMessage(0);
    if (desktopManager) {
        delete desktopManager;
        desktopManager = nullptr;
    }
    if (iconManager) {
        delete iconManager;
        iconManager = nullptr;
    }
    if (settingsWindow) {
        delete settingsWindow;
        settingsWindow = nullptr;
    }
    if (desktopWindow) {
        delete desktopWindow;
        desktopWindow = nullptr;
    }
}

void hotKeyHandler(int number) {
    errorLog.LogMessageToFile(L"Hotkey pressed: " + std::to_wstring(number));
    assert(number >= 0 && number < 9);
    assert(desktopManager && desktopManager->isLoaded());
    desktopManager->switchToDesktop(number);
    desktopWindow->setCurrentDesktopIndex(number);
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
                TrackPopupMenu(iconManager->hTrayMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            }
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                cleanup();
                return 0;
            } else if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
                if (settingsWindow) {
                    settingsWindow->Show(hwnd);
                }
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
    iconManager = new IconManager(hInstance, hwnd);
    iconManager->InitTray();

    // TODO: change the timing for when settings window is created
    // create settings window when the settings button is clicked
    // this way it will save both time and memory
    settingsWindow = new SettingsWindow(hInstance);
    desktopWindow = new DesktopWindow(hInstance);
    desktopManager = new DesktopManager();
    // make sure desktop manager functions are loaded
    if (desktopManager == nullptr || !desktopManager->isLoaded()) {
        errorLog.LogMessageToFile(L"Failed to load VirtualDesktopAccessor.dll");
        return 1;
    }
    int current = desktopManager->getCurrentDesktop();
    int desktops = desktopManager->getDesktopCount();
    std::vector<std::wstring> names;
    for (int i = 0; i < desktops; i++) {
        names.push_back(std::to_wstring(i + 1));
    }
    desktopWindow->setDesktopNames(names);
    desktopWindow->setCurrentDesktopIndex(current);
    errorLog.LogMessageToFile(L"Current desktop: " + std::to_wstring(current) + L", Total desktops: " + std::to_wstring(desktops));

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