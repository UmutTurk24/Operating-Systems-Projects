#ifndef FILES_H
#define FILES_H

#include<stdint.h>

#define MAX_FILES 1024

/**
 * Represents an inode of our filesystem.
 *
 * You are only going to use \ref flags_used, \ref size, and \ref head_pointer_block
 * in this assignment, but the other members are representative of a typical inode.
 */
struct inode_t_ {
	/// Owner's user ID
	uint16_t owner_uid;
	/// Owner's group ID
	uint16_t owner_gid;
	/// Permissions coded as a 16-bit integer
	uint16_t permissions;
	/// 1-bit flag indicating whether the inode is used
	uint16_t flags_used : 1;
	/// Reserved for flags
	uint16_t flags_reserved : 15;
	/// Stores the size of the tile
	uint64_t size;
	/// Reserved for general use
	uint64_t general_reserved : 64;
	/// Points to the (unique) indirect block containing the file's blocks
	uint64_t head_pointer_block;
};

typedef struct inode_t_ inode_t;

/**
 * Represents an internal pointer block of our filesystem.
 *
 * An internal pointer block has 512 64-bit entries (totaling 4K) that refer to the
 * data blocks of the file.
 *
 * Entry 0 contains the number of the disk block containing bytes 0 ... 4K - 1 of the file.
 * Entry 1 contains the number of the disk block containing bytes 4K ... 8K - 1 of the file, and so on.
 */
struct pointer_block_t_ {
	uint64_t entry[512];
};

typedef struct pointer_block_t_ pointer_block_t;

/**
 * Allocates and initializes the number of blocks necessary to store \ref MAX_FILES inode entries,
 * followed by the number of blocks necessary to store a bitmap for \ref NUM_BLOCKS blocks in disk.
 */
void format();

/**
 * Creates a file in the provided inode number.
 *
 * @param inode_number Inode where the file is going to be created
 * @return 0 on success, -1 otherwise.
 */
int ifile_create(uint64_t inode_number);

/**
 * Reads @p how_many bytes from the file referenced by @p inode_number,
 * starting at offset @p from, reading into the buffer @p buffer.
 *
 * @param inode_number Inode of the file we read from.
 * @param buffer Buffer were we read into.
 * @param how_many The amount of bytes we read from the file.
 * @param from The initial position of the file where we read from.
 *
 * @return 0 on success, -1 otherwise.
 */
int ifile_read(uint64_t inode_number, char *buffer, uint64_t how_many, uint64_t from);

/**
 * Writes @p how_many bytes into the file referenced by @p inode_number,
 * starting at offset @p from, copying from the buffer @p buffer.
 *
 * @param inode_number Inode of the file we write to.
 * @param buffer Buffer were we read from.
 * @param how_many The amount of bytes we read from the buffer.
 * @param from The initial position of the file where we write to.
 *
 * @return 0 on success, -1 otherwise.
 */
int ifile_write(uint64_t inode_number, void *buffer, uint64_t how_many, uint64_t from);

#endif /* FILES_H */
