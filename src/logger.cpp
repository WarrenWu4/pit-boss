#include "logger.h"

Logger::Logger(const std::wstring& filePath, std::size_t maxFileSize) : filePath(filePath), maxFileSize(maxFileSize) {
    file.open(filePath.c_str(), std::ios::app);
    if (!file.is_open()) {
        DWORD errorCode = GetLastError();
        PrintErrorAndExit(L"Failed to open log file", errorCode);
    }
}

Logger::~Logger() {
    if (file.is_open()) {
        file.close();
    }
}

void Logger::LogMessageToFile(const std::wstring& message) {
    if (file.is_open()) {
        /*
        std::size_t currentSize = GetFileSize();
        if (currentSize >= maxFileSize) {
            file.close();
            RotateFile();
        }
        */
        file << message << std::endl;
        file.flush();
    }
}

std::size_t Logger::GetFileSize() {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesExW(filePath.c_str(), GetFileExInfoStandard, &fileInfo)) {
        LARGE_INTEGER size;
        size.HighPart = fileInfo.nFileSizeHigh;
        size.LowPart  = fileInfo.nFileSizeLow;
        return static_cast<std::size_t>(size.QuadPart);
    }
    return 0;
}

void Logger::TruncateStart(std::size_t bytes) {
    std::ifstream in(filePath.c_str(), std::ios::binary);
    in.seekg(bytes);
    std::string remaining((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    std::ofstream out(filePath.c_str(), std::ios::binary | std::ios::trunc);
    out.write(remaining.data(), remaining.size());
    out.close();
}


void Logger::PrintErrorAndExit(const std::wstring& error, DWORD errorCode) {
    LPVOID lpMsgBuf;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        0,
        (LPWSTR)&lpMsgBuf,
        0,
        NULL
    );
    std::wcerr << error << L"\n"
        << L"WinAPI Error (" << errorCode << L"): " << (LPWSTR)lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
    ExitProcess(errorCode);
}

