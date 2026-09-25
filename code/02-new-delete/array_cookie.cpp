// array_cookie.cpp —— cookie 机制实测
// 来源：C++ 标准对 new[] cookie 的规定 + MSVC 行为实测

#include <iostream>

// 版本 A：带析构函数（非平凡析构）
struct Player {
    int hp;
    int level;
    Player() { std::cout << "Player constructed\n"; }   // ← 新增这行
    ~Player() { std::cout << "~Player\n"; }
};


// 版本 B：无析构函数（平凡析构）
struct Monster {
    int hp;
    int level;
};

int main() {
    std::cout << "==== A 组：Player（有析构）====\n";

    Player* p = new Player[5];
    std::cout << "p      = " << (void*)p << '\n';
    std::cout << "cookie = " << ((unsigned long long*)p)[-1] << '\n';
    // 预测 A1（跑之前填）：cookie = 5

    delete[] p;
    // 预测 A2（跑之前填）：~Player 打印 5行

    std::cout << "==== B 组：Monster（无析构）====\n";

    Monster* m = new Monster[5];
    std::cout << "m      = " << (void*)m << '\n';
    std::cout << "cookie = " << ((unsigned long long*)m)[-1] << '\n';
    // 预测 B（跑之前填）：cookie = 5

    delete[] m;

    return 0;
}
