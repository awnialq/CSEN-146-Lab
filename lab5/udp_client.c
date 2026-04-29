// Name: Awni AlQuraini
// Date: 04/22/2026
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
#include <netdb.h>

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Usage: %s {server ip} {port #} {file to send}", argv[0]);
        exit(1);
    }

    int socketfd;

    uint8_t buf[256];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr);

    struct hostent *host;
    host = gethostbyname(argv[1]);
    if(host == NULL){
        perror("Could not resolve host");
        exit(1);
    }

    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Could not establish socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    memcpy(&serverAddr.sin_addr, host->h_addr_list[0], host->h_length);

    FILE *src = fopen(argv[3], "r");
    if(! src){
        perror("Could not open file");
        close(socketfd);
        exit(1);
    }
	size_t bytes_read;

	while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
        ssize_t sent = sendto(socketfd, buf, bytes_read, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (sent < 0) {
            perror("Could not send data");
            close(socketfd);
            fclose(src);
            exit(1);
        }
	}


    close(socketfd);
	fclose(src);

    exit(0);
}