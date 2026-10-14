// scan9.cpp —— W8 路线A：用 API 枚举目标进程的模块
#include <windows.h>
#include <psapi.h>      // EnumProcessModulesEx / GetModuleInformation 都在这
#include <iostream>

int main()
{
    DWORD pid = 8732;      // ← 填 target2.exe 的最新 PID（tasklist | findstr target2）

    HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!proc) {
        std::cout << "OpenProcess failed: " << GetLastError() << '\n';
        return 1;
    }

    HMODULE mods[512]{};
    DWORD needed = 0;
    if (!EnumProcessModulesEx(proc, mods, sizeof(mods), &needed, LIST_MODULES_ALL)) {
        std::cout << "EnumProcessModulesEx failed: " << GetLastError() << '\n';
        CloseHandle(proc);
        return 1;
    }

    int n = needed / sizeof(HMODULE);
    std::cout << "共 " << n << " 个模块：\n\n";
    for (int i = 0; i < n; i++) {
        char name[MAX_PATH]{};
        GetModuleBaseNameA(proc, mods[i], name, sizeof(name));

        MODULEINFO mi{};
        GetModuleInformation(proc, mods[i], &mi, sizeof(mi));

        std::cout << name
                  << "   基址=0x" << std::hex << (uintptr_t)mi.lpBaseOfDll
                  << "   大小=0x" << mi.SizeOfImage << std::dec << '\n';
    }

    CloseHandle(proc);
    return 0;
}
## 为什么「模块基址+偏移」抗重启、抗更新（W6）
1) **纯偏移失效的根因**：如果偏移是相对堆/栈这类动态区域的基址，那些基址每次运行都随机
 — 实测证据：hp 地址 A1=0x39dd6ff980，重启后 A2=0xe797f5fcd0，完全不同。
所以纯偏移**每次启动就失效**，这和“程序更新”无关。

2) **「模块基址+偏移」抗重启**：模块基址每次可以从目标进程重新获取
 — 实测：10/10 模块基址 0x7ff635a70000，10/14 为 0x7ff7fd9b0000（跨会话 ASLR）。
而模块内偏移由**编译器+链接器在构建期固定**，同一二进制每次运行都不变。
所以：基址实时取 + 偏移写死，抗重启。

3) **抗更新是另一层**：从全局对象出发的链，小版本更新后通常只需微调少量内层偏移；
而硬编码绝对地址/深链偏移在更新后必须重新扫描。

## 权限位对照表（W7）
| 权限位 | 管什么 | 何时必须加 | 报错指纹（缺它时） |
|------|------|----------|----------------|
| PROCESS_VM_READ | 读目标内存(ReadProcessMemory) | 需要读时必须加 | RPM 失败，err = 5（拒绝访问） |
| PROCESS_VM_WRITE | 写目标内存(WriteProcessMemory) | 需要改值时 | WPM 失败，err = 5 |
| PROCESS_VM_OPERATION | 改目标内存属性(VirtualAllocEx/VirtualFreeEx等) | 分配/释放/改保护属性时 | WPM 常一并失败，err = 5 |
| PROCESS_QUERY_INFORMATION | 查询进程信息(VirtualQueryEx等) | **内存布局遍历（画地图）** | VirtualQueryEx 返回0（我们那次） |
| （PID不存在，非权限问题） | — | — | OpenProcess 失败, **err = 87**(参数无效) |
