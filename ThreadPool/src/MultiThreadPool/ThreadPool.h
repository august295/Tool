#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
public:
    // 构造函数
    ThreadPool(size_t);

    // 添加任务
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    // 析构函数
    ~ThreadPool();

private:
    std::vector<std::thread>          workers;     // 线程数组，需要跟踪线程，以便我们可以结束它们
    std::queue<std::function<void()>> tasks;       // 任务队列
    std::mutex                        queue_mutex; // 同步，任务队列互斥量
    std::condition_variable           condition;   // 同步，条件变量
    bool                              stop;        // 结束标志
};

// 构造函数，指定启动线程数量，推荐 (std::thread::hardware_concurrency() - 1)
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] {
                for (;;)
                {
                    std::function<void()> task;
                    // 大括号用于自动销毁 lock，作用域和生命周期
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        // 等待信号，判断线程池被终止或队列非空
                        this->condition.wait(lock,
                                             [this] { return this->stop || !this->tasks.empty(); });

                        // 若线程池被终止，并且没有待处理任务，则该线程终止
                        if (this->stop && this->tasks.empty())
                            return;

                        // 从队列中获取首部任务并弹出
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    // 执行任务
                    task();
                }
            });
}

// 添加一个新的任务到线程池
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    // return_type 是该任务的返回类型
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    // 用于存放任务结果的 std::future 对象
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // 终止线程池之后不允许添加任务
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    // 通知一个线程来处理
    condition.notify_one();
    return res;
}

// 析构函数，结束所有线程
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

#endif