/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Contains functions for ping and reconnect.
 * @version 1.0
 * @date 2022-01-30
 */

#include "definitions.h"

/**
 * If is player in a game, sends a msg to opponent, that opp is disconnected.
 * If player is in game, wait 30 s to reconnect, then forcily end game.
 * Wait 5 min for long reconnect.
 * Then delets player. 
 */ 
void* handle_disconnected_player_thread(void* arg) {    
    int client_socket = *(int *) arg;       
    player *pl = get_player(client_socket);   
    
    if (pl->waiting_for_reconnect == 0) {
        
        pl->waiting_for_reconnect = 1;
        pl->connected = 0;       

        int time_counter;        
        int game_index = get_game_index(client_socket);      
        
        if(game_index != -1) { // message for opponent, that opponent is temporarily disconnected
            if (strcmp(game_pointers[game_index]->nick1, pl->nick) == 0) {
                send_msg("vb|connection|oppdisconnected\n", game_pointers[game_index]->socket2);
            }
            else {
                send_msg("vb|connection|oppdisconnected\n", game_pointers[game_index]->socket1);
            }        

            //short disconenction
            while (pl->connected == 0 && time_counter < (WAIT_FOR_SHORT_RECONNECT / 2)) {
                sleep(2);
                time_counter++;
            }        
        
            if (pl->connected == 0) { // message for opponent, that game is ending
                forcily_end_game(pl, game_index);
                delete_game(client_socket);
            }
        }
        
        // long disconnection
        time_counter = 0;
        while (pl->connected == 0 && time_counter < (WAIT_FOR_LONG_RECONNECT / 5)) {   
            sleep(5);
            time_counter++;        
        }

        if (pl->connected == 0) {
            delete_player(client_socket);
        }
    }    
    
    free(arg);
    return 0;
}

/**
 * Pings all players and set flags to 0 for each player. Then waits 5 s
 * and checks if flags are 1 or 0. If 0 -> reconnect.
 * If player reconnects, no ping msgs are sent to him.
 */
void* ping_players() {
    int i;
    pthread_t thread_id;
    for(;;) {
        /* send ping request */
        for (i = 0; i < number_of_players; i++) {
            if (players[i] != NULL && players[i]->waiting_for_reconnect == 0) {
                players[i]->connected = 0;
                send_msg("vb|connection|ping\n", players[i]->socket);
            }
        }
        sleep(WAIT_FOR_PING);
        /* checking answers */
        for (i = 0; i < number_of_players; i++) {                  
            if (players[i] != NULL && players[i]->connected == 0 && players[i]->waiting_for_reconnect == 0) {               
                int *cl_sc = (int *) malloc(sizeof(int));
                *cl_sc = players[i]->socket;
                pthread_create(&thread_id, NULL, handle_disconnected_player_thread, (void *)cl_sc);              
            }
        }
        sleep(PING_TIME);       
    }
    return 0;
}

/**
 * Reconnects client back in lobby or game by sending him all informations, what he needs.
 * Ends thread handle_disconnected_player_thread for this player.
 */
