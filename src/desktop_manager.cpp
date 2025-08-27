#include "desktop_manager.h"
#include <windows.h>
#include <iostream>

DesktopManager::DesktopManager(const std::wstring& dllPath)
    : hDll(nullptr), loaded(false),
      GetDesktopCount(nullptr),
      GetCurrentDesktopNumber(nullptr),
      GoToDesktopNumber(nullptr) 
{
    hDll = LoadLibraryW(dllPath.c_str());
    if (!hDll) {
        std::cerr << "Failed to load " << std::string(dllPath.begin(), dllPath.end()) << "\n";
        return;
    }

    loadFunctions();
}

DesktopManager::~DesktopManager() {
    if (hDll) {
        FreeLibrary((HMODULE)hDll);
    }
}

void DesktopManager::loadFunctions() {
    GetDesktopCount = (GetDesktopCount_t)GetProcAddress((HMODULE)hDll, "GetDesktopCount");
    GetCurrentDesktopNumber = (GetCurrentDesktopNumber_t)GetProcAddress((HMODULE)hDll, "GetCurrentDesktopNumber");
    GoToDesktopNumber = (GoToDesktopNumber_t)GetProcAddress((HMODULE)hDll, "GoToDesktopNumber");

    if (GetDesktopCount && GetCurrentDesktopNumber && GoToDesktopNumber) {
        loaded = true;
    } else {
        std::cerr << "Failed to resolve function pointers from DLL.\n";
        loaded = false;
    }
}

bool DesktopManager::isLoaded() const {
    return loaded;
}

int DesktopManager::getDesktopCount() const {
    if (!loaded) return -1;
    return GetDesktopCount();
}

int DesktopManager::getCurrentDesktop() const {
    if (!loaded) return -1;
    return GetCurrentDesktopNumber();
}

int DesktopManager::switchToDesktop(int index) const {
    if (!loaded) return -1;
    return GoToDesktopNumber(index);
}
