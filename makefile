CC=gcc -fPIC -g -w

all: main
	./main

main: main.o ufs.o
	$(CXX) -o main main.o ufs.o

main.o: main.c ufs.h
	$(CXX) -c main.c

ufs.o: ufs.c ufs.h
	$(CXX) -c ufs.c

clean:
	rm -f *.o main