#include "threads.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>

#include<unistd.h>
#include<signal.h>

// Init thread_context and other global variables
tcb_t thread_context[MAX_THREADS];
tcb_t* current_thread_context;
// jmp_buf buffers[MAX_THREADS];
int created;
char stack[MAX_THREADS][STACK_SIZE];
int context_switching;

/**
 * Handles the sigusr signal
 * 
 * Once the signal is raised, it first creates the stack for the thread and saves the current stack.
 * The signal is followed by a longjmp, which executes the function of the current_thread_context
 */
void sigusr_handler(int signal_number) {
	if(setjmp(current_thread_context->buffer) == 0) {
		created = 1;
	}
	else {
        current_thread_context->function(NULL); 
	}
}

/**
 * Initializes the threads for the user
 */
void thread_init(int preemption_enabled) {
    // Init current_thread_context
    current_thread_context = malloc(sizeof(struct tcb_t_));
    context_switching = 0;
    // Create the main thread and add it to the context array
    jmp_buf main_buffer;
    tcb_t main_thread_info = {
        .number = 0,
        .function = NULL,
        .argument = NULL,
        .return_value = NULL,
        .state = STATE_ACTIVE,
        .buffer = main_buffer,
        .stack = &stack[0][0],
        .joiner_thread_number = -1,
    };

    // Add the main-thread to the the thread_context 
    thread_context[0] = main_thread_info;

    // Update the current thread context as the main-thread
    current_thread_context = &thread_context[0];

    // Create other threads and fill up the context array
    for (int i = 1; i < MAX_THREADS; i++) {
        jmp_buf buffer;
        tcb_t inactive_thread_info = {
            .number = i,
            .function = NULL,
            .argument = NULL,
            .return_value = NULL,
            .state = STATE_INVALID,
            .buffer = buffer,
            .stack = &stack[i][0],
            .joiner_thread_number = -1,
        };
        thread_context[i] = inactive_thread_info;
    }

    // Setup signals for thread creation
    struct sigaction sigusr_hints;

	memset(&sigusr_hints, 0, sizeof(struct sigaction));
	sigusr_hints.sa_handler = sigusr_handler;
	sigusr_hints.sa_flags = SA_ONSTACK; // <<-- Look at this
	sigemptyset(&sigusr_hints.sa_mask);

	if(sigaction(SIGUSR1, &sigusr_hints, NULL) == -1) {
		perror("sigaction/SIGUSR1");
		exit(EXIT_FAILURE);
	}

    // Send a signal to myself, and specify a new stack
	stack_t new_stack;
	stack_t old_stack;

	new_stack.ss_flags = 0;
	new_stack.ss_size = STACK_SIZE;
	new_stack.ss_sp = &stack[0][0];

	if(sigaltstack(&new_stack, &old_stack) == -1) {
		perror("sigaltstack");
		exit(EXIT_FAILURE);
	}
    created = 0;
    raise(SIGUSR1); // Sending the signal to create a new stack
	while(!created) {}; 

	// longjmp(current_thread_context->buffer, 1); // Execute the function
}

/**
 * Creates a thread for the given function and assigned arguments
 * 
 * @param function, function the thread will execute
 * @param arugment, arguments for the given function
 */
int thread_create(void *(*function)(void *), void *argument){

    // Find an invalid thread and activate it
    for (int i = 0; i < MAX_THREADS; i++) {
        if(thread_context[i].state == STATE_INVALID){
            // Assign the function, argument, and the status to the new thread
            thread_context[i].function = function;
            thread_context[i].argument = argument;
            thread_context[i].state = STATE_ACTIVE;
            if (setjmp(current_thread_context->buffer) == 0) {
            // Create a new stack for the new thread
                stack_t new_stack;
                stack_t old_stack;

                new_stack.ss_flags = 0;
                new_stack.ss_size = STACK_SIZE;
                new_stack.ss_sp = stack[i];

                if(sigaltstack(&new_stack, &old_stack) == -1) {
                    perror("sigaltstack");
                    exit(EXIT_FAILURE);
                }

                // Update the current context
                created = 0;
                current_thread_context = &thread_context[i];
                
                raise(SIGUSR1); 

                while(!created) {};
                
                longjmp(current_thread_context->buffer, 1);
            } 

            return thread_context[i].number;
        }
    }

    return -1;
}

/**
 * Gives quantum for the next available thread in round-robin order
 * @return If there is a thread with state STATE_ACTIVE, returns 1. Returns 0 if there is not. 
*/
int thread_yield(void) {
    // printf("HERE IN YIELD \n");
    int current_worker_id = current_thread_context->number;

    // Find the next active thread
    for (int i = 0; i < MAX_THREADS - 1; i++) {

        // Handle iteration
        current_worker_id++;
        if (current_worker_id == MAX_THREADS){
            current_worker_id = 0;
        }
        
        // Check if next thread is available
        tcb_t* c_thread = &thread_context[current_worker_id];
        if (c_thread->state == STATE_ACTIVE){
            // printf("ID: %d \n", current_thread_context->number);
            if(setjmp(current_thread_context->buffer) == 0){
                current_thread_context = c_thread; // ?
                longjmp(current_thread_context->buffer, 1);
            }
            // printf("ID: %d \n", current_thread_context->buffer);
            // printf("HERE IN IF \n");
            return 1;
        }

    }
    return 0;
}

/**
 * Changes the current thread state to STATE_FINISHED and finds the next available thread
*/
void thread_exit(void *return_value) {
    // Change the status to FINISHED
    current_thread_context->state = STATE_FINISHED;

    // Update the return value of the thread as given
    current_thread_context->return_value = return_value;

    // Check if there is a joiner thread. 
    if (current_thread_context->joiner_thread_number != -1) {
        // Switch states if there is a joiner thread, and continue execution
        current_thread_context = &thread_context[current_thread_context->joiner_thread_number];
        current_thread_context->state = STATE_ACTIVE;
        longjmp(current_thread_context->buffer, 1);
    } else {
        // Find another thread to execute
        thread_yield();
    }
}

/**
 * Blocks the current thread and switches to the given thread
 * @param target_thread_number, the context of the thread to be switched
*/
void thread_join(int target_thread_number){
    // Update the current thread context state as BLOCKED
    printf("called join \n");
    current_thread_context->state = STATE_BLOCKED;

    if(setjmp(current_thread_context->buffer) == 0) {
        // Switch to the given thread's context
        current_thread_context = &thread_context[target_thread_number];
        current_thread_context->state = STATE_ACTIVE;
        longjmp(current_thread_context->buffer,1);
    }
    current_thread_context->state = STATE_ACTIVE;
    
}
