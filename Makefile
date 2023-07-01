CC = gcc
CFLAGS = -g 
LD_FLAGS = -lm

UTIL_DIR = ./util
BITMAP_DIR = ./bitmap

OBJS = main.o $(UTIL_DIR)/imgp_util.o  $(BITMAP_DIR)/imgp_bitmap.o

all: app 

app: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LD_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@