// Name: Awni AlQuraini
// Date: 04/22/2026
// Title: Lab 5 - Part 1
// Description: UDP server implementation for serving a specific file w/ Stop and Wait error handling

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

int ack_seq_creator(int seq_num){
    // randomly decide if you send the correct ACK number or the wrong one
    if(rand() % 10 < 7) {
        return seq_num;
    } else {
        return (seq_num == 0) ? 1 : 0;
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: %s {port #} {destination file name}", argv[0]);
        exit(0);
    }

    int socketfd;

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

    uint8_t seq_num = 0; // expected sequence number starts at 0

    while(1){
        struct packet p;
        ssize_t num_bytes = recvfrom(socketfd, &p, sizeof(p), 0, (struct sockaddr*)&clientAddr, &addrLen);

        if (num_bytes < 0) {
            perror("Receive failed");
            close(socketfd);
            fclose(dst);
            exit(1);
        }

        if (p.len == 0) {
            struct packet ack;
            memset(&ack, 0, sizeof(ack));
            ack.seq_ack = ack_seq_creator(p.seq_ack);
            ack.len = 0;
            ack.checksum = checksum_creator(&ack);
            ssize_t sent = sendto(socketfd, &ack, sizeof(ack), 0, (struct sockaddr *)&clientAddr, addrLen);
            if (sent < 0) {
                perror("Could not send ACK for termination packet");
                close(socketfd);
                fclose(dst);
                exit(1);
            }
            printf("Received termination packet, closing connection.\n");
            break;
        }

        if (p.seq_ack == seq_num && p.checksum == compute_checksum(&p)) {
            printf("Received packet with expected sequence number %d\n", seq_num);
            fwrite(p.data, 1, p.len, dst);

            struct packet ack;
            memset(&ack, 0, sizeof(ack));
            ack.seq_ack = ack_seq_creator(seq_num);
            ack.len = 0;
            ack.checksum = checksum_creator(&ack);
            ssize_t sent = sendto(socketfd, &ack, sizeof(ack), 0, (struct sockaddr *)&clientAddr, addrLen);
            if (sent < 0) {
                perror("Could not send ACK");
                close(socketfd);
                fclose(dst);
                exit(1);
            }
            
            // change sequence number to the next expected one
            if (seq_num == 0) {
                seq_num = 1;
            } else {
                seq_num = 0;
            }
        } else if (p.seq_ack != seq_num) {
            printf("Received packet with wrong sequence number %d, expected %d\n", p.seq_ack, seq_num);
            struct packet ack;            
            memset(&ack, 0, sizeof(ack));            
            ack.seq_ack = (seq_num == 0) ? 1 : 0;
            ack.len = 0;
            ack.checksum = checksum_creator(&ack);
            ssize_t sent = sendto(socketfd, &ack, sizeof(ack), 0, (struct sockaddr *)&clientAddr, addrLen);
            if (sent < 0) {
                perror("Could not resend ACK");
                close(socketfd);
                fclose(dst);
                exit(1);
            }
        } else {
            printf("Received corrupted packet with sequence number %d\n", p.seq_ack);
            struct packet ack;            
            memset(&ack, 0, sizeof(ack));            
            ack.seq_ack = (seq_num == 0) ? 1 : 0; // repeat the last valid ACK number
            ack.len = 0;
            ack.checksum = checksum_creator(&ack);
            ssize_t sent = sendto(socketfd, &ack, sizeof(ack), 0, (struct sockaddr *)&clientAddr, addrLen);
            if (sent < 0) {
                perror("Could not resend ACK");
                close(socketfd);
                fclose(dst);
                exit(1);
            }
        }
    }

    close(socketfd);
    fclose(dst);
    exit(0);
}