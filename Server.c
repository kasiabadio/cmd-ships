#include "Utility.h"


void board_init(struct board *board, unsigned int size_board){

    board->all_sunk = false;
    board->ships_count = 0;

    for (unsigned int i = 0; i < size_board; i++){

        strcpy(board->board[i].name, board_alphabet[i]);
        board->board[i].ship = NO_SHIP;
        board->board[i].clicked = false;

    }
}

// ------------------------------------- GAME PART ----------------------------------


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

// -------------------------------- SHIP PLACEMENT -----------------------------------------------

// iterate through board and check if overlaps ship's squares
bool is_border_ship(struct board *board, struct ship *ship){

    for (unsigned int s = 0; s < ship->name; s++){

        //printf("s: %d\n", s);
        for (unsigned int i = 0; i < 100; i++){
            
            // printf("board->board[i].name: %s, ship->squares[s].name: %s\n", 
            //     board->board[i].name, ship->squares[s].name);

            if ((strcmp(board->board[i].name, ship->squares[s].name) == 0) && board->board[i].ship != NO_SHIP) return true;
        }
    }
    
    return false;
}


// return true if ship of such size can be placed
bool check_ship_count(int ship_size, struct board *board){

    int ship_count = 0;
    for (int i = 0; i < board->ships_count; i++){
        if (board->ships[i].name == ship_size) ship_count += 1;
    }

    if (ship_size == PATROL && ship_count == 4) return false;
    else if (ship_size == DESTROYER && ship_count == 3) return false;
    else if (ship_size == SUBMARINE && ship_count == 2) return false;
    else if (ship_size == BATTLESHIP && ship_count == 1) return false;

    return true;
}


// return true if any ship can be placed
bool is_placement_complete(struct board *my_board){
    
    if (my_board->ships_count < 10) return false;
    else if (check_ship_count(PATROL, &my_board) || check_ship_count(DESTROYER, &my_board) ||
    check_ship_count(SUBMARINE, &my_board) || check_ship_count(BATTLESHIP, &my_board)) return false;

    return true;
}

// parse message from client and return true if ship is correctly aligned 
bool parse_ship_placement(char *message, struct ship *ship){
    
    char subbuff[3];
    char letter_prev;
    char number_prev;

    ship->nhv = 'N';
    if (strlen(message) % 2 != 0 || strlen(message) > 8) return false;
  
    // message must contain pattern letter+number
    for (int ln = 0; ln < strlen(message); ln += 2){
        memcpy(subbuff, &message[ln], 2);
        subbuff[2] = '\0';
        if (!in_alphabet(subbuff)) 
        {
            return false;
            // printf("\n\nmove is not valid: %s\n\n", subbuff);

        } else {
            // printf("move is valid: %s\n", subbuff);
            // printf("letter_prev, number_prev: %c %c\n", letter_prev, number_prev);
            // check if ship is aligned in one line
            if (ln >= 4){

                // letter must be the same, number must be next to the previous one
                if (ship->nhv == 'V' && letter_prev != subbuff[0]){
                    return false;

                } else if (ship->nhv == 'V' && letter_prev == subbuff[0] && !is_next_number(number_prev, subbuff[1])){
                    return false;

                } else {
                    number_prev = subbuff[1];
                }

                // number must be the same, letter must be next to the previous one
                if (ship->nhv == 'H' && number_prev != subbuff[1]){
                    return false;

                } else if (ship->nhv == 'H' && !is_next_letter(letter_prev, subbuff[0])){
                    return false;

                } else {
                    letter_prev = subbuff[0];
                }

            } else if (ln == 2){

                // same letters - ship is placed vertically, check only number
                if (letter_prev == subbuff[0] && is_next_number(number_prev, subbuff[1])) { 
                    ship->nhv = 'V';
                    number_prev = subbuff[1];

                // same numbers - ship is placed horizontally, check only letter
                } else if (number_prev == subbuff[1] && is_next_letter(letter_prev, subbuff[0])){
                    ship->nhv = 'H';
                    letter_prev = subbuff[0];

                } else { 
                    return false; 
                }

            } else {
                letter_prev = subbuff[0];
                number_prev = subbuff[1];
            }
        }
    }
    
    return true;    
}


