#ifndef PARALLEL_SCHEDULER_H
#define PARALLEL_SCHEDULER_H

#include <pthread.h>
#include <queue>
#include <functional>


struct task_item {
    std::function<void(void*)> func;
    void* arg;
};

class parallel_scheduler
{
    pthread_t* threads;
    size_t cap;

    std::queue<task_item> q;
    pthread_mutex_t q_mtx;
    pthread_cond_t q_cv;
    bool stop;

    static void* workerThread(void* arg);

public:
    parallel_scheduler(size_t capacity);
    ~parallel_scheduler();
    void run(std::function<void(void*)> func, void* arg);
};

#endif
