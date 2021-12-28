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
#include <stdbool.h>
#include <string.h>


const char *board_alphabet[] = {
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1", "I1", "J1",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2", "I2", "J2",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3", "I3", "J3",
    "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4", "I4", "J4",
    "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5", "I5", "J5",
    "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6", "I6", "J6",
    "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7", "I7", "J7",
    "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "I8", "J8",
    "A9", "B9", "C9", "D9", "E9", "F9", "G9", "H9", "I9", "J9",
    "A10", "B10", "C10", "D10", "E10", "F10", "G10", "H10", "I10", "J10"
    };


struct square{
    char name[256];
    bool ship;
    bool clicked;
};


struct ship{
    char name[256];
    struct square squares[10];
    bool is_sunk;
};


struct board{
    struct square board[100];
    struct ship ships[10];
    unsigned int ships_count;
    bool all_sunk;
};


void output_ship(const struct ship *ship){

    printf("printing ship position ... \n\n");
    printf("name of ship: %s\n\n", ship->name);

    for (unsigned i = 0; i < sizeof(ship->squares)/sizeof(ship->squares[0]); i++){
        printf("%s ", ship->squares[i].name);
    }
    printf("\n\n");
}


void output_board(const struct board *board, unsigned int size_board){
    
    for (unsigned int i = 0; i < board->ships_count; i++){
        output_ship(&board->ships[i]);
        printf("\n");
    }
    printf("\n");
}


void board_init(struct board *board, unsigned int size_board){

    board->all_sunk = false;
    board->ships_count = 0;
    for (unsigned int i = 0; i < size_board; i++){
        strcpy(board->board[i].name, board_alphabet[i]);
        board->board[i].clicked = false;
    }
}


void ships_init(struct board *board){

    // SHIP 1
    strcpy(board->ships[0].squares[0].name, "A1");
    strcpy(board->ships[0].squares[1].name, "A2");
    strcpy(board->ships[0].name, "SHIP 1");
    board->ships_count += 1;

    // SHIP 2
    strcpy(board->ships[1].squares[0].name, "J1");
    strcpy(board->ships[1].squares[1].name, "J2");
    strcpy(board->ships[1].squares[2].name, "J3");
    strcpy(board->ships[1].name, "SHIP 2");
    board->ships_count += 1;
}


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

    struct board board;
    board_init(&board, 100);
    ships_init(&board);
    output_board(&board, 100);

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
