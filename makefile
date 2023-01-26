OBJS	= MAIN/View.o FAT/Fat.o EXT/Ext2.o UTILS/Utilities.o
SOURCE	= MAIN/View.c FAT/Fat.c EXT/Ext2.c UTILS/Utilities.c
HEADER	= MAIN/View.h FAT/Fat.h EXT/Ext2.h UTILS/Utilities.h UTILS/Error.h
OUT	= shooter
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra 
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

View.o: MAIN/View.c
	$(CC) $(FLAGS) MAIN/View.c 

Fat.o: FAT/Fat.c
	$(CC) $(FLAGS) FAT/Fat.c 

Ext2.o: EXT/Ext2.c
	$(CC) $(FLAGS) EXT/Ext2.c 

Utilities.o: UTILS/Utilities.c
	$(CC) $(FLAGS) UTILS/Utilities.c 

clean:
	rm -f $(OBJS) $(OUT)