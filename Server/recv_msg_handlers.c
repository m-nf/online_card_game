/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Contains handler for parsing messages. In this file is game logic.
 * @version 1.0
 * @date 2022-01-30
 */

#include "definitions.h"

/**
 * Handling nick messages. Checks lenght, duplicates, and manual reconnect.
 */
void handle_nick_msgs(int client_socket, char **splitted_msg) {          
    /*
    count of players is full
    if (number_of_players == number_of_connected_players) {
        send_msg("vb|nick|full\n", client_socket);
    }
    */  

    /* too long nickname */
    if (strlen(splitted_msg[2]) > 10) {
        deal_with_invalid_msg(client_socket);
    }
    /* nick already exist */
    else if (get_player_index_ni(splitted_msg[2]) != -1) {
        // reconnect
        if (players[get_player_index_ni(splitted_msg[2])]->waiting_for_reconnect == 1) { // long reconnect
            send_msg("vb|nick|ok\n", client_socket);
            sleep(1);
            reconnect_player(client_socket, splitted_msg[2]);
        }
        // duplicate
        else {
            send_msg("vb|nick|duplicate\n", client_socket);
        }
    }
    /* nick is ok */
    else {        
        send_msg("vb|nick|ok\n", client_socket);
        put_nickname(splitted_msg[2], client_socket);
    }
}

/**
 * Handles conenction messages. I.e. ping question, ping answer.
 * Reconnect request. Disconnect request.
 */
void handle_connection_msgs(int client_socket, char **splitted_msg, int number_of_splits) {    

    /* ping question from client */
    if ((number_of_splits == NUM_SPLIT_CONNECTION) && strcmp(splitted_msg[2], "ping") == 0) {
        send_msg("vb|connection|ping|ok\n", client_socket);
    }
    /* ping answer from client */
    else if ((number_of_splits == NUM_SPLIT_CONNECTION + 1) && strcmp(splitted_msg[2], "ping") == 0) {
        
        if (strcmp(splitted_msg[3], "ok") == 0)
            get_player(client_socket)->connected = 1;
        else {
            deal_with_invalid_msg(client_socket);
        }
    }
    /* reconnect */
    else if ((number_of_splits == NUM_SPLIT_CONNECTION + 1) && strcmp(splitted_msg[2], "reconnect") == 0) {

        if (strlen(splitted_msg[3]) <= 10 && get_player_index_ni(splitted_msg[3]) != -1) { // nickname is ok
            sleep(1);
            reconnect_player(client_socket, splitted_msg[3]);
        }
        else { // too long nickname or doesn't exist
            deal_with_invalid_msg(client_socket);
        }
    }
    /* disconnect player */
    else if ((number_of_splits == NUM_SPLIT_CONNECTION) && strcmp(splitted_msg[2], "disconnect") == 0) {       
        
        int player_index = get_player_index(client_socket);        
        if (player_index != -1 && players[player_index]->waiting_for_reconnect == 0) {
            int *cl_sc = (int *) malloc(sizeof(int));
            *cl_sc = client_socket;            
            pthread_t thread_id;                     
            pthread_create(&thread_id, NULL, handle_disconnected_player_thread, (void *)cl_sc);                     
        }       
    }           
    else {       
        deal_with_invalid_msg(client_socket);
    }
}

/**
 * Handles game message - player wants play.
 * There are two possibilities. Creating new game or connecting to a created game.
 * If player connects to a created game, there are two players in game now and can begin.
 * If opponent is not found in specific time - info message is send.
 */
