CC = gcc
CFLAGS = -g -Wall 

UTIL_DIR = ./util
BITMAP_DIR = ./bitmap

OBJS = main.o $(UTIL_DIR)/imgp_util.o  $(BITMAP_DIR)/imgp_bitmap.o

all: app 

app: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@