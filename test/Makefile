CC        := gcc
LIBS      := -lbitcask -lpthread  -ljansson
LDFLAGS   := -L ../ -L.
INCLUDE   := -I ../src
CFLAGS    := -g -O0  -Wall
SRCS := $(wildcard *.c)
TARGET  := $(patsubst %.c, %, $(SRCS))
OBJDIR = .

.c:
	$(CC) $(CFLAGS)   -o $@ $<   $(INCLUDE) $(LDFLAGS) $(LIBS)
all: $(TARGET) 
clean:
	rm -rf *.o
	rm -rf bitcask_demo 