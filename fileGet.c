
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
} fileGet;

struct servertoclient { // structure for receiving server data
    char returnCode;
    char padding[3];
    unsigned short int valLength;
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
        FILE *fp;
        int inc = 0;
        char buffer[100]; // size = 100 for reading at most 100 bytes
        fp = fopen(argv[4], "rb"); // open arg[4] and read it as a binary file
        fileGet.completionStatus = 0;
        if(fp==NULL){
            fileGet.completionStatus = 1;
            printf("Error opening file. Chance of NULL file pointer.\n");
            exit(1);
        }
        // loop thru byte-by-byte till buffer is full or EOF
        while (!feof(fp) && inc < 50) {
            buffer[inc] = fgetc(fp);
            printf("%02x", buffer[inc]);
            inc++;
        }

        fclose(fp);
        printf("\n");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("Error openening socket.");
    }

    /* Store arguments into variables to use for later. */
    fileGet.newClientKey = atoi(argv[3]);
    strcpy(fileGet.fileName, argv[4]);
    // printf("File name = %s\n", fileGet.fileName); // debug for printing file name
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);

    if(server == NULL){
        fprintf(stderr, "Error, no such host.");
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
    close(sockfd);
    return 0;

}
