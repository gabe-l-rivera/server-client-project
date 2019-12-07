/*
File: filed.c
Authors: Seth Rice and Gabe Rivera
Description: File.c is our file server and accepts the following set of valid requests: newKey, fileGet, and fileDigest. To invoke, the command 'filed 5678 987654', would start the file server listening for connections at port 5678 with 987654 as the secretKey.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct varStruct { // varStruct holds variables that are sent from the client program
    int requestType, completionStatus;
    unsigned int secretKey, newClientKey;
    char fileName[100];
    char padding[2];
} varStruct;

struct servertoclient { // servertoclient holds the byte-formatted message that is sent from server to client as a repsonce to a client call
    char returnCode;
    char padding[3];
    unsigned short int valLength;
} servertoclient;


void error(const char *message){ // error message for failed socket calls
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]){
    if(argc < 2){ // check for filename, port, secret key
           printf("Error: Missing arguments.\n");
           exit(1);
       }

    int sockfd, newsockfd, portno, readFromServer, writeFromServer; // sockfd = file descriptor, newsockfd = updated file descriptor, portno = port num
    unsigned int secretKeyServer; // variable for secret key stored in server
    portno = atoi(argv[1]);
    secretKeyServer = atoi(argv[2]);
    //printf("Sec key from server: %i\n", secretKeyServer); // debug code

    struct sockaddr_in serv_addr, cli_addr; // basic structure for all syscalls and functions that deal with internet addresses.
    socklen_t clilen; // data type, 32-bits
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // initial file descriptor

    if(sockfd < 0){ // if  error in socket fuction
        error("Error opening socket.");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));  // clears any data in serv_addr

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); // host to network short

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ // binding
        error("Error in binding.");
    }

    while(1) {
        listen(sockfd, 3); // 3 = max limit of clients: newKey, fileGet, fileDigest
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //new socket file descriptor

        if (newsockfd < 0) {
            error("Error on Accept.");
        }

        readFromServer = read(newsockfd, &varStruct, sizeof(varStruct));
        if (readFromServer < 0)
        {
            fprintf(stderr, "Error on read.\n");
        }
        // debug code
        //printf("newKey vars: sec key from server = %i and sec key from newKey client = %i.\n", secretKeyServer, varStruct.secretKey);
        //printf("fileGet vars: sec key from server = %i and new key from fileGet client = %i.\n", secretKeyServer, varStruct.newClientKey);
        //printf("Name of file: %s\n", varStruct.fileName);

        switch(varStruct.requestType)
        {
            case 0:
                printf("Secret key = %i\n", secretKeyServer);
                printf("Request type = newKey\n");
                printf("Detail = %i\n", varStruct.newClientKey);
                if (varStruct.secretKey == secretKeyServer) { // assign the secret key to the new key
                    printf("Completion = success\n");
                    secretKeyServer = varStruct.newClientKey;
                }
                else {
                    printf("Completion = failure\n");
                }
                printf("--------------------------\n");
                break;
            case 1:
                if(varStruct.newClientKey == secretKeyServer){
                    printf("Secret key = %i\n", secretKeyServer);
                    printf("Request type = fileGet\n");
                    printf("Detail = %s\n", varStruct.fileName);
                    if (varStruct.completionStatus == 0) {
                        printf("Completion = success\n");
                        servertoclient.valLength = sizeof(varStruct.fileName);
                        servertoclient.returnCode = 0; // success
                    }else {
                        printf("Completion = failure\n");
                        servertoclient.returnCode = 1; // failure
                    }

                    printf("--------------------------\n");

                    break;
                }
            default:
                break;

        }
        writeFromServer = write(newsockfd, &servertoclient, sizeof(servertoclient));
        if (writeFromServer < 0)
        {
            fprintf(stderr, "Error on write.\n");
        }
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}

