// scan10.cpp —— W8 路线B：走 PEB 三链表枚举模块（不用 psapi / CreateToolhelp32Snapshot）
#include <windows.h>
#include <iostream>
#include <cstring>
#include <wchar.h>

bool R64(HANDLE p, uintptr_t a, uint64_t& v){
    SIZE_T g=0;
    return ReadProcessMemory(p,(LPCVOID)a,&v,8,&g)&&g==8;
}
bool R32(HANDLE p, uintptr_t a, uint32_t& v){
    SIZE_T g=0;
    return ReadProcessMemory(p,(LPCVOID)a,&v,4,&g)&&g==4;
}
bool R16(HANDLE p, uintptr_t a, uint16_t& v){
    SIZE_T g=0;
    return ReadProcessMemory(p,(LPCVOID)a,&v,2,&g)&&g==2;
}
bool RBuf(HANDLE p, uintptr_t a, void* b, SIZE_T n){
    SIZE_T g=0;
    return ReadProcessMemory(p,(LPCVOID)a,b,n,&g)&&g==n;
}

typedef LONG (NTAPI *pNtQIP)(HANDLE, ULONG, PVOID, ULONG, PULONG);
struct PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
};

int main() {
    DWORD pid = 39328;   // TODO 0：填入 target2 最新 PID
    std::cout << "输入目标PID：";
    std::cin >> pid;

    HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!proc) {
        std::cout << "OpenProcess failed: " << GetLastError() << '\n';
        return 1;
    }

    // ① NtQueryInformationProcess 获取远端PEB地址
    auto NtQIP = (pNtQIP)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
    PROCESS_BASIC_INFORMATION pbi{};
    LONG ntStatus = NtQIP(proc, 0, &pbi, sizeof(pbi), nullptr);
    if(ntStatus != 0 || pbi.PebBaseAddress == nullptr)
    {
        std::cout << "NtQueryInformationProcess 失败，NTSTATUS = " << ntStatus << "\n";
        CloseHandle(proc);
        return 1;
    }

    uintptr_t peb = (uintptr_t)pbi.PebBaseAddress;
    std::cout << "远端 PEB = 0x" << std::hex << peb << std::dec << '\n';

    // TODO1：PEB + 0x18 读出 Ldr 指针
    uint64_t ldr = 0;
    if (!R64(proc, peb + 0x18, ldr)) {
        std::cout << "读取PEB->Ldr失败\n";
        CloseHandle(proc);
        return 1;
    }
    std::cout << "Ldr = 0x" << std::hex << ldr << std::dec << "\n";

    // TODO2：链表头地址 = ldr + 0x20；Ldr->InLoadOrderModuleList 双向链表头
    uintptr_t headAddr = ldr + 0x20;
    uint64_t node = 0;
    if(!R64(proc, headAddr, node))
    {
        std::cout << "读取链表头Flink失败\n";
        CloseHandle(proc);
        return 1;
    }

    std::cout << "\n==== 模块列表 ====\n";
    // TODO3：循环遍历链表，node != headAddr 持续遍历
    while(node != headAddr)
    {
        uintptr_t entry = node - 0x10; // 回退到 LDR_DATA_TABLE_ENTRY 结构体起始

        uint64_t dllBase = 0;
        uint32_t size = 0;
        uint16_t nameLen = 0;
        uint64_t nameBufPtr = 0;

        if(!R64(proc, entry + 0x30, dllBase)) break;
        if(!R32(proc, entry + 0x40, size)) break;
        if(!R16(proc, entry + 0x58, nameLen)) break;
        if(!R64(proc, entry + 0x60, nameBufPtr)) break;

        wchar_t wName[256]{};
        if(nameLen > 255*2) nameLen = 255*2;
        if(RBuf(proc, nameBufPtr, wName, nameLen))
        {
            char szName[256]{};
            WideCharToMultiByte(CP_UTF8, 0, wName, nameLen/2, szName, 255, nullptr, nullptr);
            std::cout << "Base:0x" << std::hex << dllBase
                      << " | Size:0x" << size
                      << " | Name:" << szName << "\n";
        }
        // 读取下一个节点Flink
        uint64_t nextNode = 0;
        if(!R64(proc, node, nextNode)) break;
        node = nextNode;
    }

    CloseHandle(proc);
    return 0;
}