void reconnect_player(int client_socket, char *nick) {    
    char output[BUFF_LENGHT];
    player *pl = players[get_player_index_ni(nick)];
    int game_index = get_game_index_ni(nick);

    if (pl->waiting_for_reconnect == 1) {

        pl->connected = 1;
        pl->waiting_for_reconnect = 0;
        pl->socket = client_socket;
        pl->invalid_msg_counter = 0;        

        // deletes incidental duplicate of this player
        int i, counter = 0;
        for (i = 0; i < number_of_players; i++) {
            if (players[i] != NULL && players[i]->socket == client_socket) {
                if (counter == 1) {
                    number_of_connected_players--;
                    free(players[i]);
                    players[i] = NULL;
                    break;
                }
                counter++;
            }
        }

        if (game_index != -1) { // reconnect to running game         
            
            if (strcmp(game_pointers[game_index]->nick1, nick) == 0) { // player 1 was disconnected
                game_pointers[game_index]->socket1 = client_socket;
                // my turn/opp turn, cards counter 0
                if (game_pointers[game_index]->played_cards_round_counter == 0) { // player's 1/2 turn and no card was played
                    
                    sprintf(output,"vb|connection|reconnect|%d|%d|%d|%d|%s|%d|%d|%d|%s\n", game_pointers[game_index]->cards1[0],
                                                                                    game_pointers[game_index]->cards1[1],
                                                                                    game_pointers[game_index]->cards1[2],
                                                                                    game_pointers[game_index]->cards1[3],
                                                                                    game_pointers[game_index]->turn,
                                                                                    game_pointers[game_index]->points1,
                                                                                    game_pointers[game_index]->points2,
                                                                                    -1,
                                                                                    game_pointers[game_index]->nick2);                                                                                
                }            
                // my turn, cards counter 1
                else if (strcmp(game_pointers[game_index]->turn, nick) == 0 &&
                    game_pointers[game_index]->played_cards_round_counter == 1) { // player's 1 turn and opponent has played
                    sprintf(output,"vb|connection|reconnect|%d|%d|%d|%d|%s|%d|%d|%d|%s\n", game_pointers[game_index]->cards1[0],
                                                                                    game_pointers[game_index]->cards1[1],
                                                                                    game_pointers[game_index]->cards1[2],
                                                                                    game_pointers[game_index]->cards1[3],
                                                                                    game_pointers[game_index]->turn,
                                                                                    game_pointers[game_index]->points1,
                                                                                    game_pointers[game_index]->points2,
                                                                                    game_pointers[game_index]->played_card2,
                                                                                    game_pointers[game_index]->nick2);
                }
                // opp turn, cards counter 1
                else if (strcmp(game_pointers[game_index]->turn, nick) != 0 &&
                    game_pointers[game_index]->played_cards_round_counter == 1) { //player's 2 turn, I played
                    sprintf(output,"vb|connection|reconnect|%d|%d|%d|%d|%s|%d|%d|%d|%s\n", game_pointers[game_index]->cards1[0],
                                                                                    game_pointers[game_index]->cards1[1],
                                                                                    game_pointers[game_index]->cards1[2],
                                                                                    game_pointers[game_index]->cards1[3],
                                                                                    game_pointers[game_index]->turn,
                                                                                    game_pointers[game_index]->points1,
                                                                                    game_pointers[game_index]->points2,
                                                                                    game_pointers[game_index]->played_card1,
                                                                                    game_pointers[game_index]->nick2);
                    
                }                

                send_msg(output, client_socket);
                send_msg("vb|connection|oppconnected\n", game_pointers[game_index]->socket2);
            }
            else { // player 2 was disconnected
                game_pointers[game_index]->socket2 = client_socket;
                // my turn/opp turn, cards counter 0
                if (game_pointers[game_index]->played_cards_round_counter == 0) { // player's 1/2 turn and no card was played
                    
                    sprintf(output,"vb|connection|reconnect|%d|%d|%d|%d|%s|%d|%d|%d|%s\n", game_pointers[game_index]->cards2[0],
                                                                                    game_pointers[game_index]->cards2[1],
                                                                                    game_pointers[game_index]->cards2[2],
                                                                                    game_pointers[game_index]->cards2[3],
                                                                                    game_pointers[game_index]->turn,
                                                                                    game_pointers[game_index]->points2,
                                                                                    game_pointers[game_index]->points1,
                                                                                    -1,
                                                                                    game_pointers[game_index]->nick1);                                                                                
                }
                else if (strcmp(game_pointers[game_index]->turn, nick) == 0 &&
                    game_pointers[game_index]->played_cards_round_counter == 1){ // player's 2 and  turn and opponent has played
                    sprintf(output,"vb|connection|reconnect|%d|%d|%d|%d|%s|%d|%d|%d|%s\n", game_pointers[game_index]->cards2[0],
                                                                                    game_pointers[game_index]->cards2[1],
                                                                                    game_pointers[game_index]->cards2[2],
                                                                                    game_pointers[game_index]->cards2[3],
                                                                                    game_pointers[game_index]->turn,
                                                                                    game_pointers[game_index]->points2,
                                                                                    game_pointers[game_index]->points1,
                                                                                    game_pointers[game_index]->played_card1,
                                                                                    game_pointers[game_index]->nick1);
                }
                // opp turn, cards counter 1
                else if (strcmp(game_pointers[game_index]->turn, nick) != 0 &&
                    game_pointers[game_index]->played_cards_round_counter == 1) { //player's 1 turn, I played
                    sprintf(output,"vb|connection|reconnect|%d|%d|%d|%d|%s|%d|%d|%d|%s\n", game_pointers[game_index]->cards2[0],
                                                                                    game_pointers[game_index]->cards2[1],
                                                                                    game_pointers[game_index]->cards2[2],
                                                                                    game_pointers[game_index]->cards2[3],
                                                                                    game_pointers[game_index]->turn,
                                                                                    game_pointers[game_index]->points2,
                                                                                    game_pointers[game_index]->points1,
                                                                                    game_pointers[game_index]->played_card2,
                                                                                    game_pointers[game_index]->nick1);
                    
                }               

                send_msg(output, client_socket);
                send_msg("vb|connection|oppconnected\n", game_pointers[game_index]->socket1);       
            }        
        }
        else { // to lobby        
            sprintf(output, "vb|connection|reconnect|lobby|%d\n", pl->number_of_win_games);  
            send_msg(output, client_socket);        
        }
    }
}
