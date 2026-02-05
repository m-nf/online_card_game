/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Contains functions for communication with clients.
 * @version 1.0
 * @date 2022-01-30
 */

#include "definitions.h"

/**
 * Sends message in parameter to client identified by socket in parameter.
 * Writes what has been send to console.
 */
void send_msg(char *msg, int client_socket) {
    pthread_mutex_lock(&send_lock);
    write(client_socket, msg, strlen(msg));
    printf("Send %s", msg);
    pthread_mutex_unlock(&send_lock);
}

/**
 * First messages handler. Determines if message has header. Passes on further to a 
 * specific msg handler according to type of msg (nick, game, round, connection).
 */
void handle_recv_msg(char **splitted_msg, int number_of_splits, int client_socket) {    
    if ((number_of_splits >= 1) && (strcmp(splitted_msg[0], "vb") == 0)) {
        
        /* nick messages */
        if ((number_of_splits == NUM_SPLIT_NICK) && (strcmp(splitted_msg[1], "nick") == 0)) {        
            handle_nick_msgs(client_socket, splitted_msg);
        }

        /* game messages */
        else if ((number_of_splits == NUM_SPLIT_GAME) && (strcmp(splitted_msg[1], "game") == 0)) {
            if (strcmp(splitted_msg[2], "play") == 0) {
                handle_game_msgs(client_socket);               
            }
            else {
                deal_with_invalid_msg(client_socket);
            }
        }

        /* round messages */
        else if ((number_of_splits == NUM_SPLIT_ROUND) && (strcmp(splitted_msg[1], "round") == 0)) {
            if (strcmp(splitted_msg[2], "playedcard") == 0) {
                handle_round_msgs(client_socket, splitted_msg);               
            }
            else {
                deal_with_invalid_msg(client_socket);
            }
        }       

        /* connection messages */
        else if ((number_of_splits >= NUM_SPLIT_CONNECTION) && (strcmp(splitted_msg[1], "connection") == 0)) {
            handle_connection_msgs(client_socket, splitted_msg, number_of_splits);
        }

        /* trash messages */
        else {
            deal_with_invalid_msg(client_socket);
        }
    }
    else {
        deal_with_invalid_msg(client_socket);
    }
}

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
void communicate(int port, char * ip) {
    int invalid_char;
    int server_socket;
	int client_socket, fd;
	int return_value;	
    int buff_index;
    int ibuf[BUFF_LENGHT];
	unsigned int addr_lenght = sizeof(struct sockaddr_in);
	int a2read;
    int *cl_sc;
	struct sockaddr_in server_addr, client_addr;
	fd_set tests;
    //socklen_t remote_addr_len;
	pthread_t thread_id;
    thread_parameters *thread_parameter;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int param = 1;
    return_value = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&param, sizeof(int));
	
	if (return_value == -1)
		printf("setsockopt - ERROR\n");

	memset(&server_addr, 0, sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
    return_value = inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
    if (return_value != 1) {
        printf("Bad ip address.");
        return;
    }    

	return_value = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
	if (return_value == 0) 
		printf("Bind - OK\n");
	else {
		printf("Bind - ERROR\n");
		return;
	}

	return_value = listen(server_socket, number_of_players);
	if (return_value == 0){
		printf("Listen - OK\n");
	} else {
		printf("Listen - ERROR\n");
        return;
	}
    
	// cleans set of descriptor
	FD_ZERO(&clients_sockets);      
	FD_SET(server_socket, &clients_sockets);    
    
	for (;;) {    	
        tests = clients_sockets;       
        
		// set of descriptors is overwrited by select
		return_value = select( FD_SETSIZE, &tests, ( fd_set *)0, ( fd_set *)0, ( struct timeval *)0 );
        
		if (return_value < 0) {
			printf("Select - ERROR\n");
			return;
		}
		// excepts stdin, stdout, stderr
		for (fd = 3; fd < FD_SETSIZE; fd++) {
            
            // is the socket in test?
			if (FD_ISSET(fd, &tests)) {
			
            // it is server socket - we accept new connection
				if (fd == server_socket) {					
                    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_lenght);
					FD_SET(client_socket, &clients_sockets);      
                    
                    create_player(client_socket); // creates new player
                    printf("New player connected.\n");			
				}
				// it is client socket - we recieves data
				else {                    
					// number of bytes for reading
					ioctl( fd, FIONREAD, &a2read );
					// we have something to read
					if (a2read > 0) {                       

                        thread_parameter = malloc(sizeof(thread_parameters));
                        buff_index = 0;
                        invalid_char = 0;
                        bzero(ibuf, sizeof(int) * BUFF_LENGHT);
                        
                        while (buff_index < BUFF_LENGHT && read(fd, &ibuf[buff_index], 1) == 1) {
                            if(buff_index < 3) {
                                if(buff_index == 0 && (ibuf[buff_index] != FIRST_MSG_CHAR)) {          
                                    if (ibuf[buff_index] < 0 || ibuf[buff_index] > 127) { // check if it is a valid char, i.e. [0,127]
                                        invalid_char = 1;
                                    }
                                    deal_with_invalid_msg(fd);
                                    break;
                                }
                                if(buff_index == 1 && (ibuf[buff_index] != SECOND_MSG_CHAR)) {                                    
                                    if (ibuf[buff_index] < 0 || ibuf[buff_index] > 127) { // check if it is a valid char, i.e. [0,127]
                                        invalid_char = 1;
                                    }
                                    deal_with_invalid_msg(fd);
                                    break;
                                }                               
                                if(buff_index == 2 && (ibuf[buff_index] != THIRD_MSG_CHAR)) {                                                           
                                    if (ibuf[buff_index] < 0 || ibuf[buff_index] > 127) { // check if it is a valid char, i.e. [0,127]
                                        invalid_char = 1;
                                    }
                                    deal_with_invalid_msg(fd);
                                    break;
                                }
                            }
                            // check if it is a valid char, i.e. [0,127]
                            if (ibuf[buff_index] < 0 || ibuf[buff_index] > 127) {
                                invalid_char = 1;
                                deal_with_invalid_msg(fd);
                                break;
                            }
                            
                            thread_parameter->msg[buff_index] = (char) ibuf[buff_index];                            

                            if ('\n' == thread_parameter->msg[buff_index]) {
                                thread_parameter->msg[buff_index] = '\0';
                                break;
                            }                            
                            buff_index++;
                        }
                        
                        // check if in buffer are only writable chars                        
                        if (invalid_char == 0) {
                            printf("Recieved: %s\n", thread_parameter->msg);
                            
                            //creating thread for processing the message
                            thread_parameter->client_socket = fd;                        
			                pthread_create(&thread_id, NULL, serve_communicate_thread, (void *)thread_parameter);
                        }
                        else {
                            printf("Recieved: invalid char.\n");
                        }                       
                        
					}
					// something bad on socket - tryies to reconnect and close and remove socket from sockets
					else {                        
                        int player_index = get_player_index(fd);                        
                        if (player_index != -1 && players[player_index]->waiting_for_reconnect == 0) {
                            cl_sc = malloc(sizeof(int));
                            *cl_sc = fd;
                            pthread_create(&thread_id, NULL, handle_disconnected_player_thread, (void *)cl_sc);                                                     
                        }
                        
                        add_socket_to_close(fd);

                        printf("Client socket closed, deleted from sockets.\n");               											
					}                    
				}
			}
		}
        close_sockets();     
	}    
}

