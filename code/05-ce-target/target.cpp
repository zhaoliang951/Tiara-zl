// W5 靶子：hp = 100，每秒打印一次
// 自写扫描器的任务：不看代码、不看这里任何地址，纯靠读内存找到 hp 并改掉它
#include <iostream>
#include <windows.h>

int main() {
    int hp = 100;   // ← 扫描器要找的就是这个
    while (true) {
        std::cout << "hp = " << hp << '\n';
        Sleep(1000);
    }
    return 0;
}
