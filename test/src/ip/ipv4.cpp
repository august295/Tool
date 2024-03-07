#include "ipv4.h"

/***************************************************************************
 * @date    2021/01/07
 * @brief   256 以内的数值转字符
 * @param   num 数字
 * @param   bin 二进制字符串
 ***************************************************************************/
void dec_to_bin(unsigned int num, char* bin)
{
    for (int i = 7; i >= 0; i--)
    {
        *(bin + i) = (num % 2) + 48;
        num        = num / 2;
    }
}

/***************************************************************************
 * @date    2021/01/07
 * @brief   ipv4 字符串转二进制和长整数
 * @param   ip_str  ipv4 字符串
 * @param   output  output[0] 存二进制； output[1] 存长整数
 ***************************************************************************/
void ip_str_convert_long(const char* ip_str, char** output)
{
    char         ip4_str[17]; // 复制，不知道为什么只有 char[] 才能使用 strtok()
    unsigned int ip4_int[4];  // 保存四个数字
    unsigned int sum;         // 求和

    strcpy(ip4_str, ip_str);
    char* pNext = strtok(ip4_str, ".");
    for (int i = 0; pNext != NULL; i++)
    {
        ip4_int[i] = atoi(pNext);
        pNext      = strtok(NULL, ".");
    }

    dec_to_bin(ip4_int[0], output[0]);
    dec_to_bin(ip4_int[1], output[0] + 8);
    dec_to_bin(ip4_int[2], output[0] + 16);
    dec_to_bin(ip4_int[3], output[0] + 24);
    sum = (ip4_int[0] << 24) | (ip4_int[1] << 16) | (ip4_int[2] << 8) | (ip4_int[3]);
    sprintf(output[1], "%d", sum); // 格式化输出
}

/***************************************************************************
 * @date    2021/01/07
 * @brief   长整数转二进制和 ipv4 字符串
 * @param   num     长整数
 * @param   output  output[0] 存二进制； output[1] 存 ipv4 字符串
 ***************************************************************************/
void long_convert_ip_str(unsigned long num, char** output)
{
    unsigned int ip4_int[4]; // 保存四个数字

    ip4_int[0] = ((num & 0xff000000) >> 24);
    ip4_int[1] = ((num & 0x00ff0000) >> 16);
    ip4_int[2] = ((num & 0x00000ff00) >> 8);
    ip4_int[3] = (num & 0x000000ff);

    dec_to_bin(ip4_int[0], output[0]);
    dec_to_bin(ip4_int[1], output[0] + 8);
    dec_to_bin(ip4_int[2], output[0] + 16);
    dec_to_bin(ip4_int[3], output[0] + 24);
    sprintf(output[1], "%d.%d.%d.%d", ip4_int[0], ip4_int[1], ip4_int[2], ip4_int[3]);
}
