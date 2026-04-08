// Name: Awni AlQuraini 
// Date: 04/08/2026
// Title: Lab 2 - Part 1
// Description: Calculates the time taken for each copy with and without multithreading

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()
#include <sys/mman.h> // mmap()

#define BUF_SIZE 2048 //buffer size

// data structure to hold copy
struct copy_struct {
	int thread_id; 
	char* src_filename;
	char* dst_filename; 
};

/*
Since clock() gets cpu time it doesn't show the efficiency gains so i showed real time instead.
*/

static double now_seconds(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

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
// copies a files form src_filename to dst_filename using syscalls open(), read(), write(), close()
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

// thread function to copy one file using C standard functions
void* copy_thread_func(void* arg) {
	struct copy_struct params = *(struct copy_struct*)arg;  
	func_copy(params.src_filename, params.dst_filename);
	pthread_exit(NULL);
}

// thread function to copy one file using system calls
void* copy_thread_syscall(void* arg) {
	struct copy_struct params = *(struct copy_struct*)arg;  
	syscall_copy(params.src_filename, params.dst_filename);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	// check correct usage of arguments in command line
	if (argc < 2) {  
		fprintf(stderr, "usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
		exit(1);
	}

	int num_files = argc - 1; 
	pthread_t threads[num_files]; 
	struct copy_struct thread_params[num_files]; 
	
	// Use monotonic wall-clock timing
	double start_time, end_time;

	// setup destination names
	char* dst_names[num_files];
	for (int i = 0; i < num_files; i++) {
		dst_names[i] = (char*)malloc(32);
		sprintf(dst_names[i], "dst_%d.dat", i + 1);
	}

	//single threaded function
	start_time = now_seconds();
	for (int i = 0; i < num_files; i++) {
		func_copy(argv[i+1], dst_names[i]);
	}
	end_time = now_seconds();
	printf("Singlethreaded - functions copy time: %.5f\n", end_time - start_time);

	//single threaded syscall
	start_time = now_seconds();
	for (int i = 0; i < num_files; i++) {
		syscall_copy(argv[i+1], dst_names[i]);
	}
	end_time = now_seconds();
	printf("Singlethreaded - syscalls copy time:  %.5f\n", end_time - start_time);

	printf("--------------------------------------------------------\n");

	//multithreaded function
	start_time = now_seconds();
	
	for (int i = 0; i < num_files; i++) {
		thread_params[i].thread_id = i;
		thread_params[i].src_filename = argv[i+1];
		thread_params[i].dst_filename = dst_names[i];

		// Pass the specific function thread routine
		pthread_create(&threads[i], NULL, copy_thread_func, (void*)&thread_params[i]);
	}

	for (int i = 0; i < num_files; i++) {
		pthread_join(threads[i], NULL);
	}
	
	end_time = now_seconds();
	printf("Multithreaded - functions copy time: %.5f\n", end_time - start_time);

	//multithreaded syscalls
	start_time = now_seconds();
	
	for (int i = 0; i < num_files; i++) {
		thread_params[i].thread_id = i;
		thread_params[i].src_filename = argv[i+1];
		thread_params[i].dst_filename = dst_names[i];

		// Pass the specific syscall thread routine
		pthread_create(&threads[i], NULL, copy_thread_syscall, (void*)&thread_params[i]);
	}

	for (int i = 0; i < num_files; i++) {
		pthread_join(threads[i], NULL);
	}
	
	end_time = now_seconds();
	printf("Multithreaded - syscalls copy time:  %.5f\n", end_time - start_time);
	printf("--------------------------------------------------------\n");

	for (int i = 0; i < num_files; i++) {
		free(dst_names[i]);
	}

	return 0;
}