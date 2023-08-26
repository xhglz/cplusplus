#include "LoopFunc.h"
#include <cstring>
#include <sched.h>
#include <pthread.h>

// SCHED_OTHER 分时调度策略,（默认的）
// SCHED_FIFO实时调度策略,先到先服务
// SCHED_RR实时调度策略,时间片轮转 
inline void set_thread_priority(std::thread &th, int policy, int priority) {
    struct sched_param sch_params;
    sch_params.sched_priority = priority;
    // 句柄，调度策略，优先级
    if(pthread_setschedparam(th.native_handle(), policy, &sch_params)) {
        std::cerr <<"Failed to set Thread scheduling :" << std::strerror(errno) << std::endl;
    }
}

LoopFunc::LoopFunc(std::string name, float period, const Callback& _cb):
    _name(name), _period(period), _fp(_cb) {
    _bindCPU = -1;
}

LoopFunc::LoopFunc(std::string name, float period, int bindCPU, const Callback& _cb):
    _name(name), _period(period), _bindCPU(bindCPU), _fp(_cb) {
}

// Move Constructor
LoopFunc::LoopFunc(LoopFunc && obj) : _thread(std::move(obj._thread)) {

}

// Move Assignment Operator
LoopFunc & LoopFunc::operator=(LoopFunc && obj) {
    if (_thread.joinable()) {
        _thread.join();
    }
    _thread = std::move(obj._thread);
    return *this;
}

LoopFunc::~LoopFunc() {
    if(_thread.joinable()) {
        _thread.join();
    }
    std::cout << "Thread " << _name << " shutdown" << std::endl;
}

void LoopFunc::start() {
    std::cout << "Thread " << _name << " start" << std::endl;
    
    int num_cpus = std::thread::hardware_concurrency();
    if (_bindCPU >= 0 && _bindCPU < num_cpus) {
        _bind_cpu_flag = true;
    }
    std::thread task(&LoopFunc::entryFunc, this);
    _thread = move(task);
    set_thread_priority(_thread, SCHED_RR, THREAD_PRIORITY);
}

void LoopFunc::shutdown() {
    // 如果线程对象不可连接，则返回false。
    if(_thread.joinable()) {
        _thread.join();
    }
    std::cout << "Thread " << _name << " shutdown" << std::endl;
}

void LoopFunc::entryFunc() {
    if (true == _bind_cpu_flag) {
        cpu_set_t mask;            // 设置当前线程绑定到指定CPU核心上
        CPU_ZERO(&mask);           // CPU 集 set 进行初始化，将其设置为空集
        CPU_SET(_bindCPU, &mask);  // 指定的 cpu 加入 CPU 集 set 中
        // 线程绑定函数，pthread_self是一种函数，功能是获得线程自身的ID
        if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
            std::cerr << "Failed to set thread affinity" << std::endl;
            return;
        }
    }
    // 执行线程任务
    int64_t period = int64_t(_period * 1e3); // 换算成毫秒
    while(1) {
        functionCB();
        std::this_thread::sleep_for(std::chrono::milliseconds(period));
    }
}

void LoopFunc::functionCB() {
    (_fp)();
}