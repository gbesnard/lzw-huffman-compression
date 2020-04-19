CC = gcc --std gnu11
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -Iheaders -O3 
PP = -DDEBUG
all: lzw clean-2

lzw: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o bin/lzw

clean:
	rm -f $(OBJS)

clean-all:
	rm -f $(OBJS) bin/lzw

clean-2 :
	rm src/*.o
