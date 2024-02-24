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
    // ���캯��
    ThreadPool(size_t);

    // �������
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    // ��������
    ~ThreadPool();

private:
    std::vector<std::thread>          workers;     // �߳����飬��Ҫ�����̣߳��Ա����ǿ��Խ�������
    std::queue<std::function<void()>> tasks;       // �������
    std::mutex                        queue_mutex; // ͬ����������л�����
    std::condition_variable           condition;   // ͬ������������
    bool                              stop;        // ������־
};

// ���캯����ָ�������߳��������Ƽ� (std::thread::hardware_concurrency() - 1)
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] {
                for (;;)
                {
                    std::function<void()> task;
                    // �����������Զ����� lock�����������������
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        // �ȴ��źţ��ж��̳߳ر���ֹ����зǿ�
                        this->condition.wait(lock,
                                             [this] { return this->stop || !this->tasks.empty(); });

                        // ���̳߳ر���ֹ������û�д�������������߳���ֹ
                        if (this->stop && this->tasks.empty())
                            return;

                        // �Ӷ����л�ȡ�ײ����񲢵���
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    // ִ������
                    task();
                }
            });
}

// ���һ���µ������̳߳�
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    // return_type �Ǹ�����ķ�������
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    // ���ڴ���������� std::future ����
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // ��ֹ�̳߳�֮�������������
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    // ֪ͨһ���߳�������
    condition.notify_one();
    return res;
}

// �������������������߳�
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