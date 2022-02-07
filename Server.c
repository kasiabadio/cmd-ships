#include "Utility.h"
#include "Placement.c"


bool is_ship_sunk(struct board *opponent_board, char *square_name){

    for (int s = 0; s < opponent_board->ships_count; s++){
        int clicked_counter = 0;
        bool ship_found = false;
        for (int i = 0; i < opponent_board->ships[s].name; i++){
            printf("   opponent_board->ships[s].squares[i].clicked: %d\n", opponent_board->ships[s].squares[i].clicked);
            printf("   opponent_board->ships[s].squares[i].name: %s\n", opponent_board->ships[s].squares[i].name);
            if (opponent_board->ships[s].squares[i].clicked == true) clicked_counter++;
            if (strcmp(opponent_board->ships[s].squares[i].name, square_name) == 0) ship_found = true;
        }
        printf("clicked_counter: %d, opponent_board->ships[s].name: %d\n", clicked_counter, opponent_board->ships[s].name);
        if ((clicked_counter == opponent_board->ships[s].name) && ship_found){ 
            opponent_board->ships[s].is_sunk = true;
            return true;
        }
    }
    return false;
}


bool check_square(struct board *opponent_board, char *square_name){

    //printf("square_name: %s\n", square_name);

    for (int i = 0; i < 100; i++){
        //printf("opponent_board->board[i].name: %s\n", opponent_board->board[i].name);
        if ((strcmp(opponent_board->board[i].name, square_name) == 0) && 
        opponent_board->board[i].ship == SHIP) { 
            opponent_board->board[i].clicked = true;

            // save clicked in ship info ships tab 
            for (int s = 0; s < opponent_board->ships_count; s++){
                for (int i = 0; i < opponent_board->ships[s].name; i++){
                    if (strcmp(opponent_board->ships[s].squares[i].name, square_name) == 0){
                        printf("Saved square: %s\n", opponent_board->ships[s].squares[i].name);
                        opponent_board->ships[s].squares[i].clicked = true;
                    }
                }
            }

            return true;
        }
    }
    return false;
}


void validate(char *client_message, int my_s, int other_s, struct board *opponent_board){

    bool validated = false;
    printf("client message: %s\n", client_message);
    for (int i = 0; i < 100; i++){
        if (strcmp(board_alphabet[i], client_message) == 0){
            validated = true;
            break;
        }
    }

    if (validated){
      
        bool is_ship = check_square(opponent_board, client_message);
        if (is_ship){

            send(my_s, "Ship!", 6, 0);
            bool is_sunk = is_ship_sunk(opponent_board, client_message);
            if (is_sunk){
                printf("SUNK!!\n");
                send(my_s, "SHIP SUNK!", 11, 0);
            } 

        } else { 
            send(my_s, "No ship!", 9, 0);
        }

    } else { // move is invalid, send error to sender
        send(my_s, "Move is not valid.", 19, 0);
    }
    
}

// ----------------------------------------- MAIN ----------------------------------------------

void *socketThread(void *arg){

    pid_t x = syscall(__NR_gettid);
    printf("Thread id: %d\n", x);
  
    struct thread_arg *targs = arg;
    int s1 = targs->s1;
    int s2 = targs->s2;
    int n1, n2, ship_size;
    

    // construct boards
    struct board board1;
    board_init(&board1, 100);

    struct board board2;
    board_init(&board2, 100);

    // placement of ships by both opponents
    while (is_placement_complete(&board1) == false || is_placement_complete(&board2) == false){

        // receive placement from s1
        n1 = recv(s1, client_message1, 256, MSG_DONTWAIT);
        if (n1 > 0) {
            
            ship_size = strlen(client_message1) / 2;
            struct ship ship;
            if (parse_ship_placement(&client_message1, &ship)){

                if (check_ship_count(ship_size, &board1)) {

                    mark_ship(&ship, ship_size, &client_message1);
                    printf("1. Ship of size %d can be placed.\n", ship_size);
                    if (!is_border_ship(&board1, &ship)){

                        printf("2. There is no other ship or border there.\n");
                        board1.ships[board1.ships_count] = ship;
                        board1.ships_count++;
                        mark_ship_and_border(&board1, &ship, ship_size, &client_message1);
                        printf("BOARD 1: \n");
                        output_board_squares(&board1);

                    } else { printf("2. There is other ship or border there.\n"); }
                } else { printf("1. Ship of size %d cannot be placed.\n", ship_size); }
            } 
            memset(&client_message1, 0, sizeof(client_message1));
        }

        // receive placement from s2
        n2 = recv(s2, client_message2, 256, MSG_DONTWAIT);
        if (n2 > 0) {
            
            ship_size = strlen(client_message2) / 2;
            struct ship ship;
            if (parse_ship_placement(&client_message2, &ship)){

                if (check_ship_count(ship_size, &board2)) {

                    mark_ship(&ship, ship_size, &client_message2);
                    printf("1. Ship of size %d can be placed\n", ship_size);
                    if (!is_border_ship(&board2, &ship)) {

                        printf("2. There is no other ship or border there\n");
                        board2.ships[board2.ships_count] = ship;
                        board2.ships_count++;
                        mark_ship_and_border(&board2, &ship, ship_size, &client_message2);
                        printf("BOARD 2: \n");
                        output_board_squares(&board2);
                        
                    } else { printf("2. There is other ship or border there\n"); }
                } else { printf("1. Ship of size %d cannot be placed.\n", ship_size); }
            }
            memset(&client_message2, 0, sizeof(client_message2));
        } 
    }


    // TODO: game part 
    printf("GAME PART\n");
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
    // struct board board;
    // board_init(&board, 100);
    // ships_init(&board);
    // output_board(&board, 100);

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
