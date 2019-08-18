CC = gcc
CFLAGS = -g -W -Wall -lpthread
#LDFLAGS=<링크 옵션>
#LDLIBS=<링크 라이브러리 목록>
OBJS = server.o logger.o btree.o util.o storage.o space.o
#SRCS = server.c logger.c btree.c util.c storage.c space.c
TARGET = kv_server

all : $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^

clean :
		rm *.o
		rm $(TARGET)


#$(TARGET) :	server.o logger.o btree.o worker.o util.o storage.o space.o
#		$(CC) $(CFLAGS) -o $(TARGET) server.o btree.o logger.o worker.o util.o storage.o space.o
#server.o :
#		$(CC) $(CFLAGS) -c -o server.o server.c
#logger.o :
#	  $(CC) $(CFLAGS) -c -o logger.o logger.c
#btree.o :
#	  $(CC) $(CFLAGS) -c -o btree.o btree.c
#worker.o :
#		$(CC) $(CFLAGS) -c -o worker.o worker.c
#util.o :
#		$(CC) $(CFLAGS) -c -o util.o util.c
#storage.o :
#		$(CC) $(CFLAGS) -c -o storage.o storage.c
#space.o :
#		$(CC) $(CFLAGS) -c -o space.o space.c
