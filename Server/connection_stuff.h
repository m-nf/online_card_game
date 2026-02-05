#ifndef connection_stuff
#define connection_stuff

/**
 * If is player in a game, sends a msg to opponent, that opp is disconnected.
 * If player is in game, wait 30 s to reconnect, then forcily end game.
 * Wait 5 min for long reconnect.
 * Then delets player. 
 */ 
void* handle_disconnected_player_thread(void* arg);

/**
 * Pings all players and set flags to 0 for each player. Then waits 5 s
 * and checks if flags are 1 or 0. If 0 -> reconnect.
 * If player reconnects, no ping msgs are sent to him.
 */
void* ping_players();

/**
 * Reconnects client back in lobby or game by sending him all informations, what he needs.
 * Ends thread handle_disconnected_player_thread for this player.
 */
void reconnect_player(int client_socket, char *nick);

#endif