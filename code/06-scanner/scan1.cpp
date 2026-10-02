// scan1.cpp —— 扫描器阶段 1：进程枚举
// 原理：CreateToolhelp32Snapshot 给系统里所有进程拍一张"名单"，
//       然后像翻点名册一样一条条读（Process32First / Process32Next）
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <cstring>  // 给 _stricmp 用
int main() {
    // 第 1 步：拍快照
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        std::cout << "snapshot failed\n";
        return 1;
    }
    // 第 2 步：准备点名册一行（PROCESSENTRY32 是窄字符A版本）
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(pe);

    // 改用 Process32First（窄字符，不是W宽字符版）
    if(Process32First(snap, &pe))
    {
        do
        {
            std::cout << pe.th32ProcessID << "  " << pe.szExeFile << '\n';

            // _stricmp 窄字符串忽略大小写比较，去掉L前缀
            if(_stricmp(pe.szExeFile, "target.exe") == 0)
            {
                std::cout << "[+] target.exe PID = " << pe.th32ProcessID << "\n";
            }

        }while(Process32Next(snap, &pe)); // Process32Next，不带W
    }
    else
    {
        std::cout << "Process32First failed\n";
    }

    CloseHandle(snap);
    return 0;
}
