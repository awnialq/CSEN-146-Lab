// Name: Awni AlQuraini 
// Date: 04/15/2026
// Title: Lab 3 - Part 1
// Description: The implementation of TCP for the server - handles the sending of one file to a client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    int sockfd;
    char rec_buf[256];

    struct sockaddr_in server_addr, client_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Failed to create socket");
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

    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    int conn_fd;
    socklen_t client_len = sizeof(client_addr);
    if((conn_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0){
        perror("Did not accept connection to client");
        close(sockfd);
        return EXIT_FAILURE;
    }

    ssize_t req_len = read(conn_fd, rec_buf, sizeof(rec_buf) - 1);
    if (req_len <= 0) {
        perror("Failed to read requested filename");
        close(conn_fd);
        close(sockfd);
        return EXIT_FAILURE;
    }
    rec_buf[req_len] = '\0';

    FILE *src = fopen(rec_buf, "r");
    if(! src){
        perror("Could not open file");
        close(conn_fd);
        close(sockfd);
        return EXIT_FAILURE;
    }
    
    char buf[256];
	size_t bytes_read;

	while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
        ssize_t bytes_sent = write(conn_fd, buf, bytes_read);
        if (bytes_sent < 0) {
            perror("Failed to send file data");
            fclose(src);
            close(conn_fd);
            close(sockfd);
            return EXIT_FAILURE;
        }
	}

    fclose(src);
    close(conn_fd);
    close(sockfd);
    return EXIT_SUCCESS;
}