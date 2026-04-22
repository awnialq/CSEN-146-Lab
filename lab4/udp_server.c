// Name: *******
// Date: 04/08/2026
// Title: Lab 2 - Part 1
// Description: UDP server implementation for serving a specific file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s {port #}", argv[0]);
        exit(0);
    }

    int sockfd;

    uint8_t rec_buf[256], s_buf[256];

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr);

    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0) < 0)){
        perror("Could not establish socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if((bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr))) < 0){
        perror("Could not bind server addr to socket");
        exit(1);
    }

    /*
        Implement file transfer logic here
    */
}