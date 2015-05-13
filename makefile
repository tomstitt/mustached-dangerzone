CC = gcc
CFLAGS = -g -pthread -Wall
EXEC = project
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm $(OBJS)

