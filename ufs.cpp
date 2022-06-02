#include "ufs.hpp"

/**
 * Global variables
 */
struct superblock super;
struct block *block_arr;
struct inode *inode_arr;

void create_fs(){
    super.blocks_amount = 50;
    super.inodes_amount = 10;
    super.blokcs_size = sizeof(struct block);
    
    inode_arr = (struct inode *)malloc(sizeof(struct inode) * super.inodes_amount);
    for(int i = 0 ; i < super.inodes_amount ; i++){
        inode_arr[i].size = -1; // if not allocated
        inode_arr[i].first_block = -1;
        strcpy(inode_arr[i].id, "empty"); // initialize id to nothing
    }
    block_arr = (struct block *)malloc(sizeof(struct block) * super.blocks_amount);
    for(int i = 0 ; i < super.blocks_amount ; i++){
        block_arr[i].next_block = -1;
    }
}

void mount_fs(){
    FILE *file;
    file = fopen("data", "r");  //reat

    // superblock
    fread(&super, sizeof(super), 1, file);

    // inodes
    for(int i = 0 ; i < super.inodes_amount ; i++){
        fread(&(inode_arr[i]), sizeof(struct inode), 1, file);
    }

    // blokcs
    for(int i = 0 ; i < super.blocks_amount ; i++){
        fread(&(block_arr[i]), sizeof(struct block), 1, file);
    }
}

void sync_fs(){
    FILE *file;
    file = fopen("data", "w+"); //write

    // superblock
    fwrite(&super, sizeof(super), 1, file);

    // inodes
    for(int i = 0 ; i < super.inodes_amount ; i++){
        fwrite(&(inode_arr[i]), sizeof(struct inode), 1, file);
    }

    //blocks
    for(int i = 0 ; i < super.blocks_amount ; i++){
        fwrite(&(block_arr[i]), sizeof(struct block), 1, file);
    }

    fclose(file);
}

void print_fs(){
    printf("Superblock info\n");
    printf("\tnum inodes %d\n", super.inodes_amount);
    printf("\tnum blocks %d\n", super.blocks_amount);
    printf("\tblock size %d\n", super.blokcs_size);

    printf("Inodes\n");
    for(int i = 0 ; i < super.inodes_amount ; i++){
        printf("\tsize: %d block: %d name: %s\n", inode_arr[i].size, inode_arr[i].first_block , inode_arr[i].id);
    }

    printf("Blokcs\n");
    for(int i = 0 ; i < super.blocks_amount ; i++){
        printf("\tblock number: %d next block: %d\n", i, block_arr[i].next_block);
    }
}

int main(){
    create_fs();
    sync_fs();
    print_fs();
    printf("done\n");
}