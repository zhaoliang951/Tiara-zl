// scanner.cpp — W5 阶段1：枚举进程找 target.exe 的 PID
// 坑位记录：wprintf 输出中文必须 setlocale(".UTF8")，否则默认 "C" locale
//           转换失败整行断流；chcp 65001 只管控制台解码，救不了 CRT 转换。
#include <windows.h>
#include <tlhelp32.h>
#include <wchar.h>
#include <stdio.h>
#include <locale.h>

int main()
{
    setlocale(LC_ALL, ".UTF8");   // 宽字符输出三件套之一，缺了中文断流

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        wprintf(L"CreateToolhelp32Snapshot failed\n");
        return 1;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    BOOL ret = Process32FirstW(hSnap, &pe);
    if (ret)
    {
        do {
            if (wcscmp(pe.szExeFile, L"target.exe") == 0) {
                wprintf(L"[+] 找到 target.exe, PID = %lu\n", pe.th32ProcessID);
                CloseHandle(hSnap);
                return 0;
            }
        } while (Process32NextW(hSnap, &pe));
    }

    CloseHandle(hSnap);
    wprintf(L"[-] 未找到 target.exe\n");
    return 2;
}

