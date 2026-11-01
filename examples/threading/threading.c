#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)


// Sleep for a given number of milliseconds
static bool sleep_ms(int milliseconds)
{
    struct timespec req;

    req.tv_sec = milliseconds / 1000;
    req.tv_nsec = (milliseconds % 1000) * 1000000L;

    while (nanosleep(&req, &req) == -1) {
        if (errno != EINTR) {
            return false;
        }
    }

    return true;
}


void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    struct thread_data* thread_func_args =
        (struct thread_data*) thread_param;

    thread_func_args->thread_complete_success = false;

    // Wait before obtaining the mutex
    if (!sleep_ms(thread_func_args->wait_to_obtain_ms)) {
        ERROR_LOG("Error during initial sleep");
        return thread_func_args;
    }

    // Obtain the mutex
    if (pthread_mutex_lock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Error obtaining mutex");
        return thread_func_args;
    }

    // Wait before releasing the mutex
    bool sleep_success =
        sleep_ms(thread_func_args->wait_to_release_ms);

    // Release the mutex
    int unlock_result =
        pthread_mutex_unlock(thread_func_args->mutex);

    if (unlock_result != 0) {
        ERROR_LOG("Error releasing mutex");
        return thread_func_args;
    }

    thread_func_args->thread_complete_success = sleep_success;

    return thread_func_args;
    //return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    // Validate arguments
    if (thread == NULL || mutex == NULL ||
        wait_to_obtain_ms < 0 || wait_to_release_ms < 0) {
        return false;
    }

    // Allocate memory for thread_data
    struct thread_data* thread_func_args =
        malloc(sizeof(struct thread_data));

    if (thread_func_args == NULL) {
        ERROR_LOG("Memory allocation failed");
        return false;
    }

    // Initialize thread arguments
    thread_func_args->mutex = mutex;
    thread_func_args->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_func_args->wait_to_release_ms = wait_to_release_ms;
    thread_func_args->thread_complete_success = false;

    // Create the thread
    int result = pthread_create(thread,
                                NULL,
                                threadfunc,
                                thread_func_args);

    if (result != 0) {
        ERROR_LOG("pthread_create failed");
        free(thread_func_args);
        return false;
    }

    return true;
    return false;
}

