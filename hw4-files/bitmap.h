#ifndef BITMAP_H
#define BITMAP_H

#include<stdint.h>

/**
 * Initializes the bitmap for a disk blocks where @p number_inode_blocks blocks have been used for inodes,
 * @p number_bitmap_blocks have been used for the bitmap itself, and a total of @p number_blocks is available.
 *
 * @param number_inode_blocks Number of blocks in the filesystem used to store inodes.
 * @param number_bitmap_blocks Number of blocks in the filesystem used to store the bitmap itself.
 * @param number_blocks Total number of blocks in the disk.
 */
void bitmap_init(uint64_t number_inode_blocks, uint64_t number_bitmap_blocks, uint64_t number_blocks);

/**
 * Allocates the first empty block in the disk among those that were not used for inodes or the bitmap.
 *
 * @return The number of the allocated block.
 */
int bitmap_allocate_block();

/**
 * Deallocates the provided @p block, making it available in the disk.
 */
void bitmap_deallocate_block(uint64_t block);

#endif /* BITMAP_H */
