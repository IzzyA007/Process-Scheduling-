CC=g++
CFLAGS=-std=c++11

all: assignment_5

assignment_5: assignment_5.o
	$(CC) $(CFLAGS) -o assignment_5 assignment_5.o

assignment_5.o: assignment_5.cpp
	$(CC) $(CFLAGS) -c assignment_5.cpp

clean:
	rm -rf *.o assignment_5

