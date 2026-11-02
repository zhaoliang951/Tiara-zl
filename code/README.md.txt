# 内存扫描工具集 scan1–10

编译环境：VS2022 x64 Native Tools 命令行
编译命令：cl /utf-8 /W4 /EHsc /MTd scanN.cpp

## scan1
- 功能：进程枚举，获取系统 PID 列表
- 输入/输出：无输入；控制台逐行打印所有进程 PID
- 关键 API：CreateToolhelp32Snapshot、Process32First、Process32Next

## scan2
- 功能：根据 PID 打开进程，校验句柄有效性
- 输入/输出：输入 PID；输出句柄打开结果
- 关键 API：OpenProcess、CloseHandle、GetProcessId

## scan3
- 功能：读取进程指定地址内存（单次读）
- 输入/输出：PID + 目标地址 + 长度；输出十六进制内存数据
- 关键 API：OpenProcess、ReadProcessMemory

## scan4
- 功能：扫描进程内存，查找 4 字节特征值
- 输入/输出：PID + 数值；输出所有匹配地址
- 关键 API：VirtualQueryEx、ReadProcessMemory

## scan5
- 功能：AOB 多字节特征码扫描（支持 ?? 通配）
- 输入/输出：PID + 十六进制特征码；输出匹配地址
- 关键 API：VirtualQueryEx、ReadProcessMemory

## scan6
- 功能：写内存——把目标地址的血量改写为指定值
- 输入/输出：PID + 目标地址 + 新值；输出来改写前后对照
- 关键 API：OpenProcess、ReadProcessMemory、WriteProcessMemory

## scan7
- 功能：遍历进程虚拟内存区域，配合读取做区域级搜索
- 输入/输出：PID；输出各区域基址与内容
- 关键 API：VirtualQueryEx、ReadProcessMemory

## scan8
- 功能：模块枚举——Toolhelp 快照路线
- 输入/输出：PID；输出模块名、基址、大小
- 关键 API：CreateToolhelp32Snapshot、Module32First、Module32Next

## scan9
- 功能：模块枚举——PSAPI 路线（W8 API 路线）
- 输入/输出：PID；输出模块名、基址、大小（与 scan8 比对）
- 关键 API：EnumProcessModules（psapi.lib）

## scan10
- 功能：模块枚举——PEB 三链表手解（W8 路线 B）
- 输入/输出：PID；远端 PEB → Ldr → InMemoryOrderModuleList 遍历，输出模块基址
- 关键 API：NtQueryInformationProcess、ReadProcessMemory
