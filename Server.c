#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <pthread.h>
#include <sys/syscall.h>


char client_message[256];
char buffer[256];

void *socketThread(void *arg)
{
    pid_t x = syscall(__NR_gettid);
    printf("Thread id: %d\n", x);

    //rzutuje pointer z czegokolwiek na inta i pobiera z niego wartość    
    int newSocket = *((int *)arg);
    int n;
    while(1){

        n = recv(newSocket, client_message, 256, 0);
        if (n == 0){
            break;
        }

        printf("%s\n", client_message);

        char *message = malloc(sizeof(client_message));
        strcpy(message, client_message);

        send(newSocket, message, strlen(message), 0);
        memset(&client_message, 0, sizeof(client_message));
    }

    pthread_exit(NULL);
}

int main(){

    pid_t x = syscall(__NR_gettid);
    printf("Main id: %d\n", x);

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    // Create the socket
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1100);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the address struct to the socket 
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        printf("Binding failed.\n");
        exit(EXIT_FAILURE);
    }
    
    if (listen(serverSocket, 10) == 0){
        printf("Listening\n");
    } else {
        printf("Error\n");
    }

    pthread_t thread_id;

    while(1)
    {

        // Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        // pointer to pthread object, attributes, thread function
        if(pthread_create(&thread_id, NULL, socketThread, &newSocket) != 0)
        {
            printf("Failed to create thread\n");
        }else{
            printf("Created new thread\n");
        }

        pthread_detach(thread_id);
    }
    
    return 0;
}
