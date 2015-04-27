CC = gcc
CFLAGS = -g -pthread -Wall -O0 -ggdb
EXEC = cmpsc473mr
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm $(OBJS)

