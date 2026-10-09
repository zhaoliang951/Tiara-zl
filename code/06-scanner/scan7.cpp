// scan7.cpp —— 指针搜索：谁的内存里存着目标地址？
#include <windows.h>
#include <vector>
#include <iostream>
#include <cstring>

int main()
{
    DWORD pid = 41040;
    uintptr_t hpAddr = 0x1e42afb8360;    // 想找的目标（World* 或 hp 地址，换着用）

    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!proc) { std::cout << "OpenProcess failed: " << GetLastError() << '\n'; return 1; }

    MEMORY_BASIC_INFORMATION mbi{};
    LPCVOID base = nullptr;
    int found = 0;

    while (base < (LPCVOID)0x7FFFFFFFFFFF) {
        if (VirtualQueryEx(proc, base, &mbi, sizeof(mbi)) == 0) break;

        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
             mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE)) {

            std::vector<BYTE> buf(mbi.RegionSize);
            SIZE_T got = 0;
            if (ReadProcessMemory(proc, mbi.BaseAddress, buf.data(), mbi.RegionSize, &got) && got == mbi.RegionSize) {
                for (SIZE_T i = 0; i + 8 <= mbi.RegionSize; i += 8) {
                    uint64_t v = 0;
                    memcpy(&v, buf.data() + i, 8);
                    if (v == (uint64_t)hpAddr) {
                        uintptr_t holder = (uintptr_t)mbi.BaseAddress + i;
                        std::cout << "持有者: 0x" << std::hex << holder
                                  << "   区域起始: 0x" << (uintptr_t)mbi.BaseAddress
                                  << "   分配基址: 0x" << (uintptr_t)mbi.AllocationBase
                                  << "   类型: 0x" << mbi.Type
                                  << std::dec << '\n';
                        found++;
                    }
                }
            }
        }
        base = (LPCVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
    }

    std::cout << "\n共 " << found << " 个位置存着 0x" << std::hex << hpAddr << std::dec << '\n';
    CloseHandle(proc);
    return 0;
}
