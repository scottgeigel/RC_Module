#include "rc_init.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Private Functions:
static void* RC_Module_threadrun(void* p_module);

void RC_Manager_init(RC_Manager* this) {
    this->run_level = RC_INIT;
    //TODO: make a thread for deferred run level setting
    this->module_lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    memset(&this->modules, 0, sizeof(this->modules));
    this->modules_count = 0;
}

void RC_Manager_destroy(RC_Manager* this) {

}

void RC_Manager_launch(RC_Manager* this) {
    //TODO: implement
    fprintf(stderr, "I didn't feel like implementing this, just call RC_Manager_set_rc\n");
    abort();
}

void RC_Manager_attach(RC_Manager* this, RC_Module* module) {
    //START take ownership of the modules
    pthread_mutex_lock(&this->module_lock);

    if (this->modules_count < RC_MAX_MODULES) {
        //START just in case, prevent module routine from running
        pthread_mutex_lock(&module->routine_lock);
        this->modules[this->modules_count++] = module;
        module->parent = this;
        module->on_rc_change(module, RC_UNKNOWN, this->run_level);
        module->wake_thread = true; //allow the thread to initialize any entry handler
        pthread_create(&module->thread, NULL, RC_Module_threadrun, module);
        pthread_mutex_unlock(&module->routine_lock);
        //END just in case, prevent module routine from running
    } else {
        fprintf(stderr, "ran out of slots. Right now it's #define RC_MAX_MODULES %d\n", RC_MAX_MODULES);
        abort();
    }
    pthread_mutex_unlock(&this->module_lock);
    //END take ownership of the modules
}

void RC_Manager_set_rc(RC_Manager* this, int desired_rc) {
    //TODO: this should be defferred action that cause static void
    //      RC_Manager_announce to be called
    pthread_mutex_lock(&this->module_lock);
    //freeze all threads
    for (int i = 0; i < this->modules_count; i++) {
        this->modules[i]->wake_thread = false;
    }

    for (int i = 0; i < this->modules_count; i++) {
        //TODO: find a more optimal way of doing this?
        pthread_mutex_lock(&this->modules[i]->routine_lock);
        this->modules[i]->on_rc_change(this->modules[i], this->run_level, desired_rc);
        pthread_mutex_unlock(&this->modules[i]->routine_lock);
    }

    //now that the run level has been anounced to all modules, actually
    //set the value
    this->run_level = desired_rc;

    //wake all threads
    for (int i = 0; i < this->modules_count; i++) {
        this->modules[i]->wake_thread = true;
    }
    pthread_mutex_unlock(&this->module_lock);
}

void RC_Module_init(RC_Module* this, void* object, callback_do_rc_task do_rc_task, callback_on_rc_change on_rc_change) {
    //copy over the basic objects
    this->object = object;
    this->on_rc_change = on_rc_change;
    this->do_rc_task = do_rc_task;
    //manager won't be set until we're attached
    this->parent = NULL;
    //we'll initialize this->thread later
    //set the mutex as a fast lock
    this->routine_lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    //by default thread shouldn't run until initially signalled
    this->wake_thread = false;
}

static void* RC_Module_threadrun(void* p_module) {
    RC_Module* this = p_module;
    struct timespec sleep_time;//TODO: replace with pthread_cond_t calls
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 5 * 1000000L;

    this->thread_dead = false;
    while (!this->stop_thread) {
        //TODO: replace with pthread_cond_t calls
        if (this->wake_thread) {
            pthread_mutex_lock(&this->routine_lock);
            this->wake_thread = this->do_rc_task(this->object, this->parent->run_level);
            pthread_mutex_unlock(&this->routine_lock);
        } else {
            nanosleep(&sleep_time, NULL);
        }
    }
    this->thread_dead = true;
    return NULL;
}

bool RC_Module_stop(RC_Module* this) {
    struct timespec wait_time;//TODO: make it a parameter?
    struct timespec wait_interval;

    wait_time.tv_sec = 0;
    wait_time.tv_nsec = 50 * 1000000L;

    wait_interval.tv_sec = 0;
    wait_time.tv_nsec = 1 * 1000000L;

    //TODO: when replaced with signal, this will need to wake the thread
    this->stop_thread = true;
    while (!this->thread_dead && wait_time.tv_nsec > 0) {
        nanosleep(&wait_interval, NULL);
        wait_time.tv_nsec -= wait_time.tv_nsec;
    }

    return this->thread_dead;
}

void RC_Module_kill(RC_Module* this) {
    fprintf(stderr, "%s not implemented\n", __func__);
    abort();//TODO: implement
}
