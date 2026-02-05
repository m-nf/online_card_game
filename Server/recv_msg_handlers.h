#ifndef recv_msg_handlers
#define recv_msg_handlers

/**
 * Handling nick messages. Checks lenght, duplicates, and manual reconnect.
 */
void handle_nick_msgs(int client_socke, char **splitted_msg);

/**
 * Handles game message - player wants play.
 * There are two possibilities. Creating new game or connecting to a created game.
 * If player connects to a created game, there are two players in game now and can begin.
 * If opponent is not found in specific time - info message is send.
 */
void handle_game_msgs(int client_socket);

/**
 * Handles round messages - start of round, played cards, end of round, end of game.
 * All the processes in round.
 */
void handle_round_msgs(int client_socket, char **splitted_msg);

/**
 * Handles conenction messages. I.e. ping question, ping answer.
 * Reconnect request. Disconnect request.
 */
void handle_connection_msgs(int client_socket, char **splitted_msg, int number_of_splits);

/**
 * Determines who is winner of a game (or if it is a draw) and ends game.
 */
void deal_with_game_end(int game_index);

/**
 * Deals with start of round. Sends a msg to players with inforamtions, that new round
 * starts, which card they drew and whose turn it is.
 * Also sends a msg, when last card was drawn from stock.
 */
void deal_with_start_round(int game_index);

/**
 * Determines who is winner of round and sends msgs to both players.
 * 
 */
void deal_with_round_result(int game_index);

/**
 * Removes played card from hand and sends a message about played card to opponent.
 */
void deal_with_played_card(int game_index, int played_card, char player_nick[]);

/**
 * Add a new card to cards in hand.
 */
void add_card_in_hand(int game_index, int drawed_card, int player);

/**
 * Removes played card from cards in hand.
 */
void remove_card_in_hand(int game_index, int played_card, int player);

/**
 * @brief Waits 30 seconds for joining a opponent to created game.
 * 
 * @param game_index
 */
void wait_for_opponent(int game_index);

#endif