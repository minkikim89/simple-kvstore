CC = gcc
CFLAGS = -g -W -Wall

OBJS = btree_test.o

TARGET = btree_test

all : $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^

clean :
	rm -f $(OBJS) $(TARGET)
