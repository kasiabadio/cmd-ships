#include "Utility.h"

bool are_all_sunk(struct board *opponent_board){
    int sunk_count = 0;
    for (int s = 0; s < opponent_board->ships_count; s++){
        if (opponent_board->ships[s].is_sunk) sunk_count++;
    }
    if (sunk_count == 10) return true;
    return false;
}


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
        printf("clicked_counter: %d, opponent_board->ships[s].name: %d, ship_found: %d\n", clicked_counter, opponent_board->ships[s].name, ship_found);
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
                        //printf("Saved square: %s\n", opponent_board->ships[s].squares[i].name);
                        opponent_board->ships[s].squares[i].clicked = true;
                    }
                }
            }
            return true;
        }
    }
    return false;
}


bool validate(char *client_message, int my_s, int other_s, struct board *opponent_board){
    
    bool validated = false;
    for (int i = 0; i < 100; i++){
        if (strcmp(board_alphabet[i], client_message) == 0){
            validated = true;
            break;
        }
    }

    if (validated){
        
        // check if there is ship and send return message to me: Ship / No ship / Ship sunk
        bool is_ship = check_square(opponent_board, client_message);
        if (is_ship){
            char client_message_copy[256];
            strcpy(client_message_copy, client_message);
            strcat(client_message, "->Ship");
            printf("Ship\n");

            if ((send(my_s, client_message, sizeof(client_message), 0)) == -1) printf("Send failed\n");
            sleep(1);
            if ((send(other_s, "Opponent hit!", 14, 0)) == -1) printf("Send failed\n");

            bool is_sunk = is_ship_sunk(opponent_board, &client_message_copy);
            if (is_sunk){
                printf("Ship sunk!\n");

                sleep(1);
                if ((send(my_s, "Ship sunk!", 11, 0)) == -1) printf("Send failed\n");
                sleep(1);
                if ((send(other_s, "Opponent sunk your ship!", 25, 0)) == -1) printf("Send failed\n");
            } 

        } else { 
            
            strcat(client_message, "->None");
            printf("None\n");

            sleep(1);
            if ((send(my_s, client_message, sizeof(client_message), 0)) == -1) printf("Send failed\n");
            sleep(1);
            if ((send(other_s, "Opponent missed!", 17, 0)) == -1) printf("Send failed\n");   
        }

    } else { // move is invalid, send error to sender
        if ((send(my_s, "Type again.", 12, 0)) == -1) printf("Send failed\n");
        return false;
    } 
    return true;
}
