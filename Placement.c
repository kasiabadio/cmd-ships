#include "Utility.h"

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

    if (my_board->ships_count == 10) return true;

    if (my_board->ships_count < 10) return false;

    // double check
    if (check_ship_count(PATROL, &my_board) || check_ship_count(DESTROYER, &my_board) ||
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

            for (int r = start_pos; r <= end_pos; r += 10){
                board->board[r].ship = SHIP;
            }
            
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
