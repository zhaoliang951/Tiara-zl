// 来源：Microsoft Learn《CRT Debug Heap Details》+ 豆包查询 _CrtDumpMemoryLeaks 用法
#define _CRTDBG_MAP_ALLOC    // 必须放在所有include之前
#include <iostream>
#include <crtdbg.h>

struct Player {
    int hp;
    int level;
};

int main() {
    // 报告默认走调试通道，重定向到 stdout 才能在控制台看到
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

    // TODO: new 一个Player，初始化hp=100，level=1，打印地址
    // _CrtSetBreakAlloc(160);   // 断点已完成使命，注释掉——留着它每次 F5 都会停下
    Player* p = new Player{100, 1};
    std::cout << (void*)p << '\n';
    // 故意不delete，制造内存泄漏

    // 程序退出前把泄漏报告倒出来（Debug CRT的泄漏检测器）
    delete p;    // 先释放
    // 程序退出前把泄漏报告倒出来（Debug CRT的泄漏检测器）
    _CrtDumpMemoryLeaks();   // 再清点——此时应两手空空
    return 0;
}