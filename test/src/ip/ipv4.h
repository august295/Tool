#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************
 * @date    2021/01/07
 * @brief   256 以内的数值转字符
 * @param   num 数字
 * @param   bin 二进制字符串
 ***************************************************************************/
void dec_to_bin(unsigned int num, char* bin);

/***************************************************************************
 * @date    2021/01/07
 * @brief   ipv4 字符串转二进制和长整数
 * @param   ip_str  ipv4 字符串
 * @param   output  output[0] 存二进制； output[1] 存长整数
 ***************************************************************************/
void ip_str_convert_long(const char* ip_str, char** output);

/***************************************************************************
 * @date    2021/01/07
 * @brief   长整数转二进制和 ipv4 字符串
 * @param   num     长整数
 * @param   output  output[0] 存二进制； output[1] 存 ipv4 字符串
 ***************************************************************************/
void long_convert_ip_str(unsigned long num, char** output);
