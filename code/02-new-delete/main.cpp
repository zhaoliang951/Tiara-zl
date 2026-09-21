#include <iostream>

struct Player {
    int hp;
    int level;
};

int main() {
    // TODO 1：循环new 5次，保存指针，打印地址
    Player* arr[5];
    for(int i = 0; i < 5; i++){
        Player* p = new Player;
        arr[i] = p;
        std::cout << (void*)p << '\n';
    }

    // TODO 2：delete这5个对象
    for(int i = 0; i < 5; i++){
        delete arr[i];
    }

    // TODO 3：再new一个Player，打印地址，对比上面5个
    Player* p_new = new Player;
    std::cout << "新对象地址：" << (void*)p_new << '\n';

    delete p_new;
    return 0;
}
