// ============================================================
// code/01-memory-toolkit/main.cpp
// S1-W1 骨架：进程虚拟地址空间六区证据（计划 §S1/W1，附录 C）
//
// 构建（二选一）：
//   MinGW : g++ -std=c++17 -Wall -Wextra -Wpedantic -O0 main.cpp -o main.exe
//   MSVC  : cl /W4 /Od /EHsc main.cpp
//
// 验收锚点（计划 W1）：
//   ① 能打印 .text/.rdata/.data/.bss/栈/堆 六类地址
//   ② 能解释"栈为什么向低地址增长"（说到 push/ABI）
//   ③ 明确说出"绝对高低由 ASLR 决定，我不记结论"
//   ④ 画出内存四区图
//
// TODO(learner)：运行后在每行输出旁手写 3 句解释（D3 作业），
//               不懂哪一行，先猜再问，禁止跳过。
// ============================================================

#include <cstdio>
#include <cstdint>

// ---- .text（代码区）：函数体住在这里 ----
static int add(int a, int b) { return a + b; }

// ---- .rdata（只读数据）：字符串字面量本体住在这里 ----
// 注意区分：g_msg_ptr（指针变量本身）在 .data，
//           它指向的 "hello from .rdata"（字符串本体）在 .rdata。
//           这是零基础最常混淆的一处，下面会分别打印两个地址。
static const char* g_msg_ptr = "hello from .rdata";

// ---- .bss（未初始化数据）：没有初值的全局变量 ----
static long g_bss_counter;

// ---- 递归探栈：每次调用，局部变量应该逐层"变小"（地址数值上） ----
static void probe_stack(int depth) {
    int local = depth;
    std::printf("  [stack] depth=%d  &local=%p\n", depth, (void*)&local);
    if (depth < 4) {
        probe_stack(depth + 1);
    }
}

int main() {
    // ① 代码区：取函数地址（标准禁止对 main 取址，所以用 add）
    std::printf("code   &add              = %p\n", (void*)&add);

    // ② 只读数据区：字符串字面量本体
    std::printf(".rdata string-literal     = %p\n",
                 static_cast<const void*>(g_msg_ptr));

    // ③ 已初始化数据区：指针变量本身（不是它指向的东西！）
    std::printf(".data  &g_msg_ptr         = %p\n", (void*)&g_msg_ptr);

    // ④ 未初始化数据区
    std::printf(".bss   &g_bss_counter     = %p\n", (void*)&g_bss_counter);

    // ⑤ 栈：main 自己的局部变量
    int stack_var = 0;
    std::printf("stack  &stack_var         = %p\n", (void*)&stack_var);

    std::printf("---- 递归探栈：看 &local 是不是逐层变小 ----\n");
    probe_stack(1);

    // ⑥ 堆：连续 5 次 new，检查地址是否严格递增
    void* blocks[5];
    for (int i = 0; i < 5; ++i) {
        blocks[i] = new char[64];
    }

    bool strictly_increasing = true;
    for (int i = 1; i < 5; ++i) {
        if (reinterpret_cast<std::uintptr_t>(blocks[i])
            <= reinterpret_cast<std::uintptr_t>(blocks[i - 1])) {
            strictly_increasing = false;
        }
    }
    for (int i = 0; i < 5; ++i) {
        std::printf("heap   new#%d              = %p\n", i, blocks[i]);
    }
    std::printf("heap strictly increasing? = %s\n",
                 strictly_increasing ? "YES" : "NO");
    std::printf(">> 答案是 NO 也是正常的（空闲块复用 + 大小分级）。\n");
    std::printf(">> 你的作业不是记结论，是解释'为什么'。\n");

    for (int i = 0; i < 5; ++i) {
        delete[] blocks[i];
    }
  std::printf("sizeof(int*)  = %zu\n", sizeof(int*));
  std::printf("sizeof(char*) = %zu\n", sizeof(char*));
  std::printf("sizeof(void*) = %zu\n", sizeof(void*));  
 struct S {
        char  a;
        int   b;
        char  c;
    };
    std::printf("sizeof(S) = %zu\n", sizeof(S));
        struct Inner { char x; int y; };
    struct Outer { char a; Inner in; char b; };
    std::printf("sizeof(Inner) = %zu\n", sizeof(Inner));
    std::printf("sizeof(Outer) = %zu\n", sizeof(Outer));
        struct T {
        char    a;
        double  d;
        int     arr[3];
    };
    std::printf("sizeof(T) = %zu\n", sizeof(T));

return 0;
  }
 // ============================================================
// MENTOR'S SELF-DOUBT（这份样板最可能错在哪——先自己找，再对答案）
// ============================================================
// 1. 指针比较用了 uintptr_t 整数转换：比较"无关指针"在标准里是
//    unspecified 行为，工程上没事，但你要知道这不是标准保证。
// 2. %p 的输出格式 MinGW 和 MSVC 不同（有无 0x 前缀、是否补零），
//    不影响判断，但别因为格式差异以为自己写错了。
// 3. 本程序必须 -O0/Debug 构建：开了 /O2 后 stack_var 可能被塞进
//    寄存器，那时"变量的地址"根本不存在，打印结果是误导。
// 4. new char[64] 连续 5 次是否递增，取决于分配器策略与当时堆状态，
//    换机器/换运行次数结果可能不同——这本身就是教学点。
// 5. probe_stack 靠 depth<4 递归 4 层：若未来有人把构建改成 /O2，
//    编译器可能内联或改写调用结构，逐层变小的现象会失真。
//    （所以第 3 条的 -O0 是硬要求，不是建议。）
// ============================================================
