#include <iostream>

#include "ThreadPool.h"

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

int main()
{
    int                           size = 16;
    std::vector<int>              initial(size, 2);
    std::vector<int>              resultVec;
    std::vector<std::future<int>> futureVec;
	
    const std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

    ThreadPool pool(16);
    for (size_t i = 0; i < initial.size(); i++)
    {
        auto future = pool.enqueue(
            [](int init) {
                int res = 0;
                res     = Step1(init);
                res     = Step2(res);
                res     = Step3(res);
                res     = Step4(res);
                return res;
            },
            initial[i]);
        futureVec.emplace_back(std::move(future));
    }
    for (auto&& fut : futureVec)
    {
        resultVec.push_back(fut.get());
    }

    const std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    std::cout << "Calculate use time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms = "
              << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s.\n";

    return 0;
}
