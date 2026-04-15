// Name: Awni AlQuraini 
// Date: 04/15/2026
// Title: Lab 3 - Part 4
// Description: The implementation of TCP for the server - handles the sending of multiple files to multiple clients

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

static void *handle_client(void *arg) {
    int conn_fd = *((int *)arg);
    free(arg);

    char rec_buf[256];
    ssize_t req_len = read(conn_fd, rec_buf, sizeof(rec_buf) - 1);
    if (req_len <= 0) {
        perror("Failed to read requested filename");
        close(conn_fd);
        return NULL;
    }
    rec_buf[req_len] = '\0';

    FILE *src = fopen(rec_buf, "rb");
    if (!src) {
        perror("Could not open file");
        close(conn_fd);
        return NULL;
    }

    char buf[256];
    size_t bytes_read;

    while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
        size_t total_sent = 0;
        while (total_sent < bytes_read) {
            ssize_t bytes_sent = write(conn_fd, buf + total_sent, bytes_read - total_sent);
            if (bytes_sent <= 0) {
                perror("Failed to send file data");
                fclose(src);
                close(conn_fd);
                return NULL;
            }
            total_sent += (size_t)bytes_sent;
        }
    }

    if (ferror(src)) {
        perror("Failed to read source file");
    }

    fclose(src);
    close(conn_fd);
    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5033);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    if (listen(sockfd, 16) < 0) {
        perror("listen failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    while (1) {
        socklen_t client_len = sizeof(client_addr);
        int conn_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (conn_fd < 0) {
            perror("accept failed");
            continue;
        }

        int *conn_fd_ptr = (int *)malloc(sizeof(int));
        *conn_fd_ptr = conn_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, conn_fd_ptr) != 0) {
            perror("pthread_create failed");
            free(conn_fd_ptr);
            close(conn_fd);
            continue;
        }

        if (pthread_detach(tid) != 0) {
            perror("pthread_detach failed");
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}