/**
 * @brief Adds client socket to sockets, which have to be closed.
 * 
 * @param client_socekt 
 */
void add_socket_to_close(int client_socket) {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (sockets_to_close[i] == -1) {
            sockets_to_close[i] = client_socket;
            break;
        }
    }
}

/**
 * @brief Closes sockets, which have to be closed, and removes them from file descriptor
 * set. 
 */
void close_sockets() {
    int i;
    for (i = 0; i < number_of_players; i++) {
        if (sockets_to_close[i] != -1) {
            if (FD_ISSET(sockets_to_close[i], &clients_sockets)) {
                close(sockets_to_close[i]);
                FD_CLR(sockets_to_close[i], &clients_sockets);
            }
            sockets_to_close[i] = -1;
        }
    }
}

/**
 * @brief Thread for handling clients messages.
 * 
 * @param arg 
 * @return void* 
 */
void* serve_communicate_thread(void *arg) {
    thread_parameters *thread_parameter = (thread_parameters *) arg;    
    char** splitted_msg;
    int number_of_splits = 0;

    splitted_msg = str_split(thread_parameter->msg, '|', &number_of_splits);

    if (splitted_msg == NULL) {
        printf("str_split ERROR\n");
    }
    else {
        handle_recv_msg(splitted_msg, number_of_splits, thread_parameter->client_socket);
    }    
   

    if (splitted_msg != NULL) {
        free(splitted_msg);
    }
    if (thread_parameter != NULL) {
        free(thread_parameter);
    }   
    
    return 0;
}

/**
 * Count invalid messages. If player has three or more invalid msgs, is deleted.
 */
void deal_with_invalid_msg(int client_socket) {    
    if (get_player(client_socket) != NULL) {
	    get_player(client_socket)->invalid_msg_counter++;

	    if (get_player(client_socket) != NULL && get_player(client_socket)->invalid_msg_counter >= 3) {
		    delete_player(client_socket);      
	    }
	}
}

/**
 * @brief Splits string.
 * 
 * @param str String to be splitted.
 * @param delim 
 * @param numSplits Number of obtains parts.
 * @return char** 
 */
char** str_split( char* str, char delim, int* numSplits ) {
    char** ret;
    int retLen = 0;
    char* c;

    if ((str == NULL) || (delim == '\0')) {        
        ret = NULL;
        retLen = -1;
    }
    else {
        retLen = 0;
        c = str;

        do {
            if (*c == delim) {
                retLen++;
            }
            c++;
        } while (*c != '\0');

        ret = malloc((retLen + 1) * sizeof(*ret));
        ret[retLen] = NULL;

        c = str;
        retLen = 1;
        ret[0] = str;

        do {
            if (*c == delim) {
                ret[retLen++] = &c[1];
                *c = '\0';
            }
            c++;
        } while (*c != '\0');
    }

    if (numSplits != NULL) {
        *numSplits = retLen;
    }

    return ret;
}

/*
while (buff_index < BUFF_LENGHT && read(fd, &thread_parameter->msg[buff_index], 1) == 1) {
                            if(buff_index < 3) {
                                if(buff_index == 0 && (thread_parameter->msg[0] != FIRST_MSG_CHAR)) {                                    
                                    break;
                                }
                                if(buff_index == 1 && (thread_parameter->msg[1] != SECOND_MSG_CHAR)) {                                    
                                    break;
                                }                               
                                if(buff_index == 2 && (thread_parameter->msg[2] != THIRD_MSG_CHAR)) {                                    
                                    break;
                                }
                            }
                            // check if it is a valid char, i.e. >= 0
                            if (thread_parameter->msg[buff_index] > 127) {
                                invalid_char = 1;
                            }

                            

                            if ('\n' == thread_parameter->msg[buff_index]) {
                                thread_parameter->msg[buff_index] = '\0';
                                break;
                            }                            
                            buff_index++;
                        }
                        */
