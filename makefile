OBJECTOS_cliente = data.o entry.o message.o list.o swap_bytes.o table_client.o network_client.o network-client-private.o client_stub.o
OBJECTOS_servidor = data.o entry.o message.o list.o swap_bytes.o table_server.o table.o table-private.o network_client.o network-client-private.o table_skel.o table-server-private.o
CC = gcc

all: 
	make table-client table-server

table-client: $(OBJECTOS_cliente)
	$(CC) $(OBJECTOS_cliente) -o table-client -pthread

table-server: $(OBJECTOS_servidor)
	$(CC) $(OBJECTOS_servidor) -o table-server -pthread

data.o: data.c data.h
	gcc -g3 -c -Wall data.c

entry.o: entry.c entry.h
	gcc -g3 -c -Wall entry.c

list.o: list.c list-private.h
	gcc -g3 -c -Wall list.c

table.o: table.c table-private.h
	gcc -g3 -c -Wall table.c

table-private.o: table-private.c table-private.h
	gcc -g3 -c -Wall table-private.c

message.o: message.c message-private.h
	gcc -g3 -c -Wall message.c

swap_bytes.o: swap_bytes.c message-private.h
	gcc -g3 -c -Wall swap_bytes.c

table_client.o: table_client.c network-client-private.h client_stub.h client_stub-private.h
	gcc -g3 -c -Wall table_client.c

table_server.o: table_server.c table-private.h network-client-private.h
	gcc -g3 -c -Wall table_server.c

network_client.o: network_client.c network-client-private.h
	gcc -g3 -c -Wall network_client.c

network-client-private.o: network_client.c network-client-private.h
	gcc -g3 -c -Wall network-client-private.c

client_stub.o: client_stub.c network-client-private.h client_stub.h client_stub-private.h
	gcc -g3 -c -Wall client_stub.c

table_skel.o: table_skel.c table-private.h network-client-private.h
	gcc -g3 -c -Wall table_skel.c

table-server-private.o: table-server-private.c network-client-private.h table-server-private.h
	gcc -g3 -c -Wall table-server-private.c

clean:
	rm -f *.o *~ table-client table-server
