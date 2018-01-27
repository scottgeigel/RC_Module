#ifndef RC_INIT_H
#define RC_INIT_H

#include <pthread.h>
#include <stdbool.h>

#ifndef RC_MAX_MODULES
    #define RC_MAX_MODULES 16
#endif
//Structures:
struct RC_Module;
typedef struct RC_Module RC_Module;

struct RC_Manager;
typedef struct RC_Manager RC_Manager;

//Constants:
#define RC_SHUTDOWN ((int) -1)
#define RC_UNKNOWN ((int) -2)
#define RC_INIT ((int) 0)
//Callbacks:
typedef int (*callback_on_rc_change)(void* p_this, int old_run_level, int new_run_level);
typedef bool (*callback_do_rc_task)(void* p_this, int run_level);

struct RC_Module {
    void* object;
    callback_on_rc_change on_rc_change;
    callback_do_rc_task do_rc_task;
    RC_Manager* parent;
    pthread_t thread;
    pthread_mutex_t routine_lock;
    pthread_cond_t wake_thread;
    pthread_mutex_t wake_lock;
    bool should_wait;
    bool stop_thread;
    bool thread_dead;//TODO: replace with pthread_cond_t???
};

struct RC_Manager {
    int run_level;
    pthread_t thread;
    pthread_mutex_t module_lock;
    RC_Module* modules[RC_MAX_MODULES];
    size_t modules_count;
};

void RC_Manager_init(RC_Manager* this);
void RC_Manager_destroy(RC_Manager* this);
void RC_Manager_launch(RC_Manager* this);
void RC_Manager_attach(RC_Manager* this, RC_Module* module);
void RC_Manager_set_rc(RC_Manager* this, int desired_rc);

void RC_Module_init(RC_Module* this, void* object, callback_do_rc_task do_rc_task, callback_on_rc_change on_rc_change);
bool RC_Module_stop(RC_Module* this);
void RC_Module_kill(RC_Module* this);
#endif
