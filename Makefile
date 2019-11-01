.SUFFIXES : .c .o
CC = gcc
CFLAGS = -g -W -Wall -lpthread

all : KvServer Test

.DEFAULT :
	cd src ; $(MAKE) $@
	cd test ; $(MAKE) $@

