#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage.h"

char *storage;

int storage_init() {
	storage = (char *) malloc(DEVICE_SIZE);

	if(!storage) {
		return -1;
	}

	return 0;
}

int storage_read_block(uint64_t block_number, void *data) {
	if(block_number >= NUM_BLOCKS) {
		return -1;
	}

	memcpy(data, storage + (block_number * BLOCK_SIZE), BLOCK_SIZE);

	return 0;
}

int storage_write_block(uint64_t block_number, void *data) {
	if(block_number >= NUM_BLOCKS) {
		return -1;
	}

	memcpy(storage + (block_number * BLOCK_SIZE), data, BLOCK_SIZE);

	return 0;
}
