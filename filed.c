/*
 File: filed.c
 Authors: Seth Rice and Gabe Rivera
 Description: File.c is our file server and accepts the following set of valid requests: newKey, fileGet, and fileDigest. To invoke, the command 'filed 5678 987654', would start the file server listening for connections at port 5678 with 987654 as the secretKey.
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

struct varStruct { // varStruct holds variables that are sent from the client program
    int requestType, completionStatus;
    unsigned int secretKey, newClientKey;
    char fileName[100];
    char buffer[100];
} varStruct;

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
    if(argc < 2){ // check for filename, port, secret key
        printf("Error: Missing arguments.\n");
        exit(1);
    }
    
    int sockfd, newsockfd, portno, readFromServer, writeFromServer; // sockfd = file descriptor, newsockfd = updated file descriptor, portno = port num
    unsigned int secretKeyServer; // variable for secret key stored in server
    char sha [100];
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
        //printf("Request type: %i\n", varStruct.requestType);
        switch(varStruct.requestType)
        {
            case 0:
                printf("Secret key = %i\n", secretKeyServer);
                printf("Request type = newKey\n");
                printf("Detail = %i\n", varStruct.newClientKey);
                if (varStruct.secretKey == secretKeyServer) { // assign the secret key to the new key
                    printf("Completion = success\n");
                    secretKeyServer = varStruct.newClientKey;
                    servertoclient.returnCode = (char)0; // success
                }
                else {
                    printf("Completion = failure\n");
                    servertoclient.returnCode = (char)-1; // failure
                    
                }
                printf("--------------------------\n");
                break;
            case 1:
                //printf("in case 1\n");
                //printf("new client key: %i\n", varStruct.newClientKey);
                if(varStruct.newClientKey == secretKeyServer){
                    //printf("in case 1\n");
                    FILE *fp;
                    int inc = 0;
                    //char buffer[100]; // size = 100 for reading at most 100 bytes
                    fp = fopen(varStruct.fileName, "rb"); // open arg[4] and read it as a binary file
                    //varStruct.completionStatus = 0;
                    if(fp==NULL){
                        //printf("Error opening file. Chance of NULL file pointer.\n");
                        printf("Secret key = %i\n", secretKeyServer);
                        printf("Request type = fileGet\n");
                        printf("Detail = %s\n", varStruct.fileName);
                        printf("%s: no such file or directory.\n", varStruct.fileName);
                        printf("Completion = failure\n");
                        printf("--------------------------\n");
                        servertoclient.returnCode = (char)-1; // fail
                        break;
                    }
                    varStruct.completionStatus = 0;
                    //printf("made it past the file\n");
                    // loop thru byte-by-byte till buffer is full or EOF
                    while (!feof(fp) && inc < 50) {
                        servertoclient.buffer[inc] = fgetc(fp);
                        //printf("%02x", servertoclient.buffer[inc]);
                        inc++;
                    }
                    fclose(fp);
                    //printf("\n");
                    //printf("filled buffer\n");
                    printf("Secret key = %i\n", secretKeyServer);
                    printf("Request type = fileGet\n");
                    printf("Detail = %s\n", varStruct.fileName);
                    printf("Completion = success\n");
                    servertoclient.valLength = sizeof(varStruct.fileName);
                    servertoclient.returnCode = (char)0; // success
                    printf("--------------------------\n");
                    break;
                }else{
                    servertoclient.returnCode = (char)-1;
                    printf("Secret key = %i.\n", secretKeyServer);
                    printf("Wong key.\n");
                    printf("--------------------------\n");
                    break;
                }
                break;
            case 2:
                if (varStruct.newClientKey == secretKeyServer) {
                    int savedOutput,savedError;
                    char shaSysCall[121];
                    strcpy(shaSysCall, "/usr/bin/sha256sum ");
                    FILE *fp;
                    fp = fopen(varStruct.fileName, "r");
                    if(fp == NULL){
                        servertoclient.returnCode = (char)-1;
                        //write(newsockfd, &servertoclient, sizeof(servertoclient));
                        printf("Secret key = %i\n", secretKeyServer);
                        printf("Request type = fileDigest\n");
                        printf("%s: no such file or directory.\n", varStruct.fileName);
                        printf("Detail = %s\n", varStruct.fileName);
                        printf("Completion = failure\n");
                        printf("--------------------------\n");
                        break;
                    }
                    //debug code
                    //printf("%s\n",servertoclient.shaFileName);
                    savedOutput = dup(1);
                    savedError = dup(2);
                    strcat(shaSysCall, varStruct.fileName);
                    //printf("before system%s\n", sysCall);
                    int pipeFeed[2];
                    pipe(pipeFeed);
                    dup2(pipeFeed[1], 1);
                    dup2(pipeFeed[1], 2);
                    system(shaSysCall);
                    //printf("after system:%s\n", shaSysCall);
                    read(pipeFeed[0], servertoclient.shaFileData, 100);
                    //printf("\n");
                    //printf("%s\n",servertoclient.shaFileData);
                    //printf("\n");
                    close(pipeFeed[1]);
                    close(pipeFeed[0]);
                    dup2(savedError, 2);
                    dup2(savedOutput, 1);
                    close(savedError);
                    close(savedOutput);
                    printf("Secret key = %i\n", secretKeyServer);
                    printf("Request type = fileDigest\n");
                    printf("Detail = %s\n", varStruct.fileName);
                    printf("Completion = success\n");
                    servertoclient.returnCode = (char)0; // success
                    printf("--------------------------\n");
                    break;
                }else{
                    servertoclient.returnCode = (char)-1;
                    printf("Secret key = %i.\n", secretKeyServer);
                    printf("Wong key.\n");
                    printf("--------------------------\n");
                    break;
                }
                break;
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

 
