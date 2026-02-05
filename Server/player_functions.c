/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Contains player realted functions. Like create player, delete player...
 * @version 1.0
 * @date 2022-01-30
 */

#include "definitions.h"

/**
 * @brief Disconnect and delete player.
 * 
 * @param client_socket 
 */
void delete_player(int client_socket) {    
    pthread_mutex_lock(&delete_player_lock);
    
    int player_index = get_player_index(client_socket);
    if (player_index != -1) {
        int game_index = get_game_index(client_socket);
        player *pl = get_player(client_socket);        
        
        if (game_index != -1) { // if player is in game
            forcily_end_game(pl, game_index);
            delete_game(client_socket);
        }

        add_socket_to_close(client_socket);
        
        number_of_connected_players--;
        free(players[player_index]);
        players[player_index] = NULL;

        printf("Player deleted.\n");                
    }

    pthread_mutex_unlock(&delete_player_lock);    
}

/**
 * Creates new player. Allocates mememory, sets the variables.
 */
void create_player(int client_socket) {
    int i;
    
    for (i = 0; i < number_of_players; i++) {
        if (players[i] == NULL) {    
            players[i] = malloc(sizeof(player));
            players[i]->socket = client_socket;            
            players[i]->invalid_msg_counter = 0;
            players[i]->connected = 1;
            players[i]->waiting_for_reconnect = 0;
            players[i]->number_of_win_games = 0;
            bzero(players[i]->nick, sizeof(char) * NICK_LENGHT);

            number_of_connected_players++;
            return;
        }
    }
}

/**
 * @brief Get the player object by client_socket.
 * 
 * @param client_socket 
 * @return player* 
 */
player* get_player(int client_socket) {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (players[i] != NULL) {       
            if (players[i]->socket == client_socket) {
                return players[i];
            }
        }        
    }
    return NULL;
}

/**
 * Return players index. Searching is by nick.
 */
int get_player_index_ni(char *nick) {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (players[i] != NULL) {       
            if (strcmp(nick, players[i]->nick) == 0) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * Return players index. Searching is by socket.
 */
int get_player_index(int client_socket) {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (players[i] != NULL) {       
            if (players[i]->socket == client_socket) {
                return i;
            }
        }        
    }
    return -1;
}

/**
 * Gives nick to the player identified by socket.
 */
void put_nickname(char *nick, int client_socket) {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (players[i] != NULL && players[i]->socket == client_socket) {                 
            strcpy(players[i]->nick, nick);         
            break;
        }        
    }    
}
