#include "frame.h"
#include "ll_double.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

uint64_t frames_allocated;
uint64_t frames_available;

// No need to change or initialize this. This is "empty" space that simulate your physical memory.
_Alignas(4096) char memory[MEMORY_SIZE];

// You should define a bitmap to keep track of the allocated frames here.
uint8_t bitmap[128];

// Define the stack here
struct s_node {
	u_int16_t* data;
	struct s_node* next;
	struct s_node* prev;
};

struct s_node* head_s;

char* decimal_to_binary(int n, int len_b);
int binary_to_decimal(int n);

void frame_init() {
	// Initialize global variables
	frames_allocated = 0;
	frames_available = 1024;
	
	// Initialize the bitmap
	for (int i = 0; i < 128; i++) {
		bitmap[i] = 0;
	}

	// Initialize the stack
	struct s_node* main_s = malloc(sizeof(struct s_node));;
    main_s->data = NULL;
    main_s->next = NULL; 
    
    // Update the pointer
    head_s = main_s;
}


int64_t allocate_frame(int number_frames) {

	// Consult the bitmap and return the first available frame number, marking it as allocated

	// Invalid inputs
	int64_t frame_number = -1;
	if (number_frames <= 0 || frames_available == 0) {
		return frame_number;
	}

	// The stack works with allocations of size 1 only
	// Retrieve the allocated bit from stack if it is not empty
	if (number_frames == 1) {
		if (head_s->data != NULL){

			u_int8_t removed_val = *(head_s->data);
			char* rep = decimal_to_binary(bitmap[removed_val], 7);
			frame_number = removed_val * 8;

			for (int l = 0; l < strlen(rep); l++){
				if (rep[l] == '0'){
					rep[l] = '1';

					int final = binary_to_decimal(atoi(rep)); // turn char to int decimal AND turn int binary to int decimal

					bitmap[removed_val] = final; // update the bitmap with the new value

					// Update the stack
					head_s = head_s->prev; 
					free(head_s->next);
					head_s->next = NULL;

					// Free the variables
					free(rep);
					frame_number += l;
					break;
				}
			}
			frames_available -= 1;
			frames_allocated += 1;
			return frame_number; // Successful return
		}
	}

	// If number_frames > 8, more indices of the bitmap has to be checked
	int checking_ahead = number_frames/8;
	int found_index = 0;

	// Find an available frame in the bitmap
	for (int i = 0; i < 128 - checking_ahead; i++) {

		// Check if the frame is full
		if (bitmap[i] == 255) {
			continue;
		}

		// Storing the retrieved the bytes
		u_int8_t byte_group[checking_ahead+1];

		// Retrieve the group needed for allocation
		for (int j = 0; j < checking_ahead+1; j++) {
			byte_group[j] = bitmap[i+j];
		}

		// String for combining binary representations
		char* p = (char*)malloc(((8*checking_ahead)+1) * sizeof(char));	// CHANGE OTHER P AS WELL
		
		// Concat all the byte representations
		for (int k = 0; k < checking_ahead+1; k++) {
			// Get the binary representation of the byte
			char* cur_representation = decimal_to_binary(byte_group[k], 7);
			// Concat the strings
			strcat(p, cur_representation); // NOTE, ONCE p GROWS TOO MUCH, IT THROWS A TRACE TRAP ERROR. 

			// Free the memory of temporary variable
			free(cur_representation);
		}

		

		// Trying to find a fit in the bitgroup
		int longest_running_index = 0;
		int longest_running_counter = 0;
		int running_index = 0;
		int running_counter = 0;

		int found = 0;

		for (int l = 0; l < strlen(p); l++){
			if (p[l] == '1'){
				running_index = l+1;
				running_counter = 0;
				continue;
			} else {
				running_counter++;
			}

			// Update the max length and index of the available frame group
			if (running_counter >= longest_running_counter) {
				longest_running_counter = running_counter;
				longest_running_index = running_index;
			}

			if (number_frames == longest_running_counter) {
				found = 1;
				break;
			}
		}

		// Space is available
		if (found) {

			// Update the bits
			for (int k = longest_running_index; k < longest_running_index + longest_running_counter; k++) {
				p[k] = '1';
				
			}

			// Ungroup and reassign the chars
			for (int j = 0; j < checking_ahead+1; j++) {

				// Retrieve a bit group
				char temp[9];
				for (int k = 0; k < 8; k++) {
					temp[k] = p[(j*8)+k];
				}
				temp[8] = '\0';

				int final = binary_to_decimal(atoi(temp)); // turn char to int decimal AND turn int binary to int decimal
				
				// Reassign the group to the bitmap
				bitmap[j+i] = final; 
			}

			free(p);			

			frames_available -= number_frames;
			frames_allocated += number_frames;
			
			frame_number = (i * 8) + longest_running_index;

			return frame_number; // Successful return
		}

		// Free the memory of temporary variable
		free(p);
	}

	return -1; // Return according to what's documented in the header file for this module
}

int64_t deallocate_frame(uint64_t frame_number, int number_frames) {

	// Invalid input
	if (frame_number < 0) {
		return -1; // Unsuccessful return
	}

	// Get the index of the frame and bit of the frame
	int frame_index = frame_number/8;
	int byte_index = frame_number%8;
	
	// Allocation for bytes need to be retrieved
	int check_ahead = (((byte_index) + number_frames)) / 8;
	u_int8_t byte_group[check_ahead+1];

	for (int j = 0; j < check_ahead + 1; j++) {
		byte_group[j] = bitmap[frame_index+j];
	}

	// String for combining binary representations
	char* p = (char*)malloc((8*check_ahead)+1);	
		
	// Concat all the byte representations
	for (int k = 0; k < check_ahead + 1; k++) {

		// Get the binary representation of the byte
		char* cur_representation = decimal_to_binary(byte_group[k], 7);

		// Concat the strings
		strcat(p, cur_representation);

		// Free the memory of temporary variable
		free(cur_representation);
	}	

	// Turn the bits
	for (int l = byte_index; l < byte_index + number_frames; l++){ 
		p[l] = '0';
		frames_allocated -= 1;
		frames_available += 1;

		// Add the free frames to the stack
		struct s_node* new_bit = malloc(sizeof(struct s_node));
		u_int16_t* val = malloc(sizeof(u_int16_t));
		*val = (frame_index) + (l/8);
        new_bit->data = val;
        new_bit->next = NULL;
		new_bit->prev = head_s;
		head_s->next = new_bit;
		head_s = new_bit;
	}

	for (int j = 0; j < check_ahead + 1; j++) {

		// Retrieve a bit group
		char temp[9];
		for (int k = 0; k < 8; k++) {
			temp[k] = p[(j*8)+k];
		}
		temp[8] = '\0';

		int final = binary_to_decimal(atoi(temp)); // turn char to int binary AND turn int binary to int decimal
		
		// Reassign the group to the map
		bitmap[j+frame_index] = final; 
	}

	free(p);

	return number_frames; // Successful return
}

/**
 * Convert an integer binary into integer decimal
 *
 * @param n integer binary to be converted
 *
 * @return The integer decimal version of n
 */
int binary_to_decimal(int n) {
    int num = n;
    int dec_value = 0;
  
    // Initializing base value to 1, i.e 2^0
    int base = 1;
  
    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;
  
        dec_value += last_digit * base;
  
        base = base * 2;
    }
  
    return dec_value;
}

/**
 * Convert an integer decimal into integer binary char array
 *
 * @param n integer decimal to be converted
 * @param len_b number of bits of n
 *
 * @return The integer binary version of n as a char array
 */
char* decimal_to_binary(int n, int len_b){
  int c, d, t;
  char *p;

  t = 0;
  p = (char*)malloc(8+1);

  if (p == NULL)
    exit(EXIT_FAILURE);

  for (c = len_b ; c >= 0 ; c--) // len_b : 7
  {
    d = n >> c;

    if (d & 1)
      *(p+t) = 1 + '0';
    else
      *(p+t) = 0 + '0';

    t++;
  }
  *(p+t) = '\0';

  return  p;
}