void handle_game_msgs(int client_socket) {    
    pthread_mutex_lock(&game_lock);   

    // player doesn't have nickname yet
    if (strcmp(get_player(client_socket)->nick, "") == 0) {      
 
        deal_with_invalid_msg(client_socket);
        pthread_mutex_unlock(&game_lock);
    }
    // game rooms are full 
    else if (number_of_games == number_of_running_games) {        
        send_msg("vb|game|full", client_socket);
        pthread_mutex_unlock(&game_lock);
    }
    // player is already in game   
    else if (get_game_index(client_socket) != -1) { 
        deal_with_invalid_msg(client_socket);
        pthread_mutex_unlock(&game_lock);        
    }
    // everything ok
    else {       
        int game_index;
        char output[BUFF_LENGHT];        
        
        game_index = get_waiting_game(); // searching for waiting game
        if (game_index != -1) {  // there is waiting game           
            
            game_pointers[game_index]->both_players = 1;
            game_pointers[game_index]->socket2 = client_socket;
            strcpy(game_pointers[game_index]->nick2, get_player(client_socket)->nick);
			send_msg("vb|game|wait\n", client_socket);

            pthread_mutex_unlock(&game_lock);            
        }
        else { // there isn't waiting game - create new game
               
            game_index = create_new_game(client_socket); // creating new game            
            number_of_running_games++;

            pthread_mutex_unlock(&game_lock); 
            
            send_msg("vb|game|wait\n", client_socket);
            wait_for_opponent(game_index); // waiting for opponent

            if (game_pointers[game_index]->both_players == 0) { // no opponent found
                delete_game(client_socket);
                number_of_running_games--;                
                send_msg("vb|game|wait|error\n", client_socket);                              
                return;
            }            
            else { // opponent found
                prepare_game(game_index);

                sprintf(output, "vb|game|play|%s\n", game_pointers[game_index]->turn);
                send_msg(output, game_pointers[game_index]->socket1);
                send_msg(output, game_pointers[game_index]->socket2);                
            }                            
        }
    }    
}

/**
 * Handles round messages - start of round, played cards, end of round, end of game.
 * All the processes in round.
 */
void handle_round_msgs(int client_socket, char **splitted_msg) {
    int played_card;
    int game_index = get_game_index(client_socket);
    char player_nick[10];
	char output[BUFF_LENGHT];
    
    strcpy(player_nick, get_player(client_socket)->nick);
    
    pthread_mutex_lock(&round_lock);

    if (game_index == -1) { // game doesn't exists
        deal_with_invalid_msg(client_socket);
    }
    else if (sscanf(splitted_msg[3], "%d", &played_card) != 1) { // sended card isn't a number
    	deal_with_invalid_msg(client_socket);
    }
    else if (played_card < 0 || played_card > 31) { // sended card isn't a card
        deal_with_invalid_msg(client_socket);
    }
    else if (strcmp(player_nick, game_pointers[game_index]->turn) != 0) { // message at wrong turn
       deal_with_invalid_msg(client_socket);
    }
    else {
        if (game_pointers[game_index]->played_cards_round_counter == 1) { // second player just played
            deal_with_played_card(game_index, played_card, player_nick);  // dealing with played card         
            deal_with_round_result(game_index);                           // dealing with round result
            sleep(WAIT_END_BTW_MSGS);

            game_pointers[game_index]->played_cards_round_counter = 0;
            if (strcmp(player_nick, game_pointers[game_index]->nick1) == 0) { // setting whose turn it is
                strcpy(game_pointers[game_index]->turn, game_pointers[game_index]->nick2);
            }
            else {
                strcpy(game_pointers[game_index]->turn, game_pointers[game_index]->nick1);
            }

            game_pointers[game_index]->round_counter++; // increment round counter            
            if (game_pointers[game_index]->round_counter == NUMBER_OF_ROUNDS) { // end of game         
               deal_with_game_end(game_index); 
               delete_game(client_socket);
            }
            else { // new round
                deal_with_start_round(game_index);
            }
        }
        else { // only one player played
            deal_with_played_card(game_index, played_card, player_nick); // dealing with played card
            
            game_pointers[game_index]->played_cards_round_counter = 1;
            if (strcmp(player_nick, game_pointers[game_index]->nick1) == 0) { // setting whose turn it is				
                strcpy(game_pointers[game_index]->turn, game_pointers[game_index]->nick2);
				sprintf(output, "vb|round|turn|%s\n", game_pointers[game_index]->turn);
            }
            else {
                strcpy(game_pointers[game_index]->turn, game_pointers[game_index]->nick1);
				sprintf(output, "vb|round|turn|%s\n", game_pointers[game_index]->turn);
            }
			send_msg(output, game_pointers[game_index]->socket1);
			send_msg(output, game_pointers[game_index]->socket2);
        }
    }

    pthread_mutex_unlock(&round_lock);
}

/**
 * Determines who is winner of a game (or if it is a draw) and ends game.
 */
