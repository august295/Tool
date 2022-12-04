#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    template <class F, class... Args>
    auto Enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    void ThreadLoop();

protected:
    std::atomic<bool>                 m_stop;
    std::vector<std::thread>          m_threads;
    std::queue<std::function<void()>> m_queue;
    std::mutex                        m_mutex;
    std::condition_variable           m_cv;
};

#endif

// 将传入函数、可变参数、函数返回，通过模板封装成 std::function<void()>
template <class F, class... Args>
inline auto ThreadPool::Enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    /* The return type of task `F` */
    using return_type = std::result_of_t<F(Args...)>;

    /* wrapper for no arguments */
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_stop)
            throw std::runtime_error("The threads has been stop.");

        /* wrapper for no returned value */
        m_queue.emplace([task]() -> void { (*task)(); });
    }
    m_cv.notify_one();
    return res;
}
