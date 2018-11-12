server : tcp_server.o
	gcc -o server tcp_server.o 

tcp_server.o : tcp_server.c
	gcc -c tcp_server.c

client : tcp_client.o
	gcc -pthread -o client tcp_client.o 

tcp_client.o : tcp_client.c
	gcc -c tcp_client.c

clean :
	rm server tcp_server.o client tcp_client.o
