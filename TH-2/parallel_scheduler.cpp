#include "parallel_scheduler.h"
#include <iostream>
parallel_scheduler::parallel_scheduler(size_t capacity)
{
    cap = capacity;
    threads = new pthread_t[cap];
    stop = false;

    pthread_mutex_init(&q_mtx, nullptr);
    pthread_cond_init(&q_cv, nullptr);

    for (size_t i = 0; i < cap; ++i)
        pthread_create(&threads[i], nullptr, &parallel_scheduler::workerThread, this);
}

parallel_scheduler::~parallel_scheduler()
{
    pthread_mutex_lock(&q_mtx);
    stop = true;
    pthread_mutex_unlock(&q_mtx);

    pthread_cond_broadcast(&q_cv);

    for (size_t i = 0; i < cap; ++i)
        pthread_join(threads[i], nullptr);

    pthread_mutex_destroy(&q_mtx);
    pthread_cond_destroy(&q_cv);
    delete[] threads;
}

void parallel_scheduler::run(std::function<void(void*)> func, void* arg)
{
    pthread_mutex_lock(&q_mtx);
    q.push({func, arg});
    pthread_mutex_unlock(&q_mtx);

    pthread_cond_signal(&q_cv);
}

void* parallel_scheduler::workerThread(void* arg)
{
    auto* self = static_cast<parallel_scheduler*>(arg);

    while (true)
    {
        pthread_mutex_lock(&self->q_mtx);

        while (self->q.empty() && !self->stop)
            pthread_cond_wait(&self->q_cv, &self->q_mtx);

        if (self->stop && self->q.empty()) {
            pthread_mutex_unlock(&self->q_mtx);
            break;
        }

        task_item item = self->q.front();
        self->q.pop();

        pthread_mutex_unlock(&self->q_mtx);

        item.func(item.arg);
    }

    return nullptr;
}

