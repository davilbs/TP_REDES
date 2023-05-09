build:
	gcc -o client/client client/client.c
	gcc -o server/server server/server.c
clean:
	rm client/client
	rm server/server
	rm server/*.txt