#include <iostream>
#include <string>

#include "flv.h"

int main(int argc, char* argv[])
{
    std::string filepath = "../../resources/";
    std::string flv      = filepath + "cuc_ieschool.flv";

    simplest_flv_parser(flv.c_str());

    return 0;
}