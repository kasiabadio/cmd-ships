#include "Utility.h"


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



void board_init(struct board *board, unsigned int size_board){

    board->all_sunk = false;
    board->ships_count = 0;

    for (unsigned int i = 0; i < size_board; i++){

        strcpy(board->board[i].name, board_alphabet[i]);
        board->board[i].ship = NO_SHIP;
        board->board[i].clicked = false;

    }
}


void mark_ship_border(struct board *board, unsigned int ship_position){
    // TODO: check if there is another ship
    // TODO: else mark all neighbors clockwise (up -> right -> down -> left)
}


void validate_move(char* move, struct board *board){
    // TODO: check if already clicked 
    // TODO: check if there is a ship
    // TODO: if successful, mark as clicked
}


void validate(char *client_message, int my_s, int other_s, struct board *opponent_board){

    bool validated = false;
    
    for (int i = 0; i < 100; i++){
        if (strcmp(board_alphabet[i], client_message) == 0){
            validated = true;
            break;
        }
    }
    
    if (validated){ // move is valid, send message to opponent 
        char *message = malloc(sizeof(client_message));

        validate_move(client_message, opponent_board);

        strcpy(message, client_message);
        send(other_s, message, strlen(message), 0);
        memset(&client_message, 0, sizeof(client_message));
        free(message);

        // TODO: server checks if the ship is sunk, if true -> send message on my_s

    } else { // move is invalid, send error to sender
        char *message = malloc(sizeof("Move is not valid."));
        strcpy(message, "Move is not valid.");
        send(my_s, "Move is not valid.", 19, 0);
        free(message);
    }

}


// void validate_placement_move(char* move, struct board *board){
//     // TODO: check if there is a ship or ship border
//     // TODO: if successful, mark square ship variable
// }


// void validate_placement(char *client_message, int my_s, struct board *my_board){

//     bool validated = false;
    
//     // TODO: check if all squares of a ship can fit into a board

//     if (validated){ // placement is valid

//         // TODO: do the placement

//         char *message = malloc(sizeof("Valid ship placement"));
//         strcpy(message, "Valid ship placement");
//         send(my_s, "Valid ship placement", 21, 0);
//         memset(&client_message, 0, sizeof(client_message));
//         free(message);

//     } else { // move is invalid, send error to sender
//         char *message = malloc(sizeof("Invalid ship placement"));
//         strcpy(message, "Invalid ship placement");
//         send(my_s, "Invalid ship placement", 23, 0);
//         free(message);
//     }
// }


bool is_placement_complete(struct board *my_board){
    // complete when there are
    // 4 ships of size 1 
    // 3 ships of size 2
    // 2 ships of size 3
    // 1 ship of size 4

    if (my_board->ships_count < 10) return false;
    return true;
}


// parse message from client
struct ship parse_ship_placement(char *message){
    
    struct ship ship;
    printf("%d\n", strlen(message));
    if (strlen(message) % 2 != 0 || strlen(message) > 9) return ship;
  
    // message must contain pattern letter+number

    // TODO: validate placement move

    return ship;
}


char client_message1[256];
char client_message2[256];
char buffer[256];


void *socketThread(void *arg)
{
    pid_t x = syscall(__NR_gettid);
    printf("Thread id: %d\n", x);
  
    struct thread_arg *targs = arg;
    int s1 = targs->s1;
    int s2 = targs->s2;
    int n1, n2;

    // construct boards
    struct board board1;
    board_init(&board1, 100);

    struct board board2;
    board_init(&board2, 100);

    // TODO: placement of ships by both opponents
    while (is_placement_complete(&board1) == false && is_placement_complete(&board2) == false){

        // receive placement from s1
        n1 = recv(s1, client_message1, 256, MSG_DONTWAIT);
        if (n1 > 0) {

            printf("SHIP PLACEMENT\n");
            // parse_ship_placement(&client_message1);

            for (int ln = 0; ln < strlen(client_message1); ln += 2){
                char subbuff[3];
                memcpy(subbuff, &client_message1[ln], 2);
                subbuff[2] = '\0';
                printf("move: %s\n", subbuff);
                if (in_alphabet(subbuff)) printf("move is valid: %s\n", subbuff);
                
            }
            memset(&client_message1, 0, sizeof(client_message1));
        }

        // receive placement from s2
        n2 = recv(s2, client_message2, 256, MSG_DONTWAIT);
        if (n2 > 0) {

            printf("SHIP PLACEMENT\n");
            for (int ln = 0; ln < strlen(client_message2); ln += 2){
                char subbuff[3];
                memcpy(subbuff, &client_message2[ln], 2);
                subbuff[2] = '\0';
                printf("move: %s\n", subbuff);
                if (in_alphabet(subbuff)) printf("move is valid: %s\n", subbuff);
                
            }
            memset(&client_message2, 0, sizeof(client_message2));
        } 

    }

    // TODO: validate placement 

    while(1){

        n1 = recv(s1, client_message1, 256, MSG_DONTWAIT);

        if (n1 > 0) {
            printf("%s\n", client_message1);
            validate(client_message1, s1, s2, &board2);
            memset(&client_message1, 0, sizeof(client_message1));
        }
        
        n2 = recv(s2, client_message2, 256, MSG_DONTWAIT);

        if (n2 > 0) {
            printf("%s\n", client_message2);
            validate(client_message2, s2, s1, &board1);
            memset(&client_message2, 0, sizeof(client_message2));
        } 


        if (n1 == 0 || n2 == 0){
            break;
        }

    }

    free(targs);

    pthread_exit(NULL);
}



int main(){

    pid_t x = syscall(__NR_gettid);
    printf("Main id: %d\n", x);

    // test
    struct board board;
    board_init(&board, 100);
    ships_init(&board);
    output_board(&board, 100);

    int serverSocket, socketClient1, socketClient2, socketClient;
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

        unsigned int socket_counter = 0;
        struct thread_arg targs;

        while (socket_counter < 2 && ((socketClient = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size)) != 0)){
            printf("Accepted! SocketClient: %d\n", socketClient);
            if (socket_counter == 0) targs.s1 = socketClient;
            else targs.s2 = socketClient;
            socket_counter++;
        }

        // pointer to pthread object, attributes, thread function
        // Create new socket for each game
        if(pthread_create(&thread_id, NULL, socketThread, (void*)&targs) != 0)
        {
            printf("Failed to create thread\n");
        }else{
            printf("Created new thread\n");
        }

        pthread_detach(thread_id);
    }
    
    return 0;
}
