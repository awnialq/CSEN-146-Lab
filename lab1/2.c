// Name: Awni AlQuraini 
// Date: 04/01/2026
// Title: Lab 1 - Part 4
// Description: This file implements a file copy using syscalls  functions

#include <stdio.h>   // for stderr
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <sys/mman.h> // mmap()

#define BUF_SIZE 2048 //buffer size

int syscall_copy(char* src_filename, char* dst_filename)  {
	int src_fd = open(src_filename, O_RDONLY);	// opens a file for reading
	if (src_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		close(src_fd);
		exit(0);
	}
	// O_WRONLY - write only
	// O_CREAT - create file if it doesn't exist
	// O_TRUNC - truncates file to length 0 if it already exists (deletes existing file)
	// opens a file for writing; erases old file/creates a new file
	int dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);	
	if (dst_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		close(dst_fd);
		close(src_fd);
		exit(0);
	}
	// read/ write loop
	char buf[BUF_SIZE];  // allocate a buffer to store read data
	ssize_t bytes_read;
	while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) {
		write(dst_fd, buf, bytes_read);
	}
	// reads up to BUF_SIZE bytes from src_filename
	// writes bytes_read to dst_filename

	
	// closes src_fd file descriptor
	close(src_fd);
	// closes dst_fd file descriptor
	close(dst_fd);

	return 0;
}


int main(int argc, char* argv[]) {
	if (argc != 3) {  // check correct usage
		fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
		exit(1);
	}
	syscall_copy(argv[1], argv[2]);
	return 0;
}