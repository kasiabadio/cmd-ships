#include "Utility.h"

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
        board->board[i].ship = NO_SHIP;
        board->board[i].clicked = false;

    }
}


// void validate_move(){
    // TODO: check if already clicked 
    // TODO: check if there is a ship
    // TODO: if successful, mark

    // send response to client about move
// }


// it must check it all the time
// void validate_ship_placement(){
    // TODO: check if all squares of a ship can fit into a board
    // TODO: if successful, mark

    // send response to client about move

// }


void mark_ship_border(struct board *board, unsigned int ship_position){
    // TODO: check if there is another ship
    // TODO: else mark all neighbors clockwise (up -> right -> down -> left)
}

void ships_init(struct board *board){

    // SHIP 1
    // add information in board->ships tab
    strcpy(board->ships[0].squares[0].name, "A1");
    strcpy(board->ships[0].squares[1].name, "A2");
    strcpy(board->ships[0].name, "SHIP 1");
    board->ships_count += 1;

    // add information in board->board tab
    board->board[A1].ship = SHIP;
    board->board[A1].ship = SHIP;
    // TODO: mark_ship_border

    // SHIP 2
    strcpy(board->ships[1].squares[0].name, "J1");
    strcpy(board->ships[1].squares[1].name, "J2");
    strcpy(board->ships[1].squares[2].name, "J3");
    strcpy(board->ships[1].name, "SHIP 2");
    board->ships_count += 1;

    board->board[J1].ship = SHIP;
    board->board[J2].ship = SHIP;
    board->board[J3].ship = SHIP;
    // TODO: mark_ship_border
}


char client_message[256];
char buffer[256];


void *socketThread(void *arg)
{
    pid_t x = syscall(__NR_gettid);
    printf("Thread id: %d\n", x);
  
    int newSocket = *((int *)arg);
    int n;
    while(1){

        // TODO: how to know to whom send message with opponent move
        n = recv(newSocket, client_message, 256, 0);
        if (n == 0){
            break;
        }

        printf("%s\n", client_message);
        // TODO: validate client move

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

    // TODO: connect two clients to enable game
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
