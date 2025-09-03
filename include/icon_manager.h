#pragma once

#include <windows.h>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SETTINGS 1002
#define ID_TRAY_EXIT 1001

class IconManager {
    private:
        HINSTANCE hInstance;
        HWND hwnd;
    public:
        IconManager(HINSTANCE hInstance, HWND hwnd);
        ~IconManager();
        // system tray stuff
        void InitTray();
        HICON trayHIcon;
        NOTIFYICONDATA trayNid;
        HMENU hTrayMenu;
};