OBJS	= main.o sha256.o sha256_utils.o
SOURCE	= main.c sha256.c sha256_utils.c
HEADER	= sos_defines.h sha256.h sha256_utils.h
OUT	= main
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

sha256.o: sha256.c
	$(CC) $(FLAGS) sha256.c 

sha256_utils.o: sha256_utils.c
	$(CC) $(FLAGS) sha256_utils.c 


clean:
	rm -f $(OBJS)

install: all
install: clean
