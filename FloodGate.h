#ifndef FLOOD_GATE_H_
#define FLOOD_GATE_H_
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    pthread_mutex_t open_lock;
    pthread_cond_t wait_cond;
    bool open;
} FloodGate;

void FloodGate_init(FloodGate* this, bool closed);
void FloodGate_open(FloodGate* this);
void FloodGate_close(FloodGate* this);
void FloodGate_wait(FloodGate* this);
//TODO: FloodGate_wait_until
//TODO: FloodGate_is_open

#endif
