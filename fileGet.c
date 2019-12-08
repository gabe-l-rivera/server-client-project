/*
 File: fileGet.c
 Authors: Seth Rice and Gabe Rivera
 Description: fileGet.c is one of our client files that will ask the server to respond with the first 100-bytes of a given file, or fewer if the file is shorter than 100-bytes. The server responds with failure if the file does not exist or is inaccessible. The client prints the bytes it receives from the server in %02x format, or the word failure.
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

struct fileGet { // structure for sending client variables to server
    int requestType, completionStatus;
    unsigned int secretKey, newClientKey;
    char fileName[100];
    char buffer[100];
} fileGet;

struct servertoclient { // servertoclient holds the byte-formatted message that is sent from server to client as a repsonce to a client call
    char returnCode; // character for the purpose of it being one byte
    char padding[3];
    unsigned short int valLength;
    char buffer[100];
    char shaFileData[100];
} servertoclient;

void error(const char *msg){
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[]){
    int sockfd, portno, writeFromClient, readFromClient;
    struct sockaddr_in serv_addr;
    struct hostent *server; // used to store info about a given host
    
    /* Ensure the required parameters are provided. */
    if(argc < 5){
        printf("Error: Missing arguments.\n");
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(argv[0], "./fileGet") == 0){
        fileGet.requestType = 1; // indicates the ./newKey command
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("Error openening socket.");
    }
    
    /* Store arguments into variables to use for later. */
    fileGet.newClientKey = atoi(argv[3]);
    //printf("new client key %i\n", fileGet.newClientKey);
    strcpy(fileGet.fileName, argv[4]);
    // printf("File name = %s\n", fileGet.fileName); // debug for printing file name
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);
    
    if(server == NULL){
        fprintf(stderr, "Error, no such host.");
    }
    size_t secretKeyLength = strlen(argv[3]);
    /*Ensure secretKey is a valid unsigned integer.*/
    for (int i = 0; i < secretKeyLength; i++)
    {
        if (isdigit(argv[3][i]) == 0)
        {
            fprintf(stderr, "Invalid\n");
            exit(EXIT_FAILURE);
        }
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));   //clear serv_addr
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server -> h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length); // copy n bytes from *server to serv_addr
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("Connection failed.");
    }
    writeFromClient = write(sockfd, &fileGet, sizeof(fileGet));
    readFromClient = read(sockfd, &servertoclient, sizeof(servertoclient));
    
    if (readFromClient < 0)
    {
        fprintf(stderr, "Read error.\n");
    }
    if (writeFromClient < 0)
    {
        fprintf(stderr, "Write error.\n");
    }
    
    if(servertoclient.returnCode == 0){
        for(int i = 0; i < 50; i++){
            printf("%02x", servertoclient.buffer[i]);
        }
        printf("\n");
    }else{
        printf("Failiure in fileGet.\n");
        
    }
    close(sockfd);
    return 0;
    
}
