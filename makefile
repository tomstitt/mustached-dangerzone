CC = gcc
CFLAGS = -g -pthread -Wall
EXEC = cmpsc473mr
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm $(OBJS)

