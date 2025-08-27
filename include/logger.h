#pragma once

#include <vector>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

class Logger {
private:
    std::wofstream file;
    std::wstring filePath;
    std::size_t maxFileSize;

    std::size_t GetFileSize();
    void TruncateStart(std::size_t bytes);
    void PrintErrorAndExit(const std::wstring& error, DWORD errorCode);
public:
    // 10KB default size
    Logger(const std::wstring& filePath, std::size_t maxSize = 10240);
    ~Logger();
    
    void LogMessageToFile(const std::wstring& message);
};

class FixedSizeLogger { 
private:
    std::fstream file;
    std::wstring filename;
    std::size_t maxFileSize;

    void TruncateStart(std::size_t bytes) {
        file.seekg(bytes, std::ios::beg);
        std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        file.open(filename.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        file.write(buffer.data(), buffer.size());
        file.seekg(0, std::ios::beg);
        file.close();
        file.open(filename.c_str(), std::ios::binary | std::ios::in | std::ios::out);
    }
public:
  FixedSizeLogger(const std::wstring& filename, std::size_t maxSize) {
    file.open(filename.c_str(), std::ios::binary | std::ios::out);
    file.close();
    file.open(filename.c_str(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
      throw std::runtime_error("Constructor cannot open file");
    }
    // set variables to correct values
    this->filename = filename;
    this->maxFileSize= maxSize;
  }
  
  ~FixedSizeLogger() {
    if (file.is_open()) {
      file.close();
    }
  }

  void write(const std::wstring& data) {
    std::size_t dataSize = data.size();
    std::size_t currentSize = file.tellg();
    if (currentSize + dataSize > maxFileSize) {
      std::size_t excess = (currentSize + dataSize) - maxFileSize;
      TruncateStart(excess);
    }
    file.write(reinterpret_cast<const char*>(data.c_str()), data.size() * sizeof(wchar_t));
  }

};