void deal_with_game_end(int game_index) {
    char output[BUFF_LENGHT];

    if (game_pointers[game_index]->points1 > game_pointers[game_index]->points2) { // player 1 is winner        
        sprintf(output, "vb|game|winner|%s\n", game_pointers[game_index]->nick1);
        send_msg(output, game_pointers[game_index]->socket1);
        send_msg(output, game_pointers[game_index]->socket2);
                
        players[get_player_index(game_pointers[game_index]->socket1)]->number_of_win_games++;
        sleep(WAIT_END_BTW_MSGS);
        sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket1)]->number_of_win_games);  
        send_msg(output, game_pointers[game_index]->socket1);                
        
        sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket2)]->number_of_win_games);  
        send_msg(output, game_pointers[game_index]->socket2);
    }
    else if (game_pointers[game_index]->points1 < game_pointers[game_index]->points2) { // player 2 is winner
        sprintf(output, "vb|game|winner|%s\n", game_pointers[game_index]->nick2);
        send_msg(output, game_pointers[game_index]->socket1);
        send_msg(output, game_pointers[game_index]->socket2);                
        
        sleep(WAIT_END_BTW_MSGS);
        sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket1)]->number_of_win_games);  
        send_msg(output, game_pointers[game_index]->socket1);                
        
        players[get_player_index(game_pointers[game_index]->socket2)]->number_of_win_games++;
        sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket2)]->number_of_win_games);  
        send_msg(output, game_pointers[game_index]->socket2);
    }
    else { // tie
        sprintf(output, "vb|game|winner|tie\n");
        send_msg(output, game_pointers[game_index]->socket1);
        send_msg(output, game_pointers[game_index]->socket2);
        
        sleep(WAIT_END_BTW_MSGS);
        sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket1)]->number_of_win_games);  
        send_msg(output, game_pointers[game_index]->socket1);                
        
        sprintf(output, "vb|game|end|%d\n", players[get_player_index(game_pointers[game_index]->socket2)]->number_of_win_games);  
        send_msg(output, game_pointers[game_index]->socket2);
    }    
}

/**
 * Deals with start of round. Sends a msg to players with inforamtions, that new round
 * starts, which card they drew and whose turn it is.
 * Also sends a msg, when last card was drawn from stock.
 */
void deal_with_start_round(int game_index) {
    char output[BUFF_LENGHT];

    if (game_pointers[game_index]->stock_card_counter <= 30) { //start a round, draw card info
        int card_to_draw;
        
        // player 1
        card_to_draw = game_pointers[game_index]->shuffled_stock[game_pointers[game_index]->stock_card_counter];
        add_card_in_hand(game_index, card_to_draw, 1);
        game_pointers[game_index]->stock_card_counter++;        
        
        sprintf(output, "vb|round|start|%d|%s\n", card_to_draw, game_pointers[game_index]->turn);
        send_msg(output, game_pointers[game_index]->socket1);
        
        //player 2
        card_to_draw = game_pointers[game_index]->shuffled_stock[game_pointers[game_index]->stock_card_counter];
        add_card_in_hand(game_index, card_to_draw, 2);
        game_pointers[game_index]->stock_card_counter++;        
        
        sprintf(output, "vb|round|start|%d|%s\n", card_to_draw, game_pointers[game_index]->turn);
        send_msg(output, game_pointers[game_index]->socket2);
    }
    if (game_pointers[game_index]->stock_card_counter == NUMBER_OF_CARDS) { // last card in stock was drawn
        send_msg("vb|round|lastcard\n", game_pointers[game_index]->socket1);
        send_msg("vb|round|lastcard\n", game_pointers[game_index]->socket2);
        game_pointers[game_index]->stock_card_counter++;
    }
    if (game_pointers[game_index]->stock_card_counter >= NUMBER_OF_CARDS + 1) { //runned out of cards --> card to draw is -1
        int card_to_draw = -1;             
        
        sprintf(output, "vb|round|start|%d|%s\n", card_to_draw, game_pointers[game_index]->turn);
        send_msg(output, game_pointers[game_index]->socket1);             
        
        sprintf(output, "vb|round|start|%d|%s\n", card_to_draw, game_pointers[game_index]->turn);
        send_msg(output, game_pointers[game_index]->socket2);
    }
}

/**
 * Determines who is winner of round and sends msgs to both players.
 * 
 */
