cc := gcc

CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c11 -Wall -lSDL2_image -lm -g

SRCS := screen.c starHandler.c geom.c

OBJS := $(SRCS:.c=.o)

EXEC := starGame

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
