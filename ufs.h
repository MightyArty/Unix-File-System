#ifndef __UFS__H__
#define __UFS__H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "color.h"
#include <sys/mount.h>
#include <regex.h>

#define BLOCK_SIZE 512
#define MAX_NAME 128

#define ID 8
#define MAX_FILES 10000

#define NAME 12
#define PATH 50

#define USED -2
#define FREE -1

#define IS_DIR 1
#define IS_FILE 0


/* The possibilities for the third argument to `fseek'.
   These values should not be changed.  */
#define SEEK_SET 0 /* Seek from beginning of file.  */
#define SEEK_CUR 1 /* Seek from current position.  */
#define SEEK_END 2 /* Seek from end of file.  */

// #define O_RDONLY 00
// #define O_WRONLY 01
// #define O_RDWR 02
#ifndef O_CREAT
#define O_CREAT 0100 /* Not fcntl.  */
#endif
#ifndef O_APPEND
#define O_APPEND 02000
#endif
// credits to : https://www.youtube.com/watch?v=n2AAhiujAqs

/**
 * @brief info about the filesystem
 * number of Inodes
 * number of disk blocks
 * size of the disk blocks
 */
typedef struct superblock
{
    int blokcs_size;
    int inodes_amount;
    int blocks_amount;
} s_block;

/**
 * @brief Place to store data
 */
typedef struct block
{ 
    int next_block;
    char data[BLOCK_SIZE];
    int free;
} block;

/**
 * @brief Reference to a file
 * contains the OS info on a file
 * isDir = 1 if pointer to directory
 * isDir = 0 if pointer to file
 *  isDir = -1 if pointer to null
 */
typedef struct inode
{
    int first_block;
    char name[MAX_NAME];
    int size_file;
    int block_count;
    int isDir;
    int free;
} i_node;

typedef struct mydirent
{
    int fd_inode;               /* Inode number */
    unsigned char d_type;    /* Type of file; not supported
                                by all filesystem types */
    char d_name[MAX_NAME];   /* Null-terminated filename */
} mydirent;
typedef struct myDIR
{
    int amount;
    int index;
    mydirent * dirent;
} myDIR;

struct myopenfile
{
    int access;
    int lseek_index;
    int inode;
};

// /**
//  * Global variables
//  */
// struct superblock super;
// struct block *block_arr = NULL;
// struct inode *inode_arr = NULL;
// struct myopenfile opened[MAX_FILES];
// int SIZE; // holds the size of file descriptor
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
 * @brief check if there is any empty fd in the opened files
 * @return int: the current index of the empty fd
 * -1 on failure (couldn't find any empty fd)
 */
int find_empty_fd();

/**
 * @brief seting the needed file size
 */
void set_file_size(int, int);

/**
 * @brief cut the file
 */
void shorten_file(int);

/**
 * @brief read signel byte from needed file
 * @return char : the readed char || -1 on failure
 */
void read_byte(int, int, size_t, char *);

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
int allocate_inode(const char *, int);

/**
 * @brief print out info about the filesystem from memory
 */
void print_fs();

/**
 * @brief create new ufs with given size
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
 * possible values of flags :
 * - O_RDWR: read and write
 * - O_WRONLY: write
 * - O_RDONLY: read
 * - O_CREAT: create new file in the given path
 */
int myopen(const char *, int);

/**
 * @brief removing the given fd from myopenfile
 */
int myclose(int);

/**
 * @brief closing directory
 * @return int
 */
int myclosedir(myDIR *);

/**
 * @brief reads data from given file into buf
 * return index of pointer at the file
 */
size_t myread(int, void *, size_t);

/**
 * @brief writes data to given fd
 */
size_t mywrite(int,const void *, size_t);

/**
 * @brief increment file ptr by given offset
 * need to return new pointer
 */
int mylseek(int, off_t, int);

/**
 * @brief  finds the fd of given dir if exist
 * if not -> will create new one
 */
myDIR * myopendir(const char *);

/**
 * @brief finding the needed directory
 */
struct mydirent *myreaddir(struct myDIR*);

/**
 * @brief creating new directory at first Inode
 */
void create_dir();

/**
 * @brief welcoming function
 * just for fun :)
 */
// void welcome()
// {
//     printf("\033[1;31m    $$      $$$$$  $$$$$$$$$ $     $         \033[1;34m $$$$$         $$     $$$$$       $$      $  $    \n");
//     printf("\033[1;31m   $  $     $   $      $      $   $          \033[1;34m $   $        $  $    $   $      $  $     $ $    \n");
//     printf("\033[1;31m  $ -- $    $$$$$      $        $    \033[1;33m @@@@@@ \033[1;34m $$$$$$$     $ -- $   $$$$$     $ -- $    $$        \n");
//     printf("\033[1;31m $      $   $    $     $        $            \033[1;34m $     $    $      $  $    $   $      $   $ $         \n");
//     printf("\033[1;31m$        $  $     $    $        $            \033[1;34m $$$$$$$   $        $ $     $ $        $  $  $       \n");
// }

#endif //!__UFS__H__