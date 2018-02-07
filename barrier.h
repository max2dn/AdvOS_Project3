

#ifndef PROJECT_3_BARRIER_H
#define PROJECT_3_BARRIER_H

#include <mutex>
#include <pthread.h>



class barrier
{
private:
    pthread_mutex_t m;
    pthread_cond_t cond;
    int waiting;
    int thread_count;

public:
    barrier(int count){
        m = PTHREAD_MUTEX_INITIALIZER;
        cond = PTHREAD_COND_INITIALIZER;
        thread_count = count;
        waiting = 0;
    }

    void wait()
    {
        pthread_mutex_lock(&m);
        waiting++;
        if (waiting == thread_count){
            waiting = 0;
            pthread_cond_broadcast(&cond);
        } else {
            pthread_cond_wait(&cond, &m);
        }
        pthread_mutex_unlock(&m);
    }
};


#endif //PROJECT_3_BARRIER_H
