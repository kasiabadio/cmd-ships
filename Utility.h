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

#define A1 0
#define B1 1
#define C1 2
#define D1 3
#define E1 4
#define F1 5
#define G1 6
#define H1 7
#define I1 8
#define J1 9

#define A2 10
#define B2 11
#define C2 12
#define D2 13
#define E2 14
#define F2 15
#define G2 16
#define H2 17
#define I2 18
#define J2 19

#define A3 20
#define B3 21
#define C3 22
#define D3 23
#define E3 24
#define F3 25
#define G3 26
#define H3 27
#define I3 28
#define J3 29

#define A4 30
#define B4 31
#define C4 32
#define D4 33
#define E4 34
#define F4 35
#define G4 36
#define H4 37
#define I4 38
#define J4 39

#define A5 40
#define B5 41
#define C5 42
#define D5 43
#define E5 44
#define F5 45
#define G5 46
#define H5 47
#define I5 48
#define J5 49

#define A6 50
#define B6 51
#define C6 52
#define D6 53
#define E6 54
#define F6 55
#define G6 56
#define H6 57
#define I6 58
#define J6 59

#define A7 60
#define B7 61
#define C7 62
#define D7 63
#define E7 64
#define F7 65
#define G7 66
#define H7 67
#define I7 68
#define J7 69

#define A8 70
#define B8 71
#define C8 72
#define D8 73
#define E8 74
#define F8 75
#define G8 76
#define H8 77
#define I8 78
#define J8 79

#define A9 80
#define B9 81
#define C9 82
#define D9 83
#define E9 84
#define F9 85
#define G9 86
#define H9 87
#define I9 88
#define J9 89

#define A10 90
#define B10 91
#define C10 92
#define D10 93
#define E10 94
#define F10 95
#define G10 96
#define H10 97
#define I10 98
#define J10 99

const char *board_alphabet[] = {
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1", "I1", "J1",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2", "I2", "J2",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3", "I3", "J3",
    "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4", "I4", "J4",
    "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5", "I5", "J5",
    "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6", "I6", "J6",
    "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7", "I7", "J7",
    "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "I8", "J8",
    "A9", "B9", "C9", "D9", "E9", "F9", "G9", "H9", "I9", "J9",
    "A10", "B10", "C10", "D10", "E10", "F10", "G10", "H10", "I10", "J10"
    };


struct square{
    char name[256];
    unsigned int ship; // 0 - no ship, 1 - ship, 2 - border of a ship
    bool clicked; // 0 - not clicked, 1 - clicked
};


struct ship{
    char name[256];
    struct square squares[10];
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

#endif