#pragma once
#include <string>

class DesktopManager {
public:
    DesktopManager(const std::wstring& dllPath = L"../external/VirtualDesktopAccessor.dll");
    ~DesktopManager();

    bool isLoaded() const;

    int getDesktopCount() const;
    int getCurrentDesktop() const;
    int switchToDesktop(int index) const;
    int getDesktopName(int index, std::wstring& name, int maxLength) const;

private:
    void loadFunctions();

    void* hDll;
    bool loaded;

    // Function pointers from DLL
    typedef int(__cdecl* GetDesktopCount_t)();
    typedef int(__cdecl* GetCurrentDesktopNumber_t)();
    typedef int(__cdecl* GoToDesktopNumber_t)(int);
    typedef int(__cdecl* GetDesktopName_t)(int, char*, size_t);

    GetDesktopCount_t GetDesktopCount;
    GetCurrentDesktopNumber_t GetCurrentDesktopNumber;
    GoToDesktopNumber_t GoToDesktopNumber;
    GetDesktopName_t GetDesktopName;
};
