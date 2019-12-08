#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct fileDigest {
    int requestType, completionStatus;
    unsigned int secretKey, newClientKey;
    char fileName[100];
    char buffer[100];
} fileDigest;

struct servertoclient { // servertoclient holds the byte-formatted message that is sent from server to client as a repsonce to a client call
    char returnCode; // character for the purpose of it being one byte
    char padding[3];
    unsigned short int valLength;
    char buffer[100];
    char shaFileData[100];
} servertoclient;

void error(const char *message){ // error message for failed socket calls
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]){
    int sockfd, portno, writeFromClient, readFromClient;
    struct sockaddr_in serv_addr;
    struct hostent *server; // used to store info about a given host
    
    /* Ensure the required parameters are provided. */
    if(argc < 5){
        fprintf(stderr, "Usage %s hostname port key filname\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(argv[0], "./fileDigest") == 0)
    {
        fileDigest.requestType = 2;
        //printf("Request type is 2\n");
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("Error openening socket.");
    }
    
    /* Store arguments into variables to use for later. */
    // printf("File name = %s\n", fileGet.fileName);
    strcpy(fileDigest.fileName, argv[4]);
    fileDigest.newClientKey = atoi(argv[3]);
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);
    /* Get the length of argv[3] and argv[4] */
    size_t secretKeyLength = strlen(argv[3]);
    
    if(server == NULL){
        fprintf(stderr, "Error, no such host.");
    }
    
    /* Ensure secretKey is a valid unsigned integer. */
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
   
    writeFromClient = write(sockfd, &fileDigest, sizeof(fileDigest));
    readFromClient = read(sockfd, &servertoclient, sizeof(servertoclient));
    
    if (readFromClient < 0){
        fprintf(stderr, "Read error.\n");
    }
    if (writeFromClient < 0){
        fprintf(stderr, "Write error.\n");
    }
    
    if(servertoclient.returnCode == 0)
        printf("%s", servertoclient.shaFileData);
    else
        printf("Failiure.\n");
    
    close(sockfd);
    return 0;
    
}

 
