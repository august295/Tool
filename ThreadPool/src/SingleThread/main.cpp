#include <iostream>

#include "SingleThread.h"

int main()
{
    int size = 16;
    std::list<int> input(size, 2);
    std::list<int> output;
    Calculate(input, output);

    return 0;
}
