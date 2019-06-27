CC = gcc
CFLAGS = -g
TARGET = kv_server

$(TARGET) :	server.o logger.o worker.o util.o storage.o space.o
		$(CC) $(CFLAGS) -o $(TARGET) server.o logger.o worker.o util.o storage.o space.o
server.o :
		$(CC) $(CFLAGS) -c -o server.o server.c
logger.o :
	  $(CC) $(CFLAGS) -c -o logger.o logger.c
worker.o :
		$(CC) $(CFLAGS) -c -o worker.o worker.c
util.o :
		$(CC) $(CFLAGS) -c -o util.o util.c
storage.o :
		$(CC) $(CFLAGS) -c -o storage.o storage.c
space.o :
		$(CC) $(CFLAGS) -c -o space.o space.c
clean :
		rm *.o kv_server
