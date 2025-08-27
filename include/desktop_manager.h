#pragma once
#include <string>

class DesktopManager {
public:
    DesktopManager(const std::wstring& dllPath = L"VirtualDesktopAccessor.dll");
    ~DesktopManager();

    bool isLoaded() const;

    int getDesktopCount() const;
    int getCurrentDesktop() const;
    bool switchToDesktop(int index) const;

private:
    void loadFunctions();

    void* hDll;
    bool loaded;

    // Function pointers from DLL
    typedef int(__cdecl* GetDesktopCount_t)();
    typedef int(__cdecl* GetCurrentDesktopNumber_t)();
    typedef void(__cdecl* GoToDesktopNumber_t)(int);

    GetDesktopCount_t GetDesktopCount;
    GetCurrentDesktopNumber_t GetCurrentDesktopNumber;
    GoToDesktopNumber_t GoToDesktopNumber;
};
