#ifndef STORAGE_H
#define STORAGE_H

/// A block has 4K
#define BLOCK_SIZE (4 * 1024)

/// We have (16 * 1024) blocks
#define NUM_BLOCKS (16 * 1024)

/// Total device size is NUM_BLOCKS * BLOCK_SIZE: 64MB in our default configuration
#define DEVICE_SIZE (NUM_BLOCKS * BLOCK_SIZE)

/**
 * This array pointer emulates a block device, that is, a device from which
 * we can read and write blocks of a certain size.
 *
 *  Example of block devices include disks or flash storage.
 */
extern char *storage;

/**
 * Initializes the pseudo-storage device, allocating a memory of size \ref DEVICE_SIZE bytes.
 *
 * @return 0 if successful, -1 if memory cannot be allocated.
 */
int storage_init();

/**
 * Read provided block number into the data buffer (which must have at least \ref BLOCK_SIZE bytes).
 *
 * @param block_number Block number to read.
 * @param data Buffer to read into.
 *
 * @return 0 if successful, -1 otherwise.
 */
int storage_read_block(uint64_t block_number, void *data);

/**
 * Write provided block number with the data buffer (which must have at least \ref BLOCK_SIZE bytes).
 *
 * @param block_number Block number to write.
 * @param data Buffer to write from.
 *
 * @return 0 if successful, -1 otherwise.
 */
int storage_write_block(uint64_t block_number, void *data);

#endif /* STORAGE_H */
