CC = gcc
CFLAGS = -g -W -Wall
LDFLAGS = -lpthread

OBJS = server.o logger.o btree.o util.o storage.o space.o

TARGET = kv_server

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean :
		rm -f $(OBJS) $(TARGET)
