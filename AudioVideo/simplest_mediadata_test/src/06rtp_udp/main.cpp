#include <iostream>
#include <string>

#include "rtp_udp.h"

int main(int argc, char* argv[])
{
	// ָ������С��Ĭ��̫����ܶ���
    // ffmpeg -re -i sintel.ts -f mpegts -pkt_size 1464 udp://127.0.0.1:8880
    // ffmpeg -re -i sintel.ts -f rtp_mpegts -pkt_size 1464 udp://127.0.0.1:8880
    simplest_udp_parser(8880);

    return 0;
}