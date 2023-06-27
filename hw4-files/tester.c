#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "storage.h"
#include "files.h"

int main(int argc, char **argv) {
	// Initialize storage
	storage_init();

	// Format the filesystem
	format();

	char *string1 = "Hello filesystem world!";
	char *string2 = "It's me again! Hello, hello, hello!";

	// Large buffer
	char buffer[1000];

	// Create a first file
	ifile_create(0);

	// Write to file with inode 0
	ifile_write(0, string1, strlen(string1) + 1, 0);
	
	// Read back from file with inode 0
	ifile_read(0, buffer, strlen(string1) + 1, 0);
	printf("READ 0: %s\n", buffer);

	// Create second file
	ifile_create(1);

	// Write to file with inode 1
	ifile_write(1, string2, strlen(string2) + 1, 0);

	// Read back from file with inode 1
	ifile_read(1, buffer, strlen(string2) + 1, 0);
	printf("READ 1: %s\n", buffer);

	// Overwrite on file with inode 1
	ifile_write(1, "LALA", strlen("LALA"), 0);

	// Read back from file with inode 1
	ifile_read(1, buffer, strlen(string2) + 1, 0);
	printf("READ 1 (after overwrite): %s\n", buffer);

	// Write to file with inode 1, offset 200
	ifile_write(1, "HIGH_OFFSET Hello!", strlen("HIGH_OFFSET Hello!") + 1, 200);

	// Read back from file with inode 1
	ifile_read(1, buffer, strlen("HIGH_OFFSET Hello!") + 1, 200);
	printf("READ 1 (from high offset): %s\n", buffer);

	return EXIT_SUCCESS;
}
