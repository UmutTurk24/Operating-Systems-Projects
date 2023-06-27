#define _OPEN_SYS_ITOA_EXT

#include<stdio.h>
#include <stdlib.h>
#include <limits.h>

#include"frame.h"
#include "translation.h"

void *table_root = NULL;
#define UNUSED_ENTRY 0
#define USED_ENTRY 1

void decimal_to_binary_buf(uint64_t n, uint64_t len_b, char* p);
long binary_to_decimal_long(long n);

int vm_map(uint64_t page, uint64_t frame, int number, uint16_t flags) {
	
	void* cur_root = table_root;

	// Partition the page bits
	char buffer [65];
	decimal_to_binary_buf(page, 63, buffer);
	long cur_ind = 0;

	// Traverse to the leaf node
	for (int i = 0; i < 3; i++){ 
		
		// Check if node has been initialized
		if (cur_root == NULL) {
			// Create a new table
			uint64_t frame_number = allocate_frame(1);
			char* frame_address = FRAME_ADDRESS(frame_number);
			struct entry* table = (struct entry*) frame_address;

			cur_root = frame_address;
			table_root = cur_root;

			// Re-assign the default values of the entries
			for (int x = 0; x < 512; x++) {
				((entry *) cur_root)[x].address = 0;
				((entry *) cur_root)[x].unused = 0;
				((entry *) cur_root)[x].flags = UNUSED_ENTRY;
			}
		}

		// Retrieve the current 9 bits 
		char cur_bits[10];
		cur_bits[9] = '\0';
		for (int j = 0; j < 9; j++){
			cur_bits[j] = buffer[28 + (i*9) + j];
		}

		// Find the current index and set its flag to used
		cur_ind = binary_to_decimal_long(atoi(cur_bits));
		((entry *) cur_root)[cur_ind].flags = USED_ENTRY;

		// Use the current page bits to move to the next table
		uint64_t next_address = ((entry *) cur_root)[cur_ind].address;

		// Check if the next_address exists. If not, create it
		if (next_address == 0) {
			uint64_t frame_number = allocate_frame(1);
			char* frame_address = FRAME_ADDRESS(frame_number);
			((entry *) cur_root)[cur_ind].address = (u_int64_t) frame_address;
			((entry *) cur_root)[cur_ind].flags = USED_ENTRY;
			next_address = (u_int64_t) frame_address;

			// Initialize the table entries
			cur_root = (u_int64_t*) next_address;

			for (int x = 0; x < 512; x++) {
				((entry *) cur_root)[x].address = 0;
				((entry *) cur_root)[x].unused = 0;
				((entry *) cur_root)[x].flags = UNUSED_ENTRY;
			}			

			continue;
		}	
		
		cur_root = (u_int64_t*) next_address;
	}

	char cur_bits[10];
	cur_bits[9] = '\0';
	for (int j = 0; j < 9; j++){
		cur_bits[j] = buffer[55 + j];
	}
	cur_ind = binary_to_decimal_long(atoi(cur_bits));

	// Check the number of frames need to be allocated
	if (cur_ind + number < 513) {

		// Map the frames to pages
		u_int64_t translation = (u_int64_t) FRAME_ADDRESS(frame);
		for (int x = cur_ind; x < cur_ind + number; x++) {
			((entry *) cur_root)[x].address = translation;
			((entry *) cur_root)[x].flags = flags;
		}

		// Successful return
		return 1;
	} 

	// Unsuccessful return
	return 0;
}

int vm_unmap(uint64_t page, int number) {

	void* cur_root = table_root;

	// Partition the page bits
	char buffer [65];
	decimal_to_binary_buf(page, 63, buffer);
	long cur_ind = 0;

	// Traverse to the leaf node
	for (int i = 0; i < 3; i++){ 
		
		// Check if node has been initialized
		if (cur_root == NULL) {
			return 0;
		}

		// Retrieve the current 9 bits 
		char cur_bits[10];
		cur_bits[9] = '\0';
		for (int j = 0; j < 9; j++){
			cur_bits[j] = buffer[28 + (i*9) + j];
		}

		// Convert the bits to binary for finding the table index
		cur_ind = binary_to_decimal_long(atoi(cur_bits));
		
		// Use the current page bits to move to the next table
		uint64_t next_address = ((entry *) cur_root)[cur_ind].address;
		if (next_address == 0) {
			return 0;
		}
		cur_root = (u_int64_t*) next_address;
	}

	char cur_bits[10];
	cur_bits[9] = '\0';
	for (int j = 0; j < 9; j++){
		cur_bits[j] = buffer[55 + j];
	}

	// Check the number of frames need to be unmapped
	if (cur_ind + number < 513) {

		// Unmap the frames
		for (int x = cur_ind; x < cur_ind + number; x++) {
			((entry *) cur_root)[x].address = 0;
			((entry *) cur_root)[x].flags = UNUSED_ENTRY;
		}
		// Successful return
		return 1;
	} 

	// Unsuccessful return
	return 0;
}

