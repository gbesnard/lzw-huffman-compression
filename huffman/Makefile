CC = gcc
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -Iheaders
PP = -DDEBUG
all: huffman clean-2

huffman: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o bin/huffman

clean:
	rm -f $(OBJS)

clean-all:
	rm -f $(OBJS) bin/huffman

clean-2 :
	rm src/*.o
