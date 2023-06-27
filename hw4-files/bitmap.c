#include "bitmap.h"

#include "storage.h"
#include "files.h"

/// First disk block used to store the bitmap
static uint64_t bitmap_first_block;
/// Last disk block used to store the bitmap
static uint64_t bitmap_number_blocks;

/// The first block in the disk after the blocks used to store the inodes or the bitmap itself
uint64_t first_allocatable;
/// The the last block in the disk
uint64_t last_allocatable;

void bitmap_init(uint64_t number_inode_blocks, uint64_t number_bitmap_blocks, uint64_t number_blocks) {
	bitmap_first_block = number_inode_blocks ;
	bitmap_number_blocks = number_bitmap_blocks;

	// The first allocatable block in the disk is the first one after the blocks used to store the inodes or the bitmap itself
	first_allocatable = (number_inode_blocks + number_bitmap_blocks);

	// The last allocatable block is the last block in the disk
	last_allocatable = number_blocks - 1;
}

int bitmap_allocate_block() {
	char block[BLOCK_SIZE];

	for(int i = 0; i < bitmap_number_blocks; i++) {
		storage_read_block(i + bitmap_first_block, block);

		for(int j = 0; j < BLOCK_SIZE; j++) {
			for(int k = 0; k < 8; k++) {
				uint64_t referenced_block =  (i * BLOCK_SIZE) + (j * 8) + k;

				// Only considers blocks between the first and last allocatable blocks
				if(referenced_block >= first_allocatable && (block[j] & (1 << k)) == 0) {
					block[j] |= (1 << k);

					storage_write_block(i + bitmap_first_block, block);
					return referenced_block;
				}
			}
		}
	}

	return -1;
}

void bitmap_deallocate_block(uint64_t block_number) {
	int disk_block_number = block_number / (BLOCK_SIZE * 8);
	int disk_block_offset = block_number % (BLOCK_SIZE * 8);
	int disk_block_byte = disk_block_offset / 8;
	int disk_block_bit = disk_block_offset % 8;

	char block[BLOCK_SIZE];

	storage_read_block(bitmap_first_block + disk_block_number, block);

	block[disk_block_byte] &= ~(1 << disk_block_bit);

	storage_write_block(disk_block_number, block);
}
