// Name: Awni AlQuraini
// Date: 04/22/2026
// Title: Lab 5 - Part 2
// Description: UDP client implementation for requesting a specific file with a Stop and Wait error handling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <netdb.h>
#include <time.h>

struct packet{
    int32_t seq_ack;
    int32_t len;
    uint8_t data[10];
    int32_t checksum;
};

int compute_checksum(struct packet *p){
    int32_t checksum = 0;
    uint8_t *ptr = (uint8_t *)p;
    uint8_t *end = ptr + sizeof(struct packet) - sizeof(int32_t); // don't compute checksum with checksum field

    while(ptr < end){
        checksum ^= *ptr++;
    }

    return checksum;
}

int checksum_creator(struct packet *p){
    // randomly decide if you send the actual checksum or not (it favors actually sending the proper one)
    if(rand() % 10 < 7) {
        return compute_checksum(p);
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]){
    srand(time(NULL));
    
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

	while ((bytes_read = fread(buf, 10, sizeof(buf), src)) > 0) {
        struct packet p;
        p.seq_ack = 0; // sequence number can be set to 0 for simplicity
        p.len = bytes_read;
        memcpy(p.data, buf, bytes_read);
        p.checksum = checksum_creator(&p);
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