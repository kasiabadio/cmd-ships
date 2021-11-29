#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>


int main(){

    char message[256];
    char buffer[256];
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // Create the socket
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == -1){
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    //Configure settings of the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1100);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.12");

    //Connect the socket to the server using the address
    addr_size = sizeof(serverAddr);
    if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size) < 0){
        close(clientSocket);
        perror("Could not connect");
    }
    
    
    while(1){

        printf("Type message: \n");
        fgets(message, 256, stdin);
        message[strcspn(message, "\n")] = 0;
        if (strcmp(message, "exit\n") == 0)
        {
            close(clientSocket);
            exit(EXIT_SUCCESS);
        }

        if ( send(clientSocket, message, strlen(message), 0) < 0)
        {
                printf("Send failed\n");
                close(clientSocket);
                exit(EXIT_FAILURE);
        }

        //Read the message from the server into the buffer
        if ( recv(clientSocket, buffer, 256, 0) < 0 )
        {
            printf("Receive failed\n");
            close(clientSocket);
            exit(EXIT_FAILURE);
        }
        //Print the received message
        printf("Data received: %s\n", buffer);

        memset(&message, 0, sizeof (message));
    }

    
    close(clientSocket);
    return 0;
}
