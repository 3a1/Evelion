#pragma once
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <dwmapi.h>
#include <d3d9.h>
#include <chrono>
#include <thread>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    const int bufferSize = 256;
    char buffer[bufferSize];

    if (GetWindowTextA(hwnd, buffer, bufferSize) > 0) {
        if (strstr(buffer, "Counter") != nullptr && strstr(buffer, "Strike") != nullptr) {

            *reinterpret_cast<HWND*>(lParam) = hwnd;
            return FALSE;
        }
    }

    return TRUE;
}

HWND GetProcessHwnd() {

    HWND hwnd = nullptr;

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&hwnd));

    if (hwnd != nullptr) {
        return hwnd;
    }
    else {
        MessageBox(nullptr, "Get ProcessHwnd error.", "Evelion", MB_OK);
        exit(0);
    }
}

bool IsProcessAlive(const char* processName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hProcessSnap);
                return true;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return false;
}

bool ProcessAlive() {
    while (1) {

        if (!IsProcessAlive("hl.exe")) {
            MessageBox(nullptr, "Game is closed. Please restart Evelion.", "Evelion", MB_OK);
            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}                                                                                                            

std::string RandomString(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}