#include "ufs.hpp"

/**
 * Global variables
 */
s_block super;
block *block_arr;
struct inode *inode_arr;
struct myopenfile opened[MAX_FILES];

void mymkfs(int s){
    int update_size = s - (sizeof(s_block));
    int free_space = (update_size/10);
    super.inodes_amount = (free_space)/(sizeof(i_node));
    super.blocks_amount = (update_size - free_space)/(sizeof(block));
    super.blokcs_size = sizeof(block);
    
    inode_arr = (i_node *)malloc(sizeof(i_node) * super.inodes_amount);
    for(int i = 0 ; i < super.inodes_amount ; i++){
        inode_arr[i].size = -1; // if not allocated
        inode_arr[i].first_block = -1;
        strcpy(inode_arr[i].id, "empty"); // initialize id to nothing
    }

    block_arr = (block *)malloc(sizeof(block) * super.blocks_amount);
    for(int i = 0 ; i < super.blocks_amount ; i++){
        block_arr[i].next_block = -1;
        strcpy(block_arr[i].data, "empty"); // initialize data to nothing
    }
}

void sync_fs(const char *ch){
    FILE *file;
    file = fopen(ch, "w+");

    // superblock
    fwrite(&super, sizeof(s_block), 1, file);

    // inodes
    for(int i = 0 ; i < super.inodes_amount ; i++){
        fwrite(&(inode_arr[i]), sizeof(i_node), 1, file);
    }

    //blocks
    for(int i = 0 ; i < super.blocks_amount ; i++){
        fwrite(&(block_arr[i]), sizeof(block), 1, file);
    }

    fclose(file);
}

void mount_fs(const char *ch){
    FILE *file;
    file = fopen(ch, "r"); 

    // superblock
    fread(&super, sizeof(struct superblock), 1, file);

    inode_arr = (struct inode *)malloc(sizeof(struct inode) * super.inodes_amount);
    block_arr = (struct block *)malloc(sizeof(struct block) * super.blocks_amount);

    fread(inode_arr, sizeof(struct inode), super.inodes_amount, file);    
    fread(block_arr, sizeof(struct block), super.blocks_amount, file);

    fclose(file);
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data){
    // write to target
    if(target != NULL)
        sync_fs(target);
    // read the source
    else if(source != NULL)
        mount_fs(source);
    
    return 0;
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

int find_empty_inode(){
    for(int i = 0 ; i < super.inodes_amount ; i++){
        if(inode_arr[i].first_block == -1)
            return i;
    }
    return -1;  // on failure
}

int find_empty_block(){
    for(int i = 0 ; i < super.blocks_amount ; i++){
        if(block_arr[i].next_block == -1)
            return i;
    }
    return -1;  // on failure
}

int allocate_file(int n, const char *target){
    if(strlen(target) >= ID){
        printf("given target file name is longer then 8\n");
        return NULL;
    }
    int Inode = find_empty_inode();
    int Block = find_empty_block();
    if((Inode == -1) || (Block == -1)){
        printf("there is no empty inodes or empty blocks\n");
        return NULL;
    }

    inode_arr[Inode].first_block = Block;
    inode_arr[Inode].size = n;
    strcpy(inode_arr[Inode].id, target);
    return Inode;
}

void make_dir(){
    directory *new_dir = (directory *)malloc(sizeof(directory));
    int check = allocate_file(sizeof(directory), "new_dir");
    if(check == 0){
        inode_arr[check].exist = 'Y';
        new_dir->amount = 0;
        // **** maybe need to update the name ****
        free(new_dir);
    }

    else{
        printf("can't create new directory\n");
        return;
    }
}

// need to complete
ssize_t mywrite(int myfd, const void *buf, size_t count){
    char *res = (char *)buf;
    if(inode_arr[myfd].exist == 'Y' || opened[myfd].fd != myfd){
        printf("error occures\n");
        return NULL;
    }
}

struct mydirent *myreaddir(int target){
    if(inode_arr[target].exist != 'Y'){
        printf("error occures\n");
        return NULL;
    }
    directory *dir = (directory *)block_arr[inode_arr[target].first_block].data;
    return dir;
}

int mylseek(int myfd, int offset, int whence){
    if((offset == 0) || (opened[myfd].fd != myfd)){
        printf("wrong usage of mysleek func, enter positive or negative offset\n");
        return NULL;
    }
    // offset != 0
    opened[myfd].index = opened[myfd].index + offset;
    return opened[myfd].index;
}

int main(){
    mymkfs(10);
    printf("done\n");
}