#include "desktop_manager.h"
#include <windows.h>
#include <iostream>

DesktopManager::DesktopManager(const std::wstring& dllPath)
    : hDll(nullptr), loaded(false),
      GetDesktopCount(nullptr),
      GetCurrentDesktopNumber(nullptr),
      GoToDesktopNumber(nullptr),
      GetDesktopName(nullptr)
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
    GetDesktopName = (GetDesktopName_t)GetProcAddress((HMODULE)hDll, "GetDesktopName");

    if (GetDesktopCount && GetCurrentDesktopNumber && GoToDesktopNumber && GetDesktopName) {
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
    if (!loaded) { return -2; }
    int result = GoToDesktopNumber(index);
    if (result == -1)
    {
        // Check GetLastError, even though the DLL doesn’t always set it
        DWORD err = GetLastError();
        wchar_t buf[512];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, err, 0, buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
        MessageBoxW(NULL, buf, L"GoToDesktopNumber failed", MB_OK | MB_ICONERROR);
    }
    return result;
}

int DesktopManager::getDesktopName(int index, std::wstring& name, int maxLength) const {
    if (!loaded) { return -1; }
    char* buffer = new char[maxLength];
    int result = GetDesktopName(index, buffer, maxLength);
    if (result > 0) {
        int needed = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, nullptr, 0);
        if (needed <= 0)
        {
            throw std::runtime_error("UTF-8 conversion failed");
        }
        std::wstring wstr(needed, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, buffer, -1, &wstr[0], needed);
        if (!wstr.empty() && wstr.back() == L'\0')
        {
            wstr.pop_back();
        }
        name = wstr;
    }
    delete[] buffer;
    return result;
}
