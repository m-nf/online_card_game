#ifndef communication
#define communication

/**
 * Sends message in parameter to client identified by socket in parameter.
 * Writes what has been send to console.
 */
void send_msg(char *msg, int client_socket);

/**
 * First messages handler. Determines if message has header. Passes on further to a 
 * specific msg handler according to type of msg (nick, game, round, connection).
 */
void handle_recv_msg(char **splitted_msg, int number_of_splits, int client_socket);

/**
 * Main function for accepting new connections, listening and recieving messages.
 * Contain infinity loop. After msg is recieved, creates new thread for its processing.
 * 
 * Recieving is maded by recieving one char till \\\n occurence or to BUFF_LENGHT (150).
 * Also if first character isn't 'v' --> invalid msg.
 *                        second 'b' 
 *                        and third '|'
 * 
 * When a connection is accepted it creates new player.
 */
void communicate(int port, char * ip);

/**
 * @brief Thread for handling clients messages.
 * 
 * @param arg 
 * @return void* 
 */
void* serve_communicate_thread(void *arg);

/**
 * Count invalid messages. If player has three or more invalid msgs, is deleted.
 */
void deal_with_invalid_msg(int client_socket);

/**
 * @brief Closes sockets, which should be closed, and removes them from file descriptor
 * set. 
 */
void close_sockets();

/**
 * @brief Adds client socket to sockets, which have to be closed.
 * 
 * @param client_socekt 
 */
void add_socket_to_close(int client_socekt);

/**
 * @brief Splits string.
 * 
 * @param str String to be splitted.
 * @param delim 
 * @param numSplits Number of obtains parts.
 * @return char** 
 */
char** str_split( char* str, char delim, int* numSplits);

#endif