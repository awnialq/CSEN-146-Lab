// Name: Awni AlQuraini 
// Date: 04/01/2026
// Title: Lab 1 - Part 3
// Description: This file implements a file copy using regular functions

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <errno.h>   // errno
#include <sys/mman.h> // mmap()

#define BUF_SIZE 2048 //buffer size

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
	FILE *src = fopen(src_filename, "r");
	FILE *dst = fopen(dst_filename, "w");
	char buf[BUF_SIZE];
	size_t bytes_read;

	while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) {
		fwrite(buf, 1, bytes_read, dst);
	}

	fclose(src);
	fclose(dst);
	return 0;
}


int main(int argc, char* argv[]) {
	if (argc != 3) {  // check correct usage
		fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
		exit(1);
	}
	//get the source and destination files from the command line arguments
	//call the check copy times function
    func_copy(argv[1], argv[2]);
	return 0;
}