#include "Utility.h"

void *socketThread(void *arg){

    pid_t x = syscall(__NR_gettid);
    printf("Thread id: %d\n", x);
  
    struct thread_arg *targs = arg;
    int s1 = targs->s1;
    int s2 = targs->s2;
    int n1, n2, ship_size;
    bool round1 = true;
    bool round2 = false;

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
                    //printf("Ship of size %d can be placed.\n", ship_size);

                    if (!is_border_ship(&board1, &ship)){

                        printf("There is no other ship or border there.\n");
                        board1.ships[board1.ships_count] = ship;
                        board1.ships_count++;
                        mark_ship_and_border(&board1, &ship, ship_size, &client_message1);
                        
                        char temp1[2];

                        // placement is ok, send message to me
                        if (ship.nhv == 'H'){
                            strcpy(temp1, "H");
                            strcat(temp1, client_message1);
                            if ((send(s1, temp1, strlen(temp1), 0)) == -1) printf("Send failed 1.1\n");

                        } else if (ship.nhv == 'V'){
                            strcpy(temp1, "V");
                            strcat(temp1, client_message1);
                            if ((send(s1, temp1, strlen(temp1), 0)) == -1) printf("Send failed 1.1\n");
                        }
                        
                    } else { 
                        send(s1, "There is other ship or border there.", 36, 0);
                        //printf("There is other ship or border there.\n\n"); 
                    }
                } else { 
                    send(s1, "Ship of this size cannot be placed.", 35, 0);
                    //printf("Ship of size %d cannot be placed.\n\n", ship_size); 
                }
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
                    //printf("Ship of size %d can be placed\n", ship_size);

                    if (!is_border_ship(&board2, &ship)) {

                        //printf("There is no other ship or border there\n");
                        board2.ships[board2.ships_count] = ship;
                        board2.ships_count++;
                        mark_ship_and_border(&board2, &ship, ship_size, &client_message2);

                        char temp2[2];

                        // placement is ok, send message to me
                        if (ship.nhv == 'H'){
                            strcpy(temp2, "H");
                            strcat(temp2, client_message2);
                            if ((send(s2, temp2, strlen(temp2), 0)) == -1) printf("Send failed 2.1\n");
                            
                        } else if (ship.nhv == 'V'){
                            strcpy(temp2, "V");
                            strcat(temp2, client_message2);
                            if ((send(s2, temp2, strlen(temp2), 0)) == -1) printf("Send failed 2.1\n");
                        }
                        
                    } else { 
                        if ((send(s2, "There is other ship or border there.", 37, 0)) == -1) printf("Send failed 3\n");
                        //printf("There is other ship or border there.\n\n"); 
                    }

                } else { 
                    if ((send(s2, "Ship of this size cannot be placed.", 36, 0)) == -1) printf("Send failed 4\n");
                    //printf("Ship of size %d cannot be placed.\n", ship_size); 
                }
            }
            memset(&client_message2, 0, sizeof(client_message2));
        } 
    }

    printf("Game part\n");
    if ((send(s1, "Game has started.", 18, 0)) == -1) printf("Send failed 5\n");
    if ((send(s2, "Game has started.", 18, 0)) == -1) printf("Send failed 6\n");

    while(1){

        if (round1) { // receive message only from s1

            if ((send(s1, "Type move", 10, 0)) == -1) printf("Send failed 7\n");

            n1 = recv(s1, client_message1, 256, 0);
            if (n1 > 0) {
                printf("Received move: %s\n", client_message1);
                validate(client_message1, s1, s2, &board2);
                memset(&client_message1, 0, sizeof(client_message1));
            }
            round1 = false;
            round2 = true; // then receive message only from s2
        }

        if (round2) { // receive message only from s2

            if ((send(s2, "Type move", 10, 0)) == -1) printf("Send failed 8\n");

            n2 = recv(s2, client_message2, 256, 0);
            if (n2 > 0) {
                printf("Received move: %s\n", client_message2);
                validate(client_message2, s2, s1, &board1);
                memset(&client_message2, 0, sizeof(client_message2));
            } 
            round1 = true; // then receive message only from s1
            round2 = false;
        }
       
        if (n1 == 0 || n2 == 0){
            break;
        }
    }

    free(targs);
    pthread_exit(NULL);
}
