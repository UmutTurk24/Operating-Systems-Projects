#include "synchronization.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ll_double.h"

#include "threads.h"

struct waiter_t {
	int thread_id;
};

/**
 * Initializes a lock the given lock
 * 
 * @param mutex: lock to be initialized
 * @return: 0 for successful initialization
 */
int thread_mutex_init(thread_mutex_t *mutex) {
	if(mutex) {
		atomic_init(&(mutex->locked), 0);
	}

	return 0;
}

/**
 * Locks the lock
 * 
 * @param mutex: lock to be used
 * @return: 0 for successful initialization
 */
int thread_mutex_lock(thread_mutex_t *mutex) {
	while(1) {
		// An empty while loop. In production code, you would insert
		// a "pause" assembly instruction inside the loop (on Intel)
		// in order to clear speculative operations in the CPU pipeline.
		while(mutex->locked); // spin!

		int expected = 0;

		if(atomic_compare_exchange_strong(&(mutex->locked), &expected, 1)) {
			break;
		}
	}

	return 0;
}

/**
 * Unlocks the lock
 * 
 * @param mutex: lock to be unlocked
 * @return: 0 for successful unlocking
 */
int thread_mutex_unlock(thread_mutex_t *mutex) {
	atomic_store(&mutex->locked, 0);

	return 0;
}

/**
 * Initializes the condition variable
 * 
 * @param condition_variable: lock to be initialized
 * @return: 0 for successful initialization
 */
int thread_cond_init(thread_cond_t *condition_variable) {
	// Initialize the waiter list
	struct list waiter_llinked_list;
	ll_init(&waiter_llinked_list);
	condition_variable->waiters_list = waiter_llinked_list;

	// Initialize the mutex 
	thread_mutex_t int_mutex;
	thread_mutex_init(&int_mutex);
	condition_variable->internal_mutex = int_mutex;
	
	return 0;
}

/**
 * Condition wait signal. Signals the thread for waiting
 * 
 * @param condition_variable: condition variable the current thread calls
 * @param mutex: mutex the condition variable is associated with
 * @return: 0 for successfully woken up thread after it waits
 */
int thread_cond_wait(thread_cond_t *condition_variable, thread_mutex_t *mutex) {

	// Get the internal lock of the condition variable
	thread_mutex_lock(&(condition_variable->internal_mutex));

	// Add the current thread to the waiter list
	ll_insert_head(&(condition_variable->waiters_list), current_thread_context);

	// Release the previously acquired lock
	thread_mutex_unlock(mutex);

	// Update the current state of the thread as BLOCKED
	current_thread_context->state = STATE_BLOCKED;

	// Release the internal lock of the condition variable
	thread_mutex_unlock(&(condition_variable->internal_mutex));

	thread_yield();

	// Acquire the condition mutex again
	thread_mutex_lock(mutex);
	
	return 0;
}

/**
 * Condition wait signal. Signals a thread for waking up for the given condition variable
 * 
 * @param condition_variable: condition variable the current thread calls
 * @return: 0 for waking up a thread, 1 for not waking up a thread
 */
int thread_cond_signal(thread_cond_t *condition_variable) {
	// Acquire the internal lock of the condition variable to avoid data races
	thread_mutex_lock(&(condition_variable->internal_mutex));

	// Remove the tail node of the waiter list
	struct node* removed_node = ll_remove_tail(&(condition_variable->waiters_list));

	// Release the internal lock of the condition variable
	thread_mutex_unlock(&(condition_variable->internal_mutex));
	if (removed_node != NULL) {
		tcb_t* removed_thread = removed_node->data;
		removed_thread->state = STATE_ACTIVE;
		return 0;
	}

	return 1;
}


/**
 * Condition wait signal. Signals all the thread for waking up for the given condition variable
 * 
 * @param condition_variable: condition variable the current thread calls
 * @return: 0 for successful call function
 */
int thread_cond_broadcast(thread_cond_t *condition_variable) {

	while (1) {
		if (thread_cond_signal(condition_variable) != 0){
			break;
		}
	}
	return 0;
}
