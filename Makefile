main: cliente.c comandos.c servidor.c rawSocket.c commonFunctions.c 
	gcc -o cliente cliente.c comandos.c rawSocket.c commonFunctions.c
	gcc -o servidor servidor.c comandos.c rawSocket.c commonFunctions.c

purge:
	rm cliente
	rm servidor