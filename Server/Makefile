BIN=main
CC=gcc
CCFLAGS=-Wall -Wextra -pedantic -g

$(BIN): clean main.o communication.o connection_stuff.o game_functions.o player_functions.o recv_msg_handlers.o
	$(CC) main.o communication.o connection_stuff.o game_functions.o player_functions.o recv_msg_handlers.o -pthread -o $(BIN) $(CCFLAGS)

main.o: main.c
	$(CC) -c main.c $(CCFLAGS)

communication.o: communication.c
	$(CC) -c communication.c $(CCFLAGS)

connection_stuff.o: connection_stuff.c
	$(CC) -c connection_stuff.c $(CCFLAGS)

game_functions.o: game_functions.c
	$(CC) -c game_functions.c $(CCFLAGS)

player_functions.o: player_functions.c
	$(CC) -c player_functions.c $(CCFLAGS)

recv_msg_handlers.o: recv_msg_handlers.c
	$(CC) -c recv_msg_handlers.c $(CCFLAGS)

clean:
	rm *.o $(BIN) -f