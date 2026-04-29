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
#include <fcntl.h>

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

int wait_for_ack_or_timeout(int sockfd){
    struct timeval tv;
    fd_set readfds;

    // Ensure recvfrom does not block after select says no data is available.
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl failed");
        return -1;
    }

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    return select(sockfd + 1, &readfds, NULL, NULL, &tv);
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
    uint8_t seq_num = 0; // sequence number starts at 0

	while ((bytes_read = fread(buf, 10, sizeof(buf), src)) > 0) {
        struct packet p;
        p.seq_ack = seq_num; // set the sequence number for the packet
        p.len = bytes_read; // how many bytes are in the packet
        memcpy(p.data, buf, bytes_read);
        p.checksum = checksum_creator(&p);
        ssize_t sent = sendto(socketfd, &p, sizeof(struct packet), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (sent < 0) {
            perror("Could not send data");
            close(socketfd);
            fclose(src);
            exit(1);
        }
        
        int return_value;

        return_value = wait_for_ack_or_timeout(socketfd);
        if (return_value == 0){
            printf("Timeout occurred, resending packet sequence number %d\n", seq_num);
            continue;
        }
        
        struct packet ack;
        ssize_t num_bytes = recvfrom(socketfd, &ack, sizeof(struct packet), 0, (struct sockaddr *)&serverAddr, &addrLen);
        
        if(ack.seq_ack == seq_num && ack.checksum == compute_checksum(&ack)){
            printf("Received ACK for sequence number %d\n", seq_num);
        } else {
            printf("Received wrong/corrupted ACK for seq number %d\n", seq_num);
            continue;
        }

        if (seq_num == 0) {
            seq_num = 1; // toggle sequence number for next packet
        } else {
            seq_num = 0;
        }
	}

    // send termination packet
    int resent_cnt = 0;
    while (1) {
        struct packet final_packet;
        memset(&final_packet, 0, sizeof(final_packet));
        final_packet.seq_ack = seq_num;
        final_packet.len = 0;
        final_packet.checksum = checksum_creator(&final_packet);

        ssize_t final_sent = sendto(socketfd, &final_packet, sizeof(struct packet), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (final_sent < 0) {
            perror("Could not send final packet");
            break;
        }

        int return_value = wait_for_ack_or_timeout(socketfd);

        if (return_value == 0) {
            if (resent_cnt == 3) {
                printf("Maxed out attempts for receiving final ACK\n");
                break;
            }
            resent_cnt++;
            printf("Timeout waiting for final ACK, resending final packet (%d/3)\n", resent_cnt);
            continue;
        }

        struct packet ack;
        ssize_t num_bytes = recvfrom(socketfd, &ack, sizeof(struct packet), 0, (struct sockaddr *)&serverAddr, &addrLen);

        if (ack.seq_ack == seq_num && ack.checksum == compute_checksum(&ack)) {
            printf("Received ACK for final packet sequence number %d\n", seq_num);
            break;
        }

        if (resent_cnt == 3) {
            printf("No valid final ACK after 3 resends; exiting client.\n");
            break;
        }
        resent_cnt++;
        printf("Wrong/corrupted final ACK, resending final packet (%d/3)\n", resent_cnt);
    }

    close(socketfd);
	fclose(src);

    exit(0);
}