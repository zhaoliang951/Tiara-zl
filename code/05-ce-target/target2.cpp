// target2.cpp —— W6 靶子 v2：World -> Player -> hp（三级结构）
// 设计要点：
//   ① g_world 是全局变量，住在模块的 .data 段 → 地址 = 模块基址 + 固定偏移（重启也遵循这个规律）
//   ② Player 是 new 出来的堆对象 → 地址每次都变（和 hp 一样漂）
//   ③ 指针链：g_world → World.player → Player.hp
#include <iostream>
#include <windows.h>

struct Player {
    int hp;       // 偏移 +0
    int level;    // 偏移 +4
};

struct World {
    Player* player;   // 偏移 +0
};

World*  g_world  = nullptr;   // 全局：住 .data（模块区）
Player* g_player = nullptr;   // 全局：住 .data（模块区）

int main() {
    g_player = new Player{100, 1};        // 堆对象
    g_world  = new World{ g_player };     // 堆对象，里面存着 player 的指针

    int tick = 0;
    while (true) {
        std::cout << "hp = " << g_player->hp << '\n';
        Sleep(1000);
        tick++;
        if (tick % 5 == 0) g_player->hp -= 10;
        if (g_player->hp <= 0) g_player->hp = 100;
    }
}
