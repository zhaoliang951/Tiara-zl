// scan8.cpp —— W6 链跟随器：模块基址 + 偏移 → 三级链 → hp
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <cstring>

// 两个读内存的小工具
bool Read64(HANDLE proc, uintptr_t addr, uint64_t& out) {
    SIZE_T got = 0;
    return ReadProcessMemory(proc, (LPCVOID)addr, &out, 8, &got) && got == 8;
}
bool Read32(HANDLE proc, uintptr_t addr, int& out) {
    SIZE_T got = 0;
    return ReadProcessMemory(proc, (LPCVOID)addr, &out, 4, &got) && got == 4;
}

int main() {
    DWORD pid = 12352;                 // TODO 0：tasklist 查最新 target2 PID（注意是 target2.exe！）
    const uintptr_t OFF_WORLD  = 0x42270;   // g_world 相对模块基址的偏移
    const uintptr_t OFF_HP     = 0x0;       // hp 相对 Player 的偏移

    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!proc) { 
        std::cout << "OpenProcess failed: " << GetLastError() << '\n'; 
        return 1; 
    }

    // ===== 第①步：模块快照，找 target2.exe 的基址 =====
    uintptr_t moduleBase = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me{};
        me.dwSize = sizeof(me);
        if (Module32First(snap, &me)) {
            do {
                // TODO1：ANSI字符串对比，_stricmp 大小写不敏感
                if (_stricmp(me.szModule, "target2.exe") == 0)
                {
                    moduleBase = (uintptr_t)me.modBaseAddr;
                    break;
                }
            } while (Module32Next(snap, &me));
        }
        CloseHandle(snap);
    }
    if (!moduleBase) { 
        std::cout << "没找到 target2.exe 模块\n"; 
        CloseHandle(proc);
        return 1; 
    }
    std::cout << "模块基址 = 0x" << std::hex << moduleBase << std::dec << '\n';

    // ===== 第②步：跟随三级链 =====
    uint64_t world = 0;
    uint64_t player = 0;
    int hp = 0;

    uintptr_t addrWorld = moduleBase + OFF_WORLD;
    if (!Read64(proc, addrWorld, world))
    {
        std::cout << "读取World指针失败，Err:" << GetLastError() << '\n';
        CloseHandle(proc);
        return 1;
    }

    uintptr_t addrPlayer = world + 0;
    if (!Read64(proc, addrPlayer, player))
    {
        std::cout << "读取Player指针失败，Err:" << GetLastError() << '\n';
        CloseHandle(proc);
        return 1;
    }

    uintptr_t addrHp = player + OFF_HP;
    if (!Read32(proc, addrHp, hp))
    {
        std::cout << "读取HP失败，Err:" << GetLastError() << '\n';
        CloseHandle(proc);
        return 1;
    }

    // TODO3：打印三级结果
    std::cout << "World* = 0x" << std::hex << world << std::dec << '\n';
    std::cout << "Player* = 0x" << std::hex << player << std::dec << '\n';
    std::cout << "HP = " << hp << '\n';

    CloseHandle(proc);
    return 0;
}
