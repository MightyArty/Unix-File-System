#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLOCK 512
#define ID 8

// credits to : https://www.youtube.com/watch?v=n2AAhiujAqs

/**
 * @brief info about the filesystem
 * number of Inodes
 * number of disk blocks
 * size of the disk blocks
 */
struct superblock{
    int blokcs_size;
    int inodes_amount;
    int blocks_amount;
};

/**
 * @brief Place to store data
 */
struct block{
    int next_block;
    char data[BLOCK];
};

/**
 * @brief Reference to a file
 * contains the OS info on a file
 */
struct inode{
    int size;
    int first_block;
    char id[ID];
};

/**
 * @brief write the file system
 */
void sync_fs();

/**
 * @brief load a file system
 */
void mount_fs();

/**
 * @brief initialize new filesystem
 */
void create_fs();

/**
 * @brief print out info about the filesystem
 */
void print_fs();