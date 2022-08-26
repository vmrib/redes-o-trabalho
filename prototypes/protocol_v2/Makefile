all:
	gcc -Wall -std=gnu17 client.c raw_socket.c prot_client.c packet.c -o client
	gcc -Wall -std=gnu17 server.c raw_socket.c prot_server.c packet.c -o server

clean:
	rm -f client server test_client

test_client:
	gcc -Wall -std=gnu17 test_client.c raw_socket.c prot_client.c packet.c -o test_client