void deal_with_round_result(int game_index) {
    char output[BUFF_LENGHT];
    
    int played_card_value1 = cards_values[game_pointers[game_index]->played_card1];
    int played_card_value2 = cards_values[game_pointers[game_index]->played_card2];

    // who won round
    if (played_card_value1 > played_card_value2) { // player 1 win
        game_pointers[game_index]->points1++;

        sprintf(output, "vb|round|roundresult|nickname|%s|%d|%d\n", game_pointers[game_index]->nick1, 
                                                                    game_pointers[game_index]->points1, 
                                                                    game_pointers[game_index]->points2);
        send_msg(output, game_pointers[game_index]->socket1);
        
        sprintf(output, "vb|round|roundresult|nickname|%s|%d|%d\n", game_pointers[game_index]->nick1, 
                                                                    game_pointers[game_index]->points2, 
                                                                    game_pointers[game_index]->points1);
        send_msg(output, game_pointers[game_index]->socket2);
    }
    else if (played_card_value1 < played_card_value2) { // player 2 win
        game_pointers[game_index]->points2++;

        sprintf(output, "vb|round|roundresult|nickname|%s|%d|%d\n", game_pointers[game_index]->nick2, 
                                                                    game_pointers[game_index]->points1, 
                                                                    game_pointers[game_index]->points2);
        send_msg(output, game_pointers[game_index]->socket1); 
        
        sprintf(output, "vb|round|roundresult|nickname|%s|%d|%d\n", game_pointers[game_index]->nick2, 
                                                                    game_pointers[game_index]->points2, 
                                                                    game_pointers[game_index]->points1);
        send_msg(output, game_pointers[game_index]->socket2);
    }
    else { // tie
        game_pointers[game_index]->points1++;
        game_pointers[game_index]->points2++;

        sprintf(output, "vb|round|roundresult|tie|%d|%d\n", game_pointers[game_index]->points1, 
                                                            game_pointers[game_index]->points2);
        send_msg(output, game_pointers[game_index]->socket1);
        
        sprintf(output, "vb|round|roundresult|tie|%d|%d\n", game_pointers[game_index]->points2, 
                                                            game_pointers[game_index]->points1);
        send_msg(output, game_pointers[game_index]->socket2);
    }   	
}

/**
 * Removes played card from hand and sends a message about played card to opponent.
 */
void deal_with_played_card(int game_index, int played_card, char player_nick[]) {
    char output[BUFF_LENGHT];

    if (strcmp(player_nick, game_pointers[game_index]->nick1) == 0) { // player 1 played
        game_pointers[game_index]->played_card1 = played_card;
        
        remove_card_in_hand(game_index, played_card, 1);
        
        sprintf(output, "vb|round|oppplayedcard|%d\n", played_card);
        send_msg(output, game_pointers[game_index]->socket2);
    }
    else { // player 2 played
        game_pointers[game_index]->played_card2 = played_card;
        
        remove_card_in_hand(game_index, played_card, 2);
        
        sprintf(output, "vb|round|oppplayedcard|%d\n", played_card);
        send_msg(output, game_pointers[game_index]->socket1);
    }    
}

/**
 * Add a new card to cards in hand.
 */
void add_card_in_hand(int game_index, int drawed_card, int player) {
    int i;
    if (player == 1) {
        for (i = 0; i < NUMBER_OF_CARDS_IN_HAND; i++) {
            if (game_pointers[game_index]->cards1[i] == -1) {
                game_pointers[game_index]->cards1[i] = drawed_card;
                return;
            }
        }
    }
    else {
        for (i = 0; i < NUMBER_OF_CARDS_IN_HAND; i++) {
            if (game_pointers[game_index]->cards2[i] == -1) {
                game_pointers[game_index]->cards2[i] = drawed_card;
                return;
            }
        }
    }
    
}

/**
 * Removes played card from cards in hand.
 */
void remove_card_in_hand(int game_index, int played_card, int player) {
    int i;
    if (player == 1) {
        for (i = 0; i < NUMBER_OF_CARDS_IN_HAND; i++) {
            if (game_pointers[game_index]->cards1[i] == played_card) {
                game_pointers[game_index]->cards1[i] = -1;
                return;
            }
        }
    }
    else {
        for (i = 0; i < NUMBER_OF_CARDS_IN_HAND; i++) {
            if (game_pointers[game_index]->cards2[i] == played_card) {
                game_pointers[game_index]->cards2[i] = -1;
                return;
            }
        }
    }
}

/**
 * @brief Waits 30 seconds for joining a opponent to created game.
 * 
 * @param game_index
 */
void wait_for_opponent(int game_index) {
    int time_counter = 0;
    while (game_pointers[game_index]->both_players == 0 && time_counter < (WAIT_FOR_OPPONENT / 2)) {
        time_counter++;
        sleep(2);
    }
}

