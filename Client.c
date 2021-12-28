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
        board->board[i].clicked = false;
    }
}


void ships_init(struct board *board){

    // SHIP 1
    strcpy(board->ships[0].squares[0].name, "A9");
    strcpy(board->ships[0].squares[1].name, "B9");
    strcpy(board->ships[0].name, "SHIP 1");
    board->ships_count += 1;

    // SHIP 2
    strcpy(board->ships[1].squares[0].name, "F6");
    strcpy(board->ships[1].squares[1].name, "G6");
    strcpy(board->ships[1].squares[2].name, "H6");
    strcpy(board->ships[1].name, "SHIP 2");
    board->ships_count += 1;
}


int main(){

    
    char message[256]; //  client move --> validate by server
    char buffer[256]; //  opponent move

    // TODO: GUI
    // client board
    struct board board;
    board_init(&board, 100);
    ships_init(&board);
    output_board(&board, 100);

    // opponent board
    struct board opponent_board;
    board_init(&board, 100);
    
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
