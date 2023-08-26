#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

void* thread_func(void* arg) {
    printf("Thread start.\n");

    // 绑定线程到第二个CPU核心上
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(1, &cpu_set);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);

    // 执行线程任务
    sleep(1);

    printf("Thread end.\n");
    return NULL;
}

int main(int argc, char* argv[]) {
    // 创建线程
    pthread_t tid;
    int rc = pthread_create(&tid, NULL, thread_func, NULL);
    if (rc != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // 等待线程执行完毕
    pthread_join(tid, NULL);

    return 0;
}