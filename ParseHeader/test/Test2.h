#ifndef __TEST2_H__
#define __TEST2_H__

namespace AAA {
namespace BBB {
struct TestStruct {
    int  a; // a
    int  b; // b
    long c; // c

    // 构造函数
    TestStruct() {}
    ~TestStruct() {}
    TestStruct operator=(const TestStruct& t)
    {
        a = t.a;
        b = t.b;
        c = t.c;
        return *this;
    }
};

// 枚举
enum TestEnum {
    MUSIC, // 音乐
    GAME,  // 游戏
    BOOK,  // 书籍
};
} // namespace BBB
} // namespace AAA

#endif
