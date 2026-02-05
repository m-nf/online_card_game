/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Main header file. Contains imported libraries, declares variables, imported headers files.
 * @version 1.0
 * @date 2022-01-30
 */

#ifndef main_h
#define main_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <string.h>

/*
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif
*/

/* general constants */
#define NUMBER_OF_CARDS_IN_HAND 4
#define NUMBER_OF_CARDS 32
#define NUMBER_OF_ROUNDS 16
#define BUFF_LENGHT 150
#define ARGUMENTS_COUNT 5
#define NICK_LENGHT 10

/* first chars of each message */
#define FIRST_MSG_CHAR 'v'
#define SECOND_MSG_CHAR 'b'
#define THIRD_MSG_CHAR '|'

/* waiting times, in seconds */
#define WAIT_FOR_OPPONENT 30 
#define WAIT_FOR_PING 5
#define PING_TIME 10
#define WAIT_FOR_SHORT_RECONNECT 30
#define WAIT_FOR_LONG_RECONNECT 300 // 5 minutes
#define WAIT_END_BTW_MSGS 5

/* number of splits of recieved messages */
#define NUM_SPLIT_GAME 3
#define NUM_SPLIT_NICK 3
#define NUM_SPLIT_ROUND 4
#define NUM_SPLIT_CONNECTION 3

/* values of cards */
#define SEVEN 7
#define EIGHT 8
#define NINE 9
#define TEN 10
#define UNTER 11
#define OBER 12
#define KING 13
#define ACE 14

/* struct for storing informations about game */
typedef struct thegame {
    int cards1[NUMBER_OF_CARDS_IN_HAND], cards2[NUMBER_OF_CARDS_IN_HAND]; // cards in hand
    int shuffled_stock[NUMBER_OF_CARDS];
    /* card_stock - index = karta, hodnota = hodnota karty-nepouzito, -1-pouzito */
    int played_card1, played_card2, points1, points2;
    int socket1, socket2;    
    int both_players; /* 1 true, 0 false */
    int stock_card_counter, round_counter;
    int played_cards_round_counter; /* 0,1*/
    char nick1[NICK_LENGHT], nick2[NICK_LENGHT];
    char turn[NICK_LENGHT];
} game;

/* struct for storing informations about player */
typedef struct theplayer {    
    int waiting_for_reconnect; /* 1 true, 0 false */
    int connected; /* 1 true, 0 false */
    int socket;
    int number_of_win_games;
    int invalid_msg_counter; // if there is more than three invalid messages, player is disconnected
    char nick[NICK_LENGHT];
} player;

/* struct for passing arguments to thread serve_communicate_thread */
typedef struct the_thread_parameters {
    int client_socket;
    char msg[BUFF_LENGHT];
} thread_parameters;

/* global variables */
extern game **game_pointers; // array for storing games
extern player **players;     // array for storing players
extern int number_of_players, number_of_connected_players;
extern int number_of_games, number_of_running_games;
extern int cards_values[NUMBER_OF_CARDS];
extern int *sockets_to_close;

extern fd_set clients_sockets;

/* threads stuff */
extern pthread_mutex_t game_lock; // locks void handle_game_msgs(int client_socket)
extern pthread_mutex_t delete_player_lock;
extern pthread_mutex_t round_lock; // locks void handle_round_msgs(int client_socket, char **splitted_msg)
extern pthread_mutex_t send_lock;  // void send_msg(char *msg, int client_socket)
extern pthread_mutex_t recv_lock;

#include "connection_stuff.h"
#include "communication.h"
#include "player_functions.h"
#include "game_functions.h"
#include "recv_msg_handlers.h"

#endif
