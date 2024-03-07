#include <gtest/gtest.h>

#include "ipv4.h"

TEST(test_ip, test_ipv4)
{
    char** output = (char**)malloc(sizeof(char*) * 2);
    output[0]     = (char*)malloc(sizeof(char) * 33);
    output[1]     = (char*)malloc(sizeof(char) * 33);

    const char* ip_str = "11.2.5.192";
    memset(output[0], '\0', 33);
    memset(output[1], '\0', 33);
    ip_str_convert_long(ip_str, output);
    EXPECT_STREQ(output[1], "184681920");

    unsigned long num = 184681920;
    memset(output[0], '\0', 33);
    memset(output[1], '\0', 33);
    long_convert_ip_str(num, output);
    EXPECT_STREQ(output[1], "11.2.5.192");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}