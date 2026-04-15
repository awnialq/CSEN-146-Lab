// Name: Awni AlQuraini 
// Date: 04/15/2026
// Title: Lab 3 - Part 2
// Description: The implementation of TCP for the client - sends a request for a file on the server that it then writes "locally"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int sockfd;
    char buf[256];
    struct sockaddr_in server_addr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s *remote file* *destination file*\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *remote_filename = argv[1];
    char *local_filename = (argc >= 3) ? argv[2] : "tcp_client.dat";

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Can't create socket");
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5033);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Could not connect to server");
        close(sockfd);
        return EXIT_FAILURE;
    }

    size_t request_len = strlen(remote_filename) + 1;
    ssize_t bytes_sent = send(sockfd, remote_filename, request_len, 0);

    FILE *dst = fopen(local_filename, "wb");
    if (!dst) {
        perror("Could not open destination file");
        close(sockfd);
        return EXIT_FAILURE;
    }

    ssize_t bytes_read;
    while ((bytes_read = read(sockfd, buf, sizeof(buf))) > 0) {
        size_t written = fwrite(buf, 1, (size_t)bytes_read, dst);
    }

    if (bytes_read < 0) {
        perror("Error while reading from server");
        fclose(dst);
        close(sockfd);
        return EXIT_FAILURE;
    }

    fclose(dst);
    close(sockfd);
    return EXIT_SUCCESS;
}
