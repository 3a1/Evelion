#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <dwmapi.h>
#include <d3d9.h>
#include <chrono>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

bool ProcessAlive() {
    while (1) {
        auto hwnd1 = FindWindowA(NULL, "Counter-Strike");
        if (!hwnd1) {
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