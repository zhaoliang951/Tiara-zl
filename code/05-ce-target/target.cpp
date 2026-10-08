#include <iostream>
#include <windows.h>

int main() {
    int hp = 100;
    int tick = 0;
    while (true) {
        std::cout << "hp = " << hp << '\n';
        Sleep(1000); // 暂停1000ms = 1秒
        tick++;
        if (tick % 5 == 0) hp -= 10; // 每5秒hp减少10，制造数值变化
notepad code\05-ce-target\target.cpp    
    }
}