void mark_ship(struct ship *ship, int ship_size, char *message){
    char subbuff[3];

    // copy client message into ship
    ship->name = ship_size;
    ship->is_sunk = false;
    unsigned int s = 0;
    for (int ln = 0; ln < strlen(message); ln += 2){
        memcpy(subbuff, &message[ln], 2);
        subbuff[2] = '\0';
        strcpy(ship->squares[s].name, subbuff);
        
        s += 1;
    }

}


void mark_ship_and_border(struct board *board, struct ship *ship, int ship_size, char *message){

    char subbuff[3];
    char start[3], end[3];

    // copy client message into ship
    ship->name = ship_size;
    ship->is_sunk = false;
    unsigned int s = 0;
    for (int ln = 0; ln < strlen(message); ln += 2){
        memcpy(subbuff, &message[ln], 2);
        subbuff[2] = '\0';
        strcpy(ship->squares[s].name, subbuff);

        if (ln == 0) strcpy(start, subbuff);
        if (ln == strlen(message) - 2) strcpy(end, subbuff);
        
        s += 1;
    }

    // mark border on board - 2 and mark ship on board - 1
    int start_pos = find_board_position(&start);
    int end_pos = find_board_position(&end);

    if (ship->nhv == 'N') ship->nhv = 'V';
    if (ship->nhv == 'V') { // ship is vertical
        printf("V\n");
        int start_first_row = (start_pos - 1) - 10;
        int start_last_row = (end_pos - 1) + 10;
        int modulo = start_pos % 10; // returns 0 if ship is in col 0, if it is at column 9, then 9

        for (int fr = start_first_row; fr < start_first_row + 3; fr++){
            if ((modulo > 0 && fr == start_first_row) || 
            (fr == start_first_row+1) || 
            (modulo < 9 && fr == start_first_row+2)){
                board->board[fr].ship = BORDER;
            }
        }

        if (modulo > 0){
            for (int r = start_pos - 1; r <= end_pos - 1; r += 10){
                board->board[r].ship = BORDER;
                board->board[r+1].ship = SHIP;
            }
        } else { 
            board->board[start_pos].ship = SHIP;
        }

        if (modulo < 9){
            for (int r = start_pos + 1; r <= end_pos + 1; r += 10){
                board->board[r].ship = BORDER;
            }
        }

        for (int lr = start_last_row; lr < start_last_row + 3; lr++){
            if ((modulo > 0 && lr == start_last_row) || 
            (lr == start_last_row+1) || 
            (modulo < 9 && lr == start_last_row+2)){
                board->board[lr].ship = BORDER;
            }
        }

    } else if (ship->nhv == 'H'){ // ship is horizontal

        printf("H\n");
        int start_first_row = (start_pos - 1) - 10;
        int end_first_row = (end_pos + 1) - 10; 

        int start_third_row = (start_pos - 1) + 10;
        int end_third_row = (end_pos + 1) + 10;

        int modulo_start = start_pos % 10; // returns 0 if ship starts at 0
        int modulo_end = end_pos % 10; // returns 9 if ship ends at 9


        for (int fr = start_first_row; fr <= end_first_row; fr++){
            if ((modulo_start > 0 && fr == start_first_row) || 
            (fr > start_first_row && fr < end_first_row) || 
            (modulo_end < 9 && fr == end_first_row)){
                board->board[fr].ship = BORDER;
            }
        }

        for (int sr = start_pos - 1; sr <= end_pos + 1; sr++){
            if (modulo_start > 0 && sr == start_pos - 1) board->board[sr].ship = BORDER;
            else if (sr > start_pos - 1 && sr < end_pos + 1) board->board[sr].ship = SHIP;
            else if (modulo_end < 9 && sr == end_pos + 1) board->board[sr].ship = BORDER;
        }

        for (int tr = start_third_row; tr <= end_third_row; tr++){
            if ((modulo_start > 0 && tr == start_third_row) || 
            (tr > start_third_row && tr < end_third_row) || 
            (modulo_end < 9 && tr == end_third_row)){
                board->board[tr].ship = BORDER;
            }
        }
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

    // TODO: placement of ships by both opponents
    while (is_placement_complete(&board1) == false && is_placement_complete(&board2) == false){

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

                // check if this ship can be placed on board
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
