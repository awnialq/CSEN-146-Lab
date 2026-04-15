// Name: Awni AlQuraini 
// Date: 04/15/2026
// Title: Lab 3 - Part 1
// Description: The implementation of TCP for the client

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    int sockfd;
    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }



    exit(EXIT_SUCCESS);
}