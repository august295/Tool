#include "ThreadPool.h"

ThreadPool::ThreadPool()
    : m_stop(false)
{
    const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    m_threads.resize(num_threads);
    for (uint32_t i = 0; i < num_threads; i++)
    {
        m_threads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    for (auto& thread : m_threads)
    {
        thread.join();
    }
    m_threads.clear();
}

void ThreadPool::ThreadLoop()
{
    while (true)
    {
        std::function<void()> task;
        /* pop a task from queue, and execute it. */
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [&]() { return m_stop || !m_queue.empty(); });
            if (m_stop && m_queue.empty())
                return;
            /* even if stop = 1, once tasks is not empty, then
             * excucte the task until tasks queue become empty
             */
            task = std::move(m_queue.front());
            m_queue.pop();
        }
        task();
    }
}
