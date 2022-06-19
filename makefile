CC=gcc
PIC = -fPIC -g -w
# FLAGS = -Wall -g
all: ufs_test mylibc_test
	./ufs_test
	./mylibc_test

ufs_test: ufs_test.o ufs.o
	$(CC) -o ufs_test ufs_test.o ufs.o

ufs_test.o: ufs_test.c ufs.h color.h
	$(CC) $(FLAGS) -c ufs_test.c

ufs.o: ufs.c ufs.h color.h
	$(CC) $(FLAGS) -c ufs.c

mylibc_test: mylibc_test.o mylibc.o ufs.o
	$(CC) $(FLAGS) -o mylibc_test mylibc_test.o mylibc.o ufs.o

mylibc_test.o: mylibc_test.c mylibc.h color.h
	$(CC) $(FLAGS) -c mylibc_test.c

mylibc.o: mylibc.c mylibc.h color.h
	$(CC) $(FLAGS) -c mylibc.c


# main: main.o libufs.so libmylibc.so
# 	$(CC) $(FLAGS) main.o ./libufs.so ./libmylibc.so -lm -o main


# libufs.so: ufs.c ufs.h
# 	$(CC) $(FLAGS) --shared -fPIC ufs.c -o libufs.so

# libmylibc.so: mylibc.c mylibc.h
# 	$(CC) $(FLAGS) --shared -fPIC mylibc.c -o libmylibc.so

# main.o: ufs.h mylibc.h main.c
# 	$(CC) -c main.c

# ufs:ufs.o ufs.h
# 	$(CC) -o ufs ufs.o

# ufs.o: ufs.c ufs.h
# 	$(CC) -c ufs.c

# mylibc: mylibc.o mylibc.h
# 	$(CC) -o mylibc mylibc.o

# mylibc.o: mylibc.c mylibc.h
# 	$(CC) -c mylibc.c

# valgrind: ufs
# 	valgrind  --leak-check=full --show-leak-kinds=all ./main
clean:
	rm -f *.o data.txt ufs ufs_test mylibc_test  unix_file_system libmylibc.so libufs.so main