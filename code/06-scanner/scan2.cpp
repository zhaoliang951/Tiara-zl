// scan2.cpp —— 扫描器阶段 2：画内存地图
// 三个新概念：
//   MEM_COMMIT  = 这块区域已提交（真实占用着，不是预订）
//   PAGE_*      = 这块区域的保护属性（可读/可读写/只执行……）
//   State/Protect 组合决定"这间房能不能进"
#include <windows.h>
#include <iostream>

int main()
{
    DWORD pid = 64604;

    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (proc == NULL) {
        std::cout << "OpenProcess failed, err = " << GetLastError() << '\n';
        return 1;
    }
    std::cout << "opened pid " << pid << '\n';

    MEMORY_BASIC_INFORMATION mbi{};
    LPCVOID baseAddress = nullptr;
    SIZE_T readableCount = 0;
    SIZE_T readableTotalBytes = 0;

    while (baseAddress < (LPCVOID)0x7FFFFFFFFFFF)
    {
        SIZE_T ret = VirtualQueryEx(proc, baseAddress, &mbi, sizeof(mbi));
        if (ret == 0)
        {
            std::cout << "VirtualQueryEx failed at " << baseAddress
                      << ", err = " << GetLastError() << '\n';
            break;
        }


       if (mbi.State == MEM_COMMIT)
        {
            // 判断是否可读：PAGE_READONLY / PAGE_READWRITE / PAGE_EXECUTE_READ / PAGE_EXECUTE_READWRITE
            bool isReadable = false;
         if (mbi.Protect == PAGE_READONLY ||
                mbi.Protect == PAGE_READWRITE ||
                mbi.Protect == PAGE_EXECUTE_READ ||
                mbi.Protect == PAGE_EXECUTE_READWRITE)
            {
                isReadable = true;
            }

            if (isReadable)
            {
                readableCount++;
                readableTotalBytes += mbi.RegionSize;
                std::cout << "Base: " << (void*)mbi.BaseAddress
                          << "\tSize: " << mbi.RegionSize
                          << "\tProtect: 0x" << std::hex << mbi.Protect << std::dec
                          << '\n';
            }
        }

        baseAddress = (LPCVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
    }

    std::cout << "\n===== Statistic =====\n";
    std::cout << "Total readable committed regions: " << readableCount << '\n';
    std::cout << "Total readable bytes: " << readableTotalBytes << '\n';

    CloseHandle(proc);
    return 0;
}
