DEBUG = y
PROGRAM = sim

CC = gcc
LD = gcc

ROOT = ../..

SRCS = \
	./main.c \
	$(ROOT)/system/src/system.c \
	$(ROOT)/system/src/task_handlers.c \
	$(ROOT)/bluetooth/src/bluetooth.c \
	$(ROOT)/bluetooth/src/gap-const.c \
	$(ROOT)/bluetooth/src/gap.c \
	$(ROOT)/bluetooth/src/l2cap.c \
	$(ROOT)/bluetooth/src/gatt.c \
	$(ROOT)/bluetooth/src/rfcomm.c \
	$(ROOT)/bluetooth/src/sdp.c \
	$(ROOT)/bluetooth/src/spp_sdp.c \
	$(ROOT)/bluetooth/src/spp.c \
	$(ROOT)/bluetooth/src/hci-usb.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -I. \
	-I$(ROOT)/system/inc \
	-I$(ROOT)/bluetooth/inc

LDFLAGS =

ifeq ($(DEBUG),y)
CFLAGS += -DDEBUG=1 -g
endif

# libusb
CFLAGS += `pkg-config libusb-1.0 --cflags`
LDFLAGS += `pkg-config libusb-1.0 --libs`

.PHONY: all clean

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) $(OBJS) -o $(PROGRAM) $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	rm -f $(OBJS) $(PROGRAM)
