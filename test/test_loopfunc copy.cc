#include <iostream>
#include "LoopFunc.h"

void task1() {
    std::cout << "task1" << std::endl;
}

void task2() {
    std::cout << "task2" << std::endl;
}

class Task {
public:
    void func(void) {
        std::cout << "class task::func" << std::endl;
    }
};

int main() {
    Task task;

    LoopFunc loop_1("task1", 1,  std::bind(task1));
    LoopFunc loop_2("task2", 2, 2, std::bind(task2));;
    LoopFunc loop_3("task3", 2, 2, std::bind(&Task::func, &task));
    // LoopFunc loop_3("task3", 2, 2, std::bind((void(Task::*)())&Task::func, &task))

    loop_1.start();
    loop_2.start();
    loop_3.start();

    return 0;
}
