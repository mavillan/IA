CC = g++
CFLAGS=-std=c++11 -O3 -Wall -c


all: main.o
	$(CC) main.o matcher.o assigner.o -o scheduler

main.o: matcher.o assigner.o
	$(CC) $(CFLAGS) main.cpp

matcher.o: matcher.cpp
	$(CC) $(CFLAGS) matcher.cpp

assigner.o: assigner.cpp
	$(CC) $(CFLAGS) assigner.cpp

clean:
	rm *.o