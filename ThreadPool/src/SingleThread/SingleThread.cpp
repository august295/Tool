#include <iostream>
#include <chrono>
#include <thread>

#include "SingleThread.h"

int Step1(const int& input)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return input + 2;
}

int Step2(const int& input)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return input - 2;
}

int Step3(const int& input)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return input * 2;
}

int Step4(const int& input)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return input / 2;
}

void Calculate(const std::list<int>& initial, std::list<int>& result)
{
    const std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
    int                                                      res   = 0;
    for (auto init : initial)
    {
        res = Step1(init);
        res = Step1(res);
        res = Step1(res);
        res = Step1(res);
        result.push_back(res);
    }
    const std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    std::cout << "Calculate use time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms = "
              << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s.\n";
}
