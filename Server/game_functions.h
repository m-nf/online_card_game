#ifndef game_functions
#define game_functions

/**
 * @brief Forcily ends game (when opponent left game). Winner is the player
 * whose stayed in a game.
 * Player in parameter is the one, whose left.
 * 
 * @param pl 
 * @param game_index 
 */
void forcily_end_game(player *pl, int game_index);

/**
 * @brief Prepares game and sends to both players their cards and opponent's nick.
 * 
 * @param game_index 
 */
void prepare_game(int game_index);

/**
 * @brief Creates new game. Allocates memory for game. Pointer is in 
 *        array game_pointers.
 *        Sets both_players to 0 (false).
 *        Sets socket1 and nick1 according to player, which created the game.
 * 
 * @param client_socket 
 * @return int Index of created game.
 */
int create_new_game(int client_socket);

/**
 * @brief Return index of game, if there is game which is waiting for opponent (second player).
 *        Return -1 if there isn't such game. 
 * @return int 
 */
int get_waiting_game();

/**
 * @brief Frees memory of the game.
 *        Set pointer of deleted game to NULL.
 *        Decrease number of running games.
 * 
 * @param client_socket 
 */

void delete_game(int client_socket);

/**
 * @brief Return index of game, if there is such a game.
 *        Return -1, if there isn't.
 *        Searching is by sockets.        
 * 
 * @param client_socket 
 * @return int 
 */
int get_game_index(int client_socket);

/**
 * Return game index, searching by nick.
 */
int get_game_index_ni(char nick[]);

/**
 * @brief Shuffles stock of cards for a game identified by index.
 * 
 * @param index 
 */
void shuffle_card_stock(int index);

#endif