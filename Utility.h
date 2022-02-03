#ifndef UTILITY_H_
#define UTILITY_H_


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

#define NO_SHIP 0
#define SHIP 1
#define BORDER 2

#define PATROL 1
#define DESTROYER 2 
#define SUBMARINE 3
#define BATTLESHIP 4

#define A0 0
#define B0 1
#define C0 2
#define D0 3
#define E0 4
#define F0 5
#define G0 6
#define H0 7
#define I0 8
#define J0 9

#define A1 10
#define B1 11
#define C1 12
#define D1 13
#define E1 14
#define F1 15
#define G1 16
#define H1 17
#define I1 18
#define J1 19

#define A2 20
#define B2 21
#define C2 22
#define D2 23
#define E2 24
#define F2 25
#define G2 26
#define H2 27
#define I2 28
#define J2 29

#define A3 30
#define B3 31
#define C3 32
#define D3 33
#define E3 34
#define F3 35
#define G3 36
#define H3 37
#define I3 38
#define J3 39

#define A4 40
#define B4 41
#define C4 42
#define D4 43
#define E4 44
#define F4 45
#define G4 46
#define H4 47
#define I4 48
#define J4 49

#define A5 50
#define B5 51
#define C5 52
#define D5 53
#define E5 54
#define F5 55
#define G5 56
#define H5 57
#define I5 58
#define J5 59

#define A6 60
#define B6 61
#define C6 62
#define D6 63
#define E6 64
#define F6 65
#define G6 66
#define H6 67
#define I6 68
#define J6 69

#define A7 70
#define B7 71
#define C7 72
#define D7 73
#define E7 74
#define F7 75
#define G7 76
#define H7 77
#define I7 78
#define J7 79

#define A8 80
#define B8 81
#define C8 82
#define D8 83
#define E8 84
#define F8 85
#define G8 86
#define H8 87
#define I8 88
#define J8 89

#define A9 90
#define B9 91
#define C9 92
#define D9 93
#define E9 94
#define F9 95
#define G9 96
#define H9 97
#define I9 98
#define J9 99


char client_message1[256];
char client_message2[256];
char buffer[256];


const char *board_alphabet[] = {
    "A0", "B0", "C0", "D0", "E0", "F0", "G0", "H0", "I0", "J0",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2", "I2", "J2",
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


#endif