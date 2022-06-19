CC=gcc
PIC = -fPIC -g -w
FLAGS = -Wall -g
all: main
	./main

main: main.o libufs.so libmylibc.so
	$(CC) $(FLAGS) main.o ./libufs.so ./libmylibc.so -lm -o main


libufs.so: ufs.c ufs.h
	$(CC) $(FLAGS) --shared -fPIC ufs.c -o libufs.so

libmylibc.so: mylibc.c mylibc.h
	$(CC) $(FLAGS) --shared -fPIC mylibc.c -o libmylibc.so

main.o: ufs.h main.c
	$(CC) -c main.c

ufs:ufs.o ufs.h
	gcc -o ufs ufs.o

ufs.o: ufs.c ufs.h
	$(CXX) -c ufs.c

valgrind: ufs
	valgrind  --leak-check=full --show-leak-kinds=all ./main
clean:
	rm -f *.o main  ufs  unix_file_system libmylibc.so libufs.so