all:
	gcc -Wall -std=gnu17 client.c raw_socket.c prot_client.c packet.c error.c -o client
	gcc -Wall -std=gnu17 server.c raw_socket.c prot_server.c packet.c error.c -o server

clean:
	rm -f client server test_client test_server

test_client:
	gcc -Wall -std=gnu17 test_client.c raw_socket.c prot_client.c packet.c error.c -o test_client

test_server:
	gcc -Wall -std=gnu17 test_server.c raw_socket.c prot_server.c packet.c error.c -o test_server