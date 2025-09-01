#include "icon_manager.h"

IconManager::IconManager(HINSTANCE hInstance, HWND hwnd): hInstance(hInstance), hwnd(hwnd) {}

IconManager::~IconManager() {
    Shell_NotifyIcon(NIM_DELETE, &trayNid);
    if (trayHIcon) {
        DestroyIcon(trayHIcon);
    }
    if (hTrayMenu) {
        DestroyMenu(hTrayMenu);
    }
}

void IconManager::InitTray() {
    trayNid.cbSize = sizeof(NOTIFYICONDATA);
    trayNid.hWnd = hwnd;
    trayNid.uID = 1;
    trayNid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    trayNid.uCallbackMessage = WM_TRAYICON;
    trayHIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    if (trayHIcon) {
        trayNid.hIcon = trayHIcon;
    } else {
        trayNid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    wcscpy_s(trayNid.szTip, L"Pit Boss");
    Shell_NotifyIcon(NIM_ADD, &trayNid);
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_SETTINGS, L"Settings");
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
}