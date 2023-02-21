#pragma once

#include <iostream>
#include <sstream>

enum PhoneType {
    MOBILE = 0,
    HOME   = 1,
    WORK   = 2,
};

// 手机
struct Phone {
    char*     _Number;
    PhoneType _Type;
};

// 人
struct Person {
    char*  _Name;
    int    _Id;
    char*  _Email;
    double _Salary;
    int    _PhoneNum;
    Phone* _Phone;
};
