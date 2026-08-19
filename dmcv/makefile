CC = gcc
CFLAGS = -Wall -Wextra -std=c11

processflow: main.o
	$(CC) $(CFLAGS) -o processflow main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o processflow

.PHONY: clean