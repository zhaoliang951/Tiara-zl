// scan5.cpp —— 阶段3第二步（升级版）：双快照对比，抓"值减少10"的地址
#include <windows.h>
#include <vector>
#include <iostream>
#include <cstring>

struct Region {
    uintptr_t base;
    SIZE_T size;
    std::vector<BYTE> data;
};

std::vector<Region> TakeSnapshot(HANDLE proc) {
    std::vector<Region> snap;
    MEMORY_BASIC_INFORMATION mbi{};
    LPCVOID base = nullptr;
    while (base < (LPCVOID)0x7FFFFFFFFFFF) {
        if (VirtualQueryEx(proc, base, &mbi, sizeof(mbi)) == 0) break;
        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
             mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE)) {
            Region r;
            r.base = (uintptr_t)mbi.BaseAddress;
            r.size = mbi.RegionSize;
            r.data.resize(r.size);
            SIZE_T got = 0;
            if (ReadProcessMemory(proc, mbi.BaseAddress, r.data.data(), r.size, &got) && got == r.size) {
                snap.push_back(std::move(r));
            }
        }
        base = (LPCVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
    }
    return snap;
}

int main() {
    DWORD pid = 41040;   // TODO：tasklist 查最新 PID 填这里

    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!proc) { std::cout << "OpenProcess failed: " << GetLastError() << '\n'; return 1; }

    std::cout << "拍第一张快照...\n";
    auto snap1 = TakeSnapshot(proc);
    std::cout << "区域数 " << snap1.size() << "，等 6 秒...\n";
    Sleep(6000);

    std::cout << "拍第二张快照并对比...\n";
    int found = 0;
    for (auto& r : snap1) {
        std::vector<BYTE> now(r.size);
        SIZE_T got = 0;
        if (!ReadProcessMemory(proc, (LPCVOID)r.base, now.data(), r.size, &got) || got != r.size)
            continue;

        // ========== 填入的比对循环 ==========
        for (SIZE_T i = 0; i + 4 <= r.size; i += 4)
        {
            int old_v, now_v;
            memcpy(&old_v, &r.data[i], 4);
            memcpy(&now_v, &now[i], 4);
            if (now_v != old_v)                                  // ← 只看"变没变"
            {
                uintptr_t addr = r.base + i;
                std::cout << "地址:0x" << std::hex << addr
                          << std::dec                                // ← 打数值前切回十进制
                          << "  旧:" << old_v << " 新:" << now_v
                          << "  变化:" << (now_v - old_v) << '\n';
                found++;
            }
        }

        // =====================================
    }

    std::cout << "\n找到 " << found << " 个值减少了 10 的地址\n";
    CloseHandle(proc);
    return 0;
}
