#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLOCK 512
#define ID 8
#define MAX_FILES 10000
#define NAME 10
#define PATH 50

// credits to : https://www.youtube.com/watch?v=n2AAhiujAqs

/**
 * @brief info about the filesystem
 * number of Inodes
 * number of disk blocks
 * size of the disk blocks
 */
typedef struct superblock{
    int blokcs_size;
    int inodes_amount;
    int blocks_amount;
}s_block;

/**
 * @brief Place to store data
 */
typedef struct block{
    int next_block;
    char data[BLOCK];
}block;

/**
 * @brief Reference to a file
 * contains the OS info on a file
 * exist = Y if pointer to directory
 * exist = N if pointer to file
 */
typedef struct inode{
    int size;
    int first_block;
    char id[ID];
    char exist;
}i_node;

typedef struct mydirent{
    char name[NAME];
    int fd_num[20];
    int amount;
}directory;

struct myopenfile{
    int index;
    int fd;
};

/**
 * @brief write the file system
 */
void sync_fs(const char *);

/**
 * @brief read the file system
 * 
 */
void mount_fs(const char *);

/**
 * @brief finds an empty inode
 * return -1 if could not find
 */
int find_empty_inode();

/**
 * @brief finds an empty block
 * return -1 if could not find
 */
int find_empty_block();

/**
 * @brief seting the needed file size
 */
void set_file_size(int,int);

/**
 * @brief cut the file
 */
void shorten_file(int);

/**
 * @brief write single byte to needed file
 */
void write_byte(int, int, char);

/**
 * @brief read signel byte from needed file
 * @return char : the readed char || -1 on failure
 */
char read_byte(int, int);

/**
 * @brief Get the block number
 * 
 * @return int : the needed block index
 */
int get_block_number(int, int);

/**
 * @brief finds empty inode
 * calim it
 * find / claim a block
 * return the fd
 */
int allocate_file(int, const char *);

/**
 * @brief print out info about the filesystem from memory
 */
void print_fs();

/**
 * @brief create new file with given size
 */
void mymkfs(int);

/**
 * @brief reading/writing UFS
 * on given source it will read it
 * on given target it will write to it
 * 0 -> sucsses
 * -1 -> failure
 */
int mymount(const char *, const char *, const char *, unsigned long, const void *);

/**
 * @brief 
 * opening file from given path
 */
int myopen(const char *, int);

/**
 * @brief removing the given fd from myopenfile
 */
int myclose(int);

/**
 * @brief reads data from given fd
 */
ssize_t myread(int, void *, size_t);

/**
 * @brief writes data to given fd
 */
ssize_t mywrite(int, const void *, size_t);

/**
 * @brief increment file ptr by given offset
 * need to return new pointer
 */
int mylseek(int, int, int);

/**
 * @brief  finds the fd of given dir if exist
 * if not -> will create new one
 */
int myopendir(const char *);

/**
 * @brief finding the needed directory
 */
struct mydirent *myreaddir(int);

/**
 * @brief creating new directory at first Inode
 */
void create_dir();