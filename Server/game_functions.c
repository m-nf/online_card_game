/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Contains game related functions. Like delete game, create game...
 * @version 1.0
 * @date 2022-01-30
 */

#include "definitions.h"

/**
 * @brief Forcily ends game (when opponent left game). Winner is the player
 * whose stayed in a game.
 * Player in parameter is the one, whose left.
 * 
 * @param pl 
 * @param game_index 
 */
void forcily_end_game(player *pl, int game_index) {
    char output[BUFF_LENGHT];
    if (game_pointers[game_index] != NULL) {
	    if (game_pointers[game_index] != NULL && game_pointers[game_index]->both_players == 1) { // if it is not waiting game
		if (strcmp(game_pointers[game_index]->nick1, pl->nick) == 0) { // player 1 left
		    sprintf(output, "vb|game|winner|forced|%s\n", game_pointers[game_index]->nick2);
		    send_msg(output, game_pointers[game_index]->socket2);
		    
		    sleep(WAIT_END_BTW_MSGS);
		    players[get_player_index(game_pointers[game_index]->socket2)]->number_of_win_games++;
		    sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket2)]->number_of_win_games);  
		    send_msg(output, game_pointers[game_index]->socket2);        
		}
		else { // player 2 left
		    sprintf(output, "vb|game|winner|forced|%s\n", game_pointers[game_index]->nick1);
		    send_msg(output, game_pointers[game_index]->socket1);
		    
		    sleep(WAIT_END_BTW_MSGS);
		    players[get_player_index(game_pointers[game_index]->socket1)]->number_of_win_games++;
		    sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket1)]->number_of_win_games);  
		    send_msg(output, game_pointers[game_index]->socket1); 
		}
	    }
     }
}

/**
 * @brief Prepares game and sends to both players their cards and opponent's nick.
 * 
 * @param game_index 
 */
void prepare_game(int game_index) {
    char output[BUFF_LENGHT];
    int rnd = rand() % 2;

    game_pointers[game_index]->round_counter = 0;
    game_pointers[game_index]->stock_card_counter = 0;
    game_pointers[game_index]->points1 = 0;
    game_pointers[game_index]->points2 = 0;
    if (rnd == 0) strcpy(game_pointers[game_index]->turn, game_pointers[game_index]->nick1);
    else strcpy(game_pointers[game_index]->turn, game_pointers[game_index]->nick2);
    shuffle_card_stock(game_index);
    
    int i;
    for (i = 0; i < NUMBER_OF_CARDS_IN_HAND; i++) {
        game_pointers[game_index]->cards1[i] = game_pointers[game_index]->shuffled_stock[game_pointers[game_index]->stock_card_counter];
        game_pointers[game_index]->stock_card_counter++;
        game_pointers[game_index]->cards2[i] = game_pointers[game_index]->shuffled_stock[game_pointers[game_index]->stock_card_counter];
        game_pointers[game_index]->stock_card_counter++;
    }

    sprintf(output, "vb|game|prepare|%s|%d|%d|%d|%d\n", game_pointers[game_index]->nick2, 
                                                        game_pointers[game_index]->cards1[0],
                                                        game_pointers[game_index]->cards1[1],
                                                        game_pointers[game_index]->cards1[2],
                                                        game_pointers[game_index]->cards1[3]);
    send_msg(output, game_pointers[game_index]->socket1);

    sprintf(output, "vb|game|prepare|%s|%d|%d|%d|%d\n", game_pointers[game_index]->nick1, 
                                                        game_pointers[game_index]->cards2[0],
                                                        game_pointers[game_index]->cards2[1],
                                                        game_pointers[game_index]->cards2[2],
                                                        game_pointers[game_index]->cards2[3]);
    send_msg(output, game_pointers[game_index]->socket2);
}

/**
 * @brief Creates new game. Allocates memory for game. Pointer is in 
 *        array game_pointers.
 *        Sets both_players to 0 (false).
 *        Sets socket1 and nick1 according to player, which created the game.
 * 
 * @param client_socket 
 * @return int Index of created game.
 */
int create_new_game(int client_socket) {
    int i;    
    for (i = 0; i < number_of_games; i++) {
        if (game_pointers[i] == NULL) {
            game_pointers[i] = (game *) malloc(sizeof(game));
            
            game_pointers[i]->both_players = 0;                        
            game_pointers[i]->socket1 = client_socket;
            game_pointers[i]->socket2 = -1;             
            strcpy(game_pointers[i]->nick1, get_player(client_socket)->nick);
            
            return i;
        }
    }    
    return -1;
}

/**
 * @brief Return index of game, if there is game which is waiting for opponent (second player).
 *        Return -1 if there isn't such game. 
 * @return int 
 */
int get_waiting_game() {
    int i;       
    for (i = 0; i < number_of_games; i++) {
            if (game_pointers[i] != NULL && game_pointers[i]->both_players == 0) {               
                return i;
            }
    }    
    return -1;
}

/**
 * @brief Frees memory of the game.
 *        Set pointer of deleted game to NULL.
 *        Decrease number of running games.
 * 
 * @param client_socket 
 */
void delete_game(int client_socket) {
    int game_index = get_game_index(client_socket);
    if (game_index != -1 && game_pointers[game_index] != NULL) {
	    free(game_pointers[game_index]);
	    game_pointers[game_index] = NULL;
	    number_of_running_games--;
	}
}

/**
 * @brief Return index of game, if there is such a game.
 *        Return -1, if there isn't.
 *        Searching is by sockets.        
 * 
 * @param client_socket 
 * @return int 
 */
int get_game_index(int client_socket) {
    int i;       
    for (i = 0; i < number_of_games; i++) {
        if (game_pointers[i] != NULL && (game_pointers[i]->socket1 == client_socket || game_pointers[i]->socket2 == client_socket)) {               
            return i;
        }
    }    
    return -1;
}

/**
 * Return game index, searching by nick.
 */
int get_game_index_ni(char nick[]) {
    int i;       
    for (i = 0; i < number_of_games; i++) {
        if (game_pointers[i] != NULL && (strcmp(game_pointers[i]->nick1, nick) == 0 || strcmp(game_pointers[i]->nick2, nick) == 0)) {               
            return i;
        }
    }    
    return -1;
}

/**
 * @brief Shuffles stock of cards for a game identified by index.
 * 
 * @param index 
 */
void shuffle_card_stock(int index) {
    int counter = 0, rnd;

    srand(time(NULL));
    memset(game_pointers[index]->shuffled_stock, -1, sizeof(int) * NUMBER_OF_CARDS);

    while (counter < 32) {
        rnd = rand() % 32;
        if (game_pointers[index]->shuffled_stock[rnd] == -1) {
            game_pointers[index]->shuffled_stock[rnd] = counter;
            counter++;
        }        
    }
}
