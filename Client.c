#include "Utility.h"
#include "Placement.c"

int main(){

    bool game = false;
    char message[256]; 
    char buffer[256]; 

    // client board
    struct board my_board;
    board_init(&my_board, 100);


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

    int pid;
    if ((pid = fork()) == 0){ // send message

        while(1){

            printf("Type message: \n");
            fgets(message, 256, stdin);
            message[strcspn(message, "\n")] = 0;
            if (send(clientSocket, message, strlen(message), 0) < 0)
            {
                    printf("Send failed\n");
                    close(clientSocket);
                    exit(EXIT_FAILURE);
            }

            if (strcmp(message, "exit") == 0)
            {
                close(clientSocket);
                exit(EXIT_SUCCESS);
            }
            memset(&message, 0, sizeof (message));           
        }
    }
    else // receive message
    {
        while(1){

            int x;
            // Read the message from the server into the buffer
            if ((x = recv(clientSocket, buffer, 256, 0)) < 0)
            {
                printf("Receive failed\n");
                close(clientSocket);
                exit(EXIT_FAILURE);
            }  
                
            if ((strcmp("Game has started.", buffer)) == 0) game = true;
            
            // shutdown
            if (x == 0){
                kill(pid, 9);
                exit(EXIT_SUCCESS);
            } 

            if (!game){

                // split buffer 
                char letter[2], subbuff[3], start[3], end[3];
                memcpy(letter, &buffer[0], 1);
                letter[1] = '\0';

                for (int l = 1; l < strlen(buffer); l += 2){
                    memcpy(subbuff, &buffer[l], 2);
                    subbuff[2] = '\0';
                    if (l == 1) strcpy(start, subbuff);
                    if (l == strlen(buffer) - 2) strcpy(end, subbuff);
                }

                // mark position
                int start_pos = find_board_position(start);
                int end_pos = find_board_position(end);

                if (strcmp(letter, "H") == 0){
                    mark(&my_board, 'H', start_pos, end_pos);
                    printf("Board after placement:\n");
                    output_board_client(&my_board);

                } else if (strcmp(letter, "V") == 0){
                    mark(&my_board, 'V', start_pos, end_pos);
                    printf("Board after placement:\n");
                    output_board_client(&my_board);
                }
            } else {
                printf("Received message: %s\n", buffer);
            }
            memset(&buffer, 0, sizeof (buffer));
        }
    }
    close(clientSocket);
    return 0;
}
