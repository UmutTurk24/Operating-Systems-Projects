#ifndef MEMORY_H
#define MEMORY_H

#include<stdint.h>

#define MEMORY_SIZE (1024 * 4096) // 4MB

/// Number of allocated frames
extern uint64_t frames_allocated;

/// Number of available frames
extern uint64_t frames_available;

/// You are allocating memory from this block
extern char memory[MEMORY_SIZE];

#define FRAME_NUMBER(address) ((address - memory) / 4096)
#define FRAME_ADDRESS(frame) ((memory + (frame * 4096)))

/**
 * Initializes the bookkeeping data structures for allocated/available frames.
 */
void frame_init();

/**
 * Allocate @p number_frames consecutive frames.
 *
 * @param number_frames Number of consecutive frames to be allocated.
 *
 * @return The number of the first allocated frame, or -1 if no frame was available.
 */
int64_t allocate_frame(int number_frames);

/**
 * Deallocate @p number_frames starting from the previously allocated @p frame_number (inclusive).
 *
 * @param number_frames Number of consecutive frames to be deallocated.
 *
 * @return The number of dallocated frames, or -1 one of the informed frames was not allocated.
 */
int64_t deallocate_frame(uint64_t frame_number, int number_frames);

#endif /* MEMORY_H */
