// Name: *****
// Date: 04/08/2026
// Title: Lab 4 - Part 2
// Description: UDP client implementation for requesting a specific file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Usage: %s {server ip} {port #} {file requested}", argv[0]);
        exit(1);
    }

    int socketfd;

    uint8_t buf[256];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr);

    struct hostent *host;
    host = (struct hostnet *)gethostbyname(argv[1]);

    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Could not establish socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr = *((struct in_addr *)host->h_addr);

    /*
        Implement file transfer logic here
    */
}