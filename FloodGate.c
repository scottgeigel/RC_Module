#include "FloodGate.h"

void FloodGate_init(FloodGate* this, bool closed) {
    this->open = !closed;
    this->open_lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    this->wait_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
}

void FloodGate_open(FloodGate* this) {
    pthread_mutex_lock(&this->open_lock);
    this->open = true;
    pthread_cond_broadcast(&this->wait_cond);
    pthread_mutex_unlock(&this->open_lock);
}

void FloodGate_close(FloodGate* this) {
    pthread_mutex_lock(&this->open_lock);
    this->open = true;
    pthread_mutex_unlock(&this->open_lock);
}

void FloodGate_wait(FloodGate* this) {
    pthread_mutex_lock(&this->open_lock);
    if (this->open) {
        pthread_mutex_unlock(&this->open_lock);
    } else {
        pthread_mutex_unlock(&this->open_lock);
        pthread_cond_wait(&this->wait_cond, &(pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER);
    }
}

#include <stdio.h>

void* test_func(void* args) {
    FloodGate* gate = args;
    printf("thread %lu waiting\n", pthread_self());
    FloodGate_wait(gate);
    printf("thread %lu woke up\n", pthread_self());
    return NULL;
}

int main() {
    #define THREAD_COUNT 10
    FloodGate gate;
    pthread_t threads[THREAD_COUNT];
    FloodGate_init(&gate, true);

    for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, test_func, &gate);
        printf("started thread %lu\n", threads[i]);
    }

    printf("press a character to get this moving\n");
    getchar();
    FloodGate_open(&gate);

    for (int i = 0; i < THREAD_COUNT; i++) {
        printf("joining thread %lu\n", threads[i]);
        pthread_join(threads[i], NULL);
    }
    return 0;
}
