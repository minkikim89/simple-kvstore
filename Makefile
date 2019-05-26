CC = gcc
CFLAGS = -g
TARGET = kv_server

$(TARGET) :	server.o
		$(CC) $(CFLAGS) -o $(TARGET) server.o
server.o :
		$(CC) $(CFLAGS) -c -o server.o server.c
clean :
		rm *.o kv_server
