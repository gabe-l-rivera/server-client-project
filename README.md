Files Included: filed.c, newKey.c, fileGet.c, fileDigest.c, README.txt, & makefile.

Documentation: In this project, Seth and I wrote our own file service. Consisting of two parts, we wrote a server under the name filed.c and three client programs, fileDigest.c, fileGet.c and newKey.c. The three client programs make requests to the server, which accepts the three commands newKey and fileGet. The algorithms used throughout the server, filed.c, are made up of three parts: The socket connection, receiving a structure of variables from the client, and processing the client’s data. For the socket connection, using  help from Tau Beta Pi tutors, we solved how to make the socket connections from server to client by including <sys/types.h>, <sys/socket.h>, and <netinet/in.h>. These libraries allowed us to use socket systems calls to create file descriptors and make the connections between server and client. When it came to receiving data from either client or server, our algorithm for such actions came from the two functions read() and write(), along with utilizing structures to effectively transfer the data in one single write (we chose write() instead of send because we decided not to use send()’s flag features). Finally, the algorithms used for processing data also consisted of using read() to read in the written data, from either server or client. We thought it was smart and efficient to send structures in the form of write(fileDescriptor, &structure, sizeof(struct)), so that we could pass all the variables at once, and not use multiple read()’s and write()’s.

Special Features and Limitations: Some features include in our version of the project that differ from the working program provided by Dr. Finkel are our error handling features. Essentially, to make our programs as robust, we implemented error checkers that handle non-unsigned integer values for secret and new key in newKey and a NULL file handler for fileGet. Additionally, we used variables readFromClient, writeFromClient, readFromServer and writeFromServer that were assigned the integer return value from the function calls read() and write(). This allowed us to debug error in sending/receiving data, along with making the server and client programs less error-prone. There were no known limitations in this project, all the three necessary client programs were written and included. 

Group Members: Seth Rice and Gabe Rivera | Dr. Raphael Finkel, Systems Programming CS270 University of Kentucky
