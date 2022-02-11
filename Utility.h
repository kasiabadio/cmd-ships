#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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

#define NO_SHIP 0
#define SHIP 1
#define BORDER 2

#define PATROL 1
#define DESTROYER 2 
#define SUBMARINE 3
#define BATTLESHIP 4

char client_message1[256];
char client_message2[256];
char buffer[256];


const char *board_alphabet[] = {
    "A0", "B0", "C0", "D0", "E0", "F0", "G0", "H0", "I0", "J0",
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1", "I1", "J1",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2", "I2", "J2",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3", "I3", "J3",
    "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4", "I4", "J4",
    "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5", "I5", "J5",
    "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6", "I6", "J6",
    "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7", "I7", "J7",
    "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "I8", "J8",
    "A9", "B9", "C9", "D9", "E9", "F9", "G9", "H9", "I9", "J9" };


struct square{
    char name[256];
    unsigned int ship; // 0 - no ship, 1 - ship, 2 - border of a ship
    bool clicked; // 0 - not clicked, 1 - clicked
    
};


struct ship{
    int name;
    struct square squares[4];
    bool is_sunk;
    char nhv; // 'H' - horizontal, 'V' - vertical

};


struct board{
    struct square board[100];
    struct ship ships[10];
    unsigned int ships_count;
    bool all_sunk;
    
};


struct thread_arg{
    int s1;
    int s2;
};

void output_board_squares(const struct board *board){

    for (int i = 0; i < 100; i++){
        if (i % 10 == 0) printf("\n");
        printf("%d ", board->board[i].ship);
    }
    printf("\n\n");
}


void output_board_client(const struct board *board){
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < 100; i++){
        if (i % 10 == 0) printf("\n");

        if (i == 0) printf("0  ");
        else if (i == 10) printf("1  ");
        else if (i == 20) printf("2  ");
        else if (i == 30) printf("3  ");
        else if (i == 40) printf("4  ");
        else if (i == 50) printf("5  ");
        else if (i == 60) printf("6  ");
        else if (i == 70) printf("7  ");
        else if (i == 80) printf("8  ");
        else if (i == 90) printf("9  ");

        if (board->board[i].ship == NO_SHIP) printf("- ");
        if (board->board[i].ship == SHIP) printf("X ");
        if (board->board[i].ship == BORDER) printf("~ ");
    }
    printf("\n\n");
}


bool in_alphabet(const char *move){

    for (int i = 0; i < 100; i++){
        if (strcmp(move, board_alphabet[i]) == 0) return true;
    }
    return false;
}


bool is_next_letter(char prev_letter, char curr_letter){

    //printf("Is next letter: %c %c\n", prev_letter, curr_letter);
    if (prev_letter + 1 != curr_letter) return false;
    return true;
}


bool is_next_number(char prev_number, char curr_number){
    
    
    int prev_number1 = (int)prev_number - 48;
    int curr_number1 = (int)curr_number - 48;
    //printf("Is next number: %d %d\n", prev_number1, curr_number1);
    if (prev_number1 + 1 != curr_number1) return false;
    return true;
}


int find_board_position(char *square){

    for (int i = 0; i < 100; i++){
        if (strcmp(board_alphabet[i], square) == 0) return i;
    }
    return -1;
}


void board_init(struct board *board, unsigned int size_board){

    board->all_sunk = false;
    board->ships_count = 0;

    for (unsigned int i = 0; i < size_board; i++){

        strcpy(board->board[i].name, board_alphabet[i]);
        //printf("board->board[i].name: %s\n", board->board[i].name);
        board->board[i].ship = NO_SHIP;
        board->board[i].clicked = false;

    }
}


#endif