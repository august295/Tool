#ifndef __TEST2_H__
#define __TEST2_H__

#include <string>

namespace AAA {
namespace BBB {
typedef struct TestStruct {
    int    a; // a
    int    b; // b
    long   c; // c

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
} TestStruct1, TestStruct2;

// 枚举
enum TestEnum {
    MUSIC, // 音乐
    GAME,  // 游戏
    BOOK,  // 书籍
};
} // namespace BBB
} // namespace AAA

#endif
