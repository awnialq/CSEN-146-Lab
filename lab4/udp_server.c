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
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: %s {port #} {destination file name}", argv[0]);
        exit(0);
    }

    int socketfd;

    uint8_t buf[256];
    memset(buf, 0, sizeof(buf));

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Could not establish socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if((bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) < 0){
        perror("Could not bind server addr to socket");
        exit(1);
    }
    
    FILE *dst = fopen(argv[2], "wb");
    if(!dst){
        perror("Could not open file");
        close(socketfd);
        exit(1);
    }

    int receivedAny = 0;

    while(1){
        ssize_t num_bytes = recvfrom(socketfd, buf, sizeof(buf), 0, (struct sockaddr*)&clientAddr, &addrLen);

        if (num_bytes < 0) {
            perror("Receive failed");
            close(socketfd);
            fclose(dst);
            exit(1);
        }

        receivedAny = 1;


        if (fwrite(buf, 1, (size_t)num_bytes, dst) != (size_t)num_bytes) {
            perror("Could not write to file");
            close(socketfd);
            fclose(dst);
            exit(1);
        }

        if ((size_t)num_bytes < sizeof(buf)) {
            break;
        }

        memset(buf, 0, sizeof(buf));
    }

    close(socketfd);
    fclose(dst);
    exit(0);
}