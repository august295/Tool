#ifndef __TEST_H__
#define __TEST_H__

// 枚举
enum Topic {
    // 1231
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
    char m_sex;   /* 性别 */
    int  m_age;   /*
年龄
*/
    char* m_name; // 名称
    // 未关联的注释 1
    int m_phone_num; // 手机数量
    /*
      未关联的注释 2
     */
    Phone* m_phone;
} People;

#endif
