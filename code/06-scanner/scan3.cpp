// scan3.cpp —— 阶段3第一步：读内存 + 按值过滤
// 前置：靶子跑起来，tasklist 查最新 PID 填进下面
#include <windows.h>
#include <vector>
#include <iostream>
#include <cstring> // memcpy

int main()
{
    DWORD pid = 90852;    // 填入PID
    int target = 100;     // 当前屏幕上打印的血量

    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!proc) {
        std::cout << "OpenProcess failed: " << GetLastError() << '\n';
        return 1;
    }

    MEMORY_BASIC_INFORMATION mbi{};
    LPCVOID base = nullptr;
    size_t hitCount = 0;
    std::vector<uintptr_t> hits;

    while (base < (LPCVOID)0x7FFFFFFFFFFF)
    {
        if (VirtualQueryEx(proc, base, &mbi, sizeof(mbi)) == 0)
            break;

        // 只处理"已提交 + 可读"的区域（沿用 scan2 的判断）
        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
             mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE))
        {
            // TODO 1：申请一块 mbi.RegionSize 字节的缓冲区（std::vector<BYTE>）
            std::vector<BYTE> buf(mbi.RegionSize);
            SIZE_T read = 0;

            // TODO 2：用 ReadProcessMemory 把这一整块读进来
            BOOL ret = ReadProcessMemory(proc, mbi.BaseAddress, buf.data(), mbi.RegionSize, &read);
            if (!ret || read == 0)
            {
                // 读失败，跳过这块内存区域
                base = (LPCVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
                continue;
            }

            // TODO 3：把这块缓冲区按 4 字节步长走一遍
            // 缓冲到 RegionSize - 4 收尾，别越界
            for (SIZE_T i = 0; i <= read - 4; i += 4)
            {
                int v;
                memcpy(&v, buf.data() + i, 4);
                if (v == target)
                {
                    uintptr_t absAddr = (uintptr_t)mbi.BaseAddress + i;
                    hits.push_back(absAddr);
                }
            }
        }

        base = (LPCVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
    }

    std::cout << "====== 结果 ======\n";
    std::cout << "内存里等于 " << target << " 的位置共 " << hits.size() << " 个\n";
    // TODO 4：打印前 20 个地址（超出就省略）
    size_t printMax = 20;
    for (size_t i = 0; i < hits.size() && i < printMax; ++i)
    {
        std::cout << "0x" << std::hex << hits[i] << '\n';
    }
    if (hits.size() > printMax)
    {
        std::cout << "...还有更多地址省略\n";
    }

    CloseHandle(proc);
    return 0;
}
