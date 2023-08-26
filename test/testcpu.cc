#include <iostream>
#include <thread>
void task(int cpu_id) {
    // 设置当前线程绑定到指定CPU核心上
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu_id, &mask);
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        std::cerr << "Failed to set thread affinity" << std::endl;
        return;
    }
    // 执行任务
    for (int i = 0; i < 100000000; ++i) {
        // do something
    }
}
int main() {
    int num_cpus = std::thread::hardware_concurrency();
    std::cout << "Detected " << num_cpus << " CPUs" << std::endl;
    int num_threads = 4; // 假设要创建4个线程
    for (int i = 0; i < num_threads; ++i) {
        int cpu_id = i % num_cpus; // 循环利用所有CPU核心
        std::thread t(task, cpu_id);
        t.detach(); // 分离线程
    }
    // 等待所有线程结束
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}