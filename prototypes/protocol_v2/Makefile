all:
	gcc -Wall -std=gnu17 client.c raw_socket.c protocol.c -o client
	gcc -Wall -std=gnu17 server.c raw_socket.c protocol.c -o server

clean:
	rm client server