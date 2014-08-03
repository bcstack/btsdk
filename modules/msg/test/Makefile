DEBUG = y
PROGRAM = testmsg

CC = gcc
LD = gcc

SRCS = \
	./testmsg.c \
	../src/msg.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -I. -I../inc -I../../posix/inc
LDFLAGS =

ifeq ($(DEBUG),y)
CFLAGS += -DDEBUG=1 -g
endif

.PHONY: all clean

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) $(OBJS) -o $(PROGRAM) $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	rm -f $(OBJS) $(PROGRAM)
