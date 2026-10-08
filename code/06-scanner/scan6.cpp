// scan6.cpp —— 阶段4：改写血量
// 目标：把 target.exe 的 hp 改成 999，看它打印反应
#include <windows.h>
#include <iostream>
#include <cstring>

int main()
{
    DWORD pid = 10060;                    // 最新 PID
    uintptr_t hpAddr = 0x1000ffbc4;       // scan5 抓到的地址
    int newHp = 999;                      // 想改成多少

    // 写内存需要新权限：VM_WRITE（写的权力）+ VM_OPERATION（改内存状态的权力）
    HANDLE proc = OpenProcess(
        PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
        FALSE, pid);
    if (!proc) {
        std::cout << "OpenProcess failed: " << GetLastError() << '\n';
        return 1;
    }

    // 改之前先读一眼（作对照）
    int before = 0;
    SIZE_T got = 0;
    if (ReadProcessMemory(proc, (LPCVOID)hpAddr, &before, sizeof(before), &got))
        std::cout << "改之前的值 = " << before << '\n';

    // 写入 999
    SIZE_T wrote = 0;
    if (WriteProcessMemory(proc, (LPVOID)hpAddr, &newHp, sizeof(newHp), &wrote))
        std::cout << "写入成功，写了 " << wrote << " 字节\n";
    else
        std::cout << "写入失败，err = " << GetLastError() << '\n';

    // 再读回来验证
    int after = 0;
    if (ReadProcessMemory(proc, (LPCVOID)hpAddr, &after, sizeof(after), &got))
        std::cout << "改之后的值 = " << after << '\n';

    CloseHandle(proc);
    return 0;
}
