#ifndef __STRUCT_H__
#define __STRUCT_H__

#include <string>

// 枚举
enum Topic {
    MUSIC, // 音乐
    GAME,  // 游戏
    BOOK,  // 书籍
};

// Phone
typedef struct Phone {
    int m_type;
} Phone;

// People
typedef struct People {
    char m_sex;    /* 性别 */
    int  m_age;    /*
    
年龄
    
*/
    char*  m_name; // 名称
    int    m_phone_num;
    Phone* m_phone;
} People;

#endif