uint64_t vm_locate(int number) {

	void* cur_root = table_root;

	// Current indices at certain levels
	int index_mapping[3] = {0, 0, 0};
	uint64_t* pointer_mapping[3] = {0, 0, 0};

	// Current level
	int cur_level = 0;
	// Uninitialized root or can't traverse bigger lists
	if (cur_root == NULL || number > 512) {
		return -1;
	} 

	while (1) {

		int broken = 0;

		for (int x = index_mapping[cur_level]; x < 512; x++){

			// If at the bottom level, search!
			if (cur_level == 3) {


				// Init search variables
				int counter = 0;
				int found_index = 0;

				for (int x = 0; x < 512; x++) {

					// Found enough space
					if (counter == number) {

						// Page number calculation
						uint64_t page_num = found_index + (512 * index_mapping[2]) + (512 * 512 * index_mapping[1]) + (512 * 512 * 512 * index_mapping[0]);
						return page_num;
					}

					// Update counter according to the availability
					if (((entry *) cur_root)[x].flags == UNUSED_ENTRY) {
						counter++;
					} else {
						counter = 0;
						found_index = x;
					}

				}

				// Go up a level, and continue
				cur_level--;
				uint64_t next_address = (u_int64_t) pointer_mapping[cur_level];
				cur_root = (u_int64_t*) next_address;
				x = index_mapping[cur_level];
				continue;
			}

			// Found a place to go down
			if ( ((entry *) cur_root)[x].address != 0) {
				pointer_mapping[cur_level] = (u_int64_t*) cur_root;
				uint64_t next_address = ((entry *) cur_root)[x].address;
				cur_root = (u_int64_t*) next_address;
				index_mapping[cur_level] = x;
				cur_level++;
				broken = 1;
				break;
			}
		}

		if (broken) {
			broken = 0;
			continue;
		}

		// Go up a level and update mapping
		for (int j = cur_level; j < 3; j++) {
			index_mapping[j] = 0;
			pointer_mapping[j] = 0;
		}

		cur_level--;

		// Return if every initialized entry on the table is checked
		if (cur_level < 0) {
			return 0;
		}

		index_mapping[cur_level]++;
		uint64_t next_address = (u_int64_t) pointer_mapping[cur_level];
		cur_root = (u_int64_t*) next_address;
		
	}

	return 0;
}

uint64_t vm_translate(uint64_t virtual_address) {
	void* cur_root = table_root;

	// Partition the page bits
	char buffer [65];
	decimal_to_binary_buf(virtual_address, 63, buffer);
	long cur_ind = 0;

	// Traverse in the tree to find the page address
	for (int i = 0; i < 4; i++){ 
		
		// Check if node has been initialized
		if (cur_root == NULL) {
			return UINT64_MAX;
		}

		// Retrieve the current 9 bits 
		char cur_bits[10];
		cur_bits[9] = '\0';
		for (int j = 0; j < 9; j++){
			cur_bits[j] = buffer[28 + (i*9) + j];
		}

		// Convert the bits to binary for finding the table index
		cur_ind = binary_to_decimal_long(atoi(cur_bits));
		
		// Use the current page bits to move to the next table
		uint64_t next_address = ((entry *) cur_root)[cur_ind].address;
		if (next_address == 0) {
			return UINT64_MAX;
		}

		if (i==3) {
			// char* f_add = FRAME_NUMBER(next_address);
			uint64_t frame_num = FRAME_NUMBER((char *) next_address);
			return frame_num;
		}
		cur_root = (u_int64_t*) next_address;
	}
	
	return virtual_address;
}

/**
 * Convert an integer decimal into integer binary char array
 *
 * @param n integer decimal to be converted
 * @param len_b number of bits of n
 * @param p buffer for storing the converted number
 *
 */
void decimal_to_binary_buf(uint64_t n, uint64_t len_b, char* p){ 
	long long c, d, t;
	c = 0;
	t = 0;
	t = 0;

	if (p == NULL)
		exit(EXIT_FAILURE);
	for (c = len_b ; c >= 0 ; c--)
	{
		d = n >> c;

		if (d & 1)
		*(p+t) = 1 + '0';
		else
		*(p+t) = 0 + '0';

		t++;
	}
	*(p+t) = '\0';

}

/**
 * Convert an integer binary into integer decimal
 *
 * @param n integer binary to be converted
 *
 * @return The integer decimal version of n
 */
long binary_to_decimal_long(long n) {
    long num = n;
    long dec_value = 0;
  
    // Initializing base value to 1, i.e 2^0
    long base = 1;
  
    long temp = num;
    while (temp) {
        long last_digit = temp % 10;
        temp = temp / 10;
  
        dec_value += last_digit * base;
  
        base = base * 2;
    }
    return dec_value;
}
