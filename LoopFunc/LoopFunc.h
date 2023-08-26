#pragma once

#include <iostream>
#include <string.h>
#include <thread>
#include <functional>

constexpr int THREAD_PRIORITY = 40;   // real-time priority

// period     unit:second
// bindCPU    change the CPU affinity of this thread
class LoopFunc {

public:
    using Callback = std::function<void()>;

    // 删除默认构造函数
    LoopFunc(const LoopFunc&) = delete;
    LoopFunc& operator=(const LoopFunc&) = delete;

    LoopFunc(std::string name, float period, const Callback& _cb);
    LoopFunc(std::string name, float period, int bindCPU, const Callback& _cb);

    LoopFunc(LoopFunc && obj);
    LoopFunc& operator=(LoopFunc && obj);
    ~LoopFunc();

    void start();
    void shutdown();

private:
    void entryFunc();
    void functionCB();

    std::thread _thread;
    std::string _name;
    float _period;
    int _bindCPU;
    Callback _fp;
    bool _bind_cpu_flag = false;
};