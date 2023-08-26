#pragma once

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>

using namespace std;

template<typename T>
class SyncQueue
{
public:
    SyncQueue(int maxSize) :m_maxSize(maxSize), m_needStop(false)
    {
    }

    void Put(const T&x)
    {
        Add(x);
    }

    void Put(T&&x)
    {
        Add(std::forward<T>(x));
    }

    void Take(std::list<T>& list)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        // 1. 停止标志 2. 不为空，当不满足任何一个条件时，
        // 条件变量会释放mutex并将线程置于waiting状态。
        // 等待其他线程调用 notify_one/notify_all将其唤醒
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty(); });
        
        if (m_needStop) {
            return;
        }
        list = std::move(m_queue);
        m_notFull.notify_one();
    }

    void Take(T& t)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty(); });
        
        if (m_needStop) {
            return;
        }
        t = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            m_needStop = true;
        }
        // notify_one() 因为只唤醒等待队列中的第一个线程；不存在锁争用，所以能够立即获得锁。
        // 其余的线程不会被唤醒，需要等待再次调用notify_one()或者notify_all()。
        // notify_all()：会唤醒所有等待队列中阻塞的线程，存在锁争用，只有一个线程能够获得锁。
        // 那其余未获取锁的线程接会继续尝试获得锁(类似于轮询)，而不会再次阻塞。
        // 当持有锁的线程释放锁时，这些线程中的一个会获得锁。而其余的会接着尝试获得锁。
        m_notFull.notify_all();
        m_notEmpty.notify_all();
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.empty();
    }

    bool Full()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size() == m_maxSize;
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size();
    }

    int Count()
    {
        return m_queue.size();
    }
private:
    bool NotFull() const
    {
        bool full = m_queue.size() >= m_maxSize;
        if (full) {
            cout << "full, waiting, thread id: " << this_thread::get_id() << endl;
        }
        return !full;
    }

    bool NotEmpty() const
    {
        bool empty = m_queue.empty();
        if (empty) {
            cout << "empty,waiting, thread id: " << this_thread::get_id() << endl;
        }
        return !empty;
    }

    template<typename F>
    void Add(F&&x)
    {
        // 新任务加入队列
        std::unique_lock< std::mutex> locker(m_mutex);
        m_notFull.wait(locker, [this]{return m_needStop || NotFull(); });
        if (m_needStop) {
            return;
        }
        m_queue.push_back(std::forward<F>(x));
        m_notEmpty.notify_one();
    }

private:
    std::list<T> m_queue; //缓冲区
    std::mutex m_mutex; //互斥量和条件变量结合起来使用
    std::condition_variable m_notEmpty;//不为空的条件变量
    std::condition_variable m_notFull; //没有满的条件变量
    int m_maxSize; //同步队列最大的size

    bool m_needStop; //停止的标志
};