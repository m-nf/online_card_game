/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief File with main function. Checks arguments, inicialize variables.
 * @version 1.0
 * @date 2022-01-30
 */

#include "definitions.h"

/* global variables */
game **game_pointers; // array for storing games
player **players;     // array for storing players
int number_of_players, number_of_connected_players;
int number_of_games, number_of_running_games;
int cards_values[NUMBER_OF_CARDS];
int *sockets_to_close;

fd_set clients_sockets;

/* threads stuff */
pthread_mutex_t game_lock; // locks void handle_game_msgs(int client_socket)
pthread_mutex_t delete_player_lock; 
pthread_mutex_t round_lock; // locks void handle_round_msgs(int client_socket, char **splitted_msg)
pthread_mutex_t send_lock; // void send_msg(char *msg, int client_socket)
pthread_mutex_t recv_lock; 

/**
 * Pepares cards values. There are in the same order in client. 
 */
void prepare_card_values() {
    cards_values[0] = TEN;
    cards_values[1] = NINE;
    cards_values[2] = ACE;
    cards_values[3] = KING;
    cards_values[4] = EIGHT;
    cards_values[5] = SEVEN;
    cards_values[6] = UNTER;
    cards_values[7] = OBER;
    cards_values[8] = TEN;
    cards_values[9] = NINE;
    cards_values[10] = ACE;
    cards_values[11] = KING;
    cards_values[12] = EIGHT;
    cards_values[13] = SEVEN;
    cards_values[14] = UNTER;
    cards_values[15] = OBER;
    cards_values[16] = TEN;
    cards_values[17] = NINE;
    cards_values[18] = ACE;
    cards_values[19] = KING;
    cards_values[20] = EIGHT;
    cards_values[21] = SEVEN;
    cards_values[22] = UNTER;
    cards_values[23] = OBER;
    cards_values[24] = TEN;
    cards_values[25] = NINE;
    cards_values[26] = ACE;
    cards_values[27] = KING;
    cards_values[28] = EIGHT;
    cards_values[29] = SEVEN;
    cards_values[30] = UNTER;
    cards_values[31] = OBER;
}

/**
 * Checks if arguments are alright.
 * Checks their number, format. 
 * Return 0 - ok, 1 - error.
 */
int handle_arguments(int argc, char *argv[], int *port) {
    if (argc != ARGUMENTS_COUNT) {
        printf("Wrong number of arguments.");
        return 1;
    }
    else {        
        if ((sscanf(argv[1], "%d", &number_of_players) != 1) || (number_of_players <= 0)) {
            printf("Wrong format of arguments.");
            return 1;
        }
        if ((sscanf(argv[2], "%d", &number_of_games) != 1) || (number_of_games <= 0)) {
            printf("Wrong format of arguments.");
            return 1;
        }
        if ((sscanf(argv[3], "%d", port) != 1) || (*port > 65535)) {
            printf("Wrong format of arguments.");
            return 1;
        }
        if (strlen(argv[4]) > 15) {
            printf("Wrong format of arguments.");
            return 1;
        }
    }
    return 0;         
}

/**
 * @brief Prepare arrays **players and **game_pointers.
 *        Allocate memory and set each pointer of array to NULL.
 * 
 */
void prepare_arrays() {    
    players = (player **) calloc(number_of_players, sizeof(player *));
    game_pointers = (game **) calloc(number_of_games, sizeof(game *));
    sockets_to_close = (int *) malloc(number_of_players * sizeof(int));

    int i;
    for (i = 0; i < number_of_players; i++) {
        players[i] = NULL;
    }
    for (i = 0; i < number_of_games; i++) {
        game_pointers[i] = NULL;
    }
    for (i = 0; i < number_of_players; i++) {
        sockets_to_close[i] = -1;
    }
}

/**
 * Inititalize mutexes.
 * Return 0, if it is ok. 1, when errror occures.
 */
int prepare_mutexes() {
    if (pthread_mutex_init(&game_lock, NULL) != 0) {
        printf("Mutex init has failed\n");
        return 1;
    }
    if (pthread_mutex_init(&delete_player_lock, NULL) != 0) {
        printf("Mutex init has failed\n");
        return 1;
    }
    if (pthread_mutex_init(&round_lock, NULL) != 0) {
        printf("Mutex init has failed\n");
        return 1;
    }
    if (pthread_mutex_init(&send_lock, NULL) != 0) {
        printf("Mutex init has failed\n");
        return 1;
    }
    if (pthread_mutex_init(&recv_lock, NULL) != 0) {
        printf("Mutex init has failed\n");
        return 1;
    }
    return 0;
}

/**
 * CLeans up after programme ends.
 * Frees memory, destroys mutexes.
 */
void clean_up() {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (players[i] != NULL) {
            free(players[i]);
        }
    }
    for (i = 0; i < number_of_games; i++) {
        if (game_pointers[i] != NULL) {
            free(game_pointers[i]);
        }
    }
    
    free(players);
    free(game_pointers);
    free(sockets_to_close); 

    pthread_mutex_destroy(&game_lock);
    pthread_mutex_destroy(&delete_player_lock);
    pthread_mutex_destroy(&round_lock);
    pthread_mutex_destroy(&send_lock);
}

/**
 * Main accesing point of programme.
 */
int main(int argc, char *argv[]) {    
    int port;
    pthread_t thread_id;

    if (handle_arguments(argc, argv, &port) == 1) {        
        return EXIT_FAILURE;
    }
    else {
        if (prepare_mutexes() == 1) {
            return EXIT_FAILURE;
        }   
        else {
            prepare_arrays();
            prepare_card_values();
            pthread_create(&thread_id, NULL, ping_players, NULL);
            communicate(port, argv[4]);
            clean_up();
        }        
    }
    
  	return EXIT_SUCCESS;
}