CC = gcc
CFLAGS = -g -pthread
EXEC = cmpsc473mr
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

