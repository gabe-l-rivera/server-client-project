/*
 File: newKey.c
 Authors: Seth Rice and Gabe Rivera
 Description: newKey.c asks the server to set the secret key to a new value. This value must be a valid integer in the range 0 to 232-1. The command must include the current secret key as a parameter when called. The client prints either success, failure (if the server rejects the command) or invalid (if the value is invalid).
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct newKey { // structure for sending client variables
    int requestType, completionStatus;
    unsigned int secretKey, newClientKey;
    char fileName[100];
} newKey;


void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    int sockfd, portno, writeFromClient;
    struct sockaddr_in serv_addr;
    struct hostent *server; // used to store info about a given host

    /* Ensure the required parameters are provided. */
    if(argc < 5){
        printf("Error: Missing arguments.\n");
        exit(EXIT_FAILURE);
    }


    if (strcmp(argv[0], "./newKey") == 0){
        newKey.requestType = 0; // indicates the ./newKey command
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("Error openening socket.");
    }

    /* Store arguments into variables to use for later. */
    newKey.secretKey = atoi(argv[3]);
    printf("secret key: %i\n",newKey.secretKey);
    newKey.newClientKey = atoi(argv[4]);
    printf("new key: %i\n",newKey.newClientKey);
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);


    /* Get the length of argv[3] and argv[4] */
    size_t secretKeyLength = strlen(argv[3]);
    size_t newKeyLength = strlen(argv[4]);

    /* Ensure secretKey is a valid unsigned integer. */
    for (int i = 0; i < secretKeyLength; i++)
    {
        if (isdigit(argv[3][i]) == 0)
        {
            fprintf(stderr, "Invalid\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Ensure newKey is a valid unsigned integer. */
    for (int i = 0; i < newKeyLength; i++)
    {
        if (isdigit(argv[4][i]) == 0)
        {
            fprintf(stderr, "Invalid\n");
            exit(EXIT_FAILURE);
        }
    }

    if(server == NULL){
        fprintf(stderr, "Error, no such host.\n");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));   //clear serv_addr
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server -> h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length); // copy n bytes from *server to serv_addr
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("Connection failed.");
    }


    writeFromClient = write(sockfd, &newKey, sizeof(newKey));
    if (writeFromClient < 0)
    {
        fprintf(stderr, "Error on write.\n");
    }

    close(sockfd);
    return 0;

}
