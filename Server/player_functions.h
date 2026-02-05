#ifndef player_functions
#define player_functions


/**
 * Creates new player. Allocates mememory, sets the variables.
 */
void create_player(int client_socket);

/**
 * @brief Get the player object by client_socket.
 * 
 * @param client_socket 
 * @return player* 
 */
player* get_player(int client_socket);

/**
 * Gives nick to the player identified by socket.
 */
void put_nickname(char *nick, int client_socket);

/**
 * Return players index. Searching is by nick.
 */
int get_player_index_ni(char *nick);

/**
 * Return players index. Searching is by socket.
 */
int get_player_index(int client_socket);

/**
 * @brief Disconnect and delete player.
 * 
 * @param client_socket 
 */
void delete_player(int client_socket);


#endif