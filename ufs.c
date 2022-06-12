#include "ufs.h"
#include "color.h"

/**
 * Global variables
 */
s_block super;
block *block_arr;
struct inode *inode_arr;
struct myopenfile opened[MAX_FILES];
int SIZE; // holds the size of file descriptor

void mymkfs(int s)
{
    int update_size = s - (sizeof(s_block));
    int free_space = (update_size / 10);
    super.inodes_amount = (free_space) / (sizeof(i_node));
    super.blocks_amount = (update_size - free_space) / (sizeof(block));
    super.blokcs_size = sizeof(block);

    inode_arr = (i_node *)malloc(sizeof(i_node) * super.inodes_amount);
    for (int i = 0; i < super.inodes_amount; i++)
    {
        inode_arr[i].size = -1;
        inode_arr[i].first_block = -1;
        inode_arr[i].exist = 0;
        strcpy(inode_arr[i].id, "empty inode"); // initialize id to nothing
    }

    block_arr = (block *)malloc(sizeof(block) * super.blocks_amount);
    for (int i = 0; i < super.blocks_amount; i++)
    {
        block_arr[i].next_block = -1;
        bzero(block_arr[i].data, 0); // initialize data to nothing
    }

    int my_fd = allocate_file(sizeof(directory), "root");
    if (my_fd != 0)
    {
        printf("file already exist, can't create\n");
        exit(EXIT_FAILURE);
    }
    inode_arr[my_fd].exist = 1;
    directory *dir = (directory *)malloc(sizeof(directory));
    for (size_t i = 0; i < NAME; i++)
    {
        dir->fd_num[i] = -1;
    }
    strcpy(dir->name, "root");
    dir->amount = 0;
    char *root_dir = (char *)dir;
    // for(size_t i = 0 ; i < sizeof(directory) ; i++){
    //     write_byte(my_fd, i, &root_dir[i]);
    // }
    write_byte(my_fd, 0, root_dir);
    opened[my_fd].index = opened[my_fd].index + sizeof(root_dir);
    free(dir);
    welcome();
    printf("\n");
    GREEN;
    printf("Succsess in creating new directory\n");
    RESET;
}

void sync_fs(const char *ch)
{
    FILE *file;
    file = fopen(ch, "w+");
    if (file == NULL)
    {
        printf("error in opening file for syncing\n");
        exit(EXIT_FAILURE);
    }

    // superblock
    fwrite(&super, sizeof(s_block), 1, file);

    // inodes
    fwrite(inode_arr, sizeof(struct inode), super.inodes_amount, file);

    // blocks
    fwrite(block_arr, sizeof(struct block), super.blocks_amount, file);

    fclose(file);
}

void mount_fs(const char *ch)
{
    FILE *file;
    file = fopen(ch, "r");
    if (file == NULL)
    {
        printf("error in opening file\n");
        exit(EXIT_FAILURE);
    }

    // superblock
    fread(&super, sizeof(struct superblock), 1, file);

    inode_arr = (struct inode *)malloc(sizeof(struct inode) * super.inodes_amount);
    block_arr = (struct block *)malloc(sizeof(struct block) * super.blocks_amount);

    fread(inode_arr, sizeof(struct inode), super.inodes_amount, file);
    fread(block_arr, sizeof(struct block), super.blocks_amount, file);

    fclose(file);
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
    if (source == NULL && target == NULL)
    {
        printf("the source and the target are not provided\n");
        exit(EXIT_FAILURE);
    }
    // write to target
    if (target != NULL)
        sync_fs(target);
    // read the source
    else if (source != NULL)
        mount_fs(source);

    return 0;
}

void print_fs()
{
    RED;
    printf("Superblock info\n");
    printf("\tnum inodes %d\n", super.inodes_amount);
    printf("\tnum blocks %d\n", super.blocks_amount);
    printf("\tblock size %d\n", super.blokcs_size);

    BLUE;
    printf("Inodes\n");
    for (int i = 0; i < super.inodes_amount; i++)
    {
        printf("\tsize: %d block: %d name: %s\n", inode_arr[i].size, inode_arr[i].first_block, inode_arr[i].id);
    }
    YELLOW;
    printf("Blokcs\n");
    for (int i = 0; i < super.blocks_amount; i++)
    {
        printf("\tblock number: %d next block: %d\n", i, block_arr[i].next_block);
    }
    RESET;
}

int find_empty_inode()
{
    for (int i = 0; i < super.inodes_amount; i++)
    {
        if (inode_arr[i].first_block == -1)
            return i;
    }
    return -1; // on failure
}

int find_empty_block()
{
    for (int i = 0; i < super.blocks_amount; i++)
    {
        if (block_arr[i].next_block == -1)
            return i;
    }
    return -1; // on failure
}

int find_empty_fd()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (opened[i].inode == -1)
            return i;
    }
    return -1;
}

int allocate_file(int size, const char *target)
{
    if (strlen(target) >= ID)
    {
        printf("given target file name is longer then 8\n");
        exit(EXIT_FAILURE);
    }
    int Inode = find_empty_inode();
    if (Inode == -1)
    {
        printf("error in finding empty Inode\n");
        return -1;
    }
    int Block = find_empty_block();
    if (Block == -1)
    {
        printf("error in finding empty block\n");
        return -1;
    }

    inode_arr[Inode].size = size;
    inode_arr[Inode].first_block = Block;
    block_arr[Block].next_block = -2;
    strcpy(inode_arr[Inode].id, target);
    return Inode;
}

void set_file_size(int num, int size)
{
    int block_amount = size / BLOCK;
    int block_index = inode_arr[num].first_block;
    if (size % BLOCK != 0)
        block_amount = block_amount + 1;

    while (block_amount > 0)
    {
        int next = block_arr[block_index].next_block;
        if (next == -2)
        {
            int curr_block = find_empty_block();
            block_arr[block_index].next_block = curr_block;
            block_arr[curr_block].next_block = -2;
        }
        block_index = block_arr[block_index].next_block;
        block_amount--;
    }
    shorten_file(block_index);
    block_arr[block_index].next_block = -2;
}

void write_byte(int num, int target, char *data)
{
    printf("target %d\n", target);
    int curr = target / BLOCK;
    printf("curr %d\n", curr);
    int block_index = get_block_number(num, curr);
    printf("block index %d\n", block_index);
    int res = target % BLOCK;
    printf("res %d\n", res);
    for (int i = 0; i < strlen(data); i++)
    {
        block_arr[block_index].data[res + i] = data[i];
    }
}

int get_block_number(int num, int res)
{
    int offset = res;
    int block_index = inode_arr[num].first_block;
    while (offset > 0)
    {
        block_index = block_arr[block_index].next_block;
        offset--;
    }
    return block_index;
}

void shorten_file(int num)
{
    int next = block_arr[num].next_block;
    if (next >= 0)
        shorten_file(next);
    block_arr[num].next_block = -1;
}

size_t mywrite(int myfd, void *buf, size_t count)
{
    // if(inode_arr[myfd].exist == 1 || opened[myfd].fd != myfd){
    //     perror("error occures\n");
    //     return -1;
    // }
    char *curr = (char *)buf;
    printf("before\n");
    write_byte(myfd, opened[myfd].index, curr);
    opened[myfd].index = opened[myfd].index + count;
    printf("after\n");
    return opened[myfd].index;
}

size_t myread(int myfd, void *buf, size_t count)
{
    if (inode_arr[myfd].exist == 1)
    {
        perror("error ocures\n");
        return -1;
    }
    // char *s = (char *)malloc(count + 1); // set string to count size
    // for(size_t i = 0 ; i < count ; i++){
    //     s[i] = read_byte(myfd, opened[myfd].index);
    // }
    // printf("index of of%d\n", opened[myfd].index);
    // printf("count %d\n", count);
    read_byte(myfd, opened[myfd].index, count, (char *)buf);
    // s[count] = '\0';
    // strncpy((char *)buf, s, count);
    // free(s);
    return opened[myfd].index;
}

char read_byte(int fd, int target, size_t count, char *buf)
{
    bzero(buf, 0);
    int next = inode_arr[fd].first_block;
    int block_num = count / BLOCK;
    printf("block arr data%s\n", block_arr[next].data);
    int j = 0;
    do
    {
        while (target <= BLOCK && j != count)
        {
            // target ++;
            buf[j] = block_arr[next].data[target];
            j++, target++;
            // next = block_arr[next].next_block;
            // if(next == -1 || next == -2){
            //     printf("error in reading\n");
            //     return -1;
            // }
        }
        if (j == count)
            break;
        next = block_arr[next].next_block;
        block_num--;
        target = 0;
    } while (block_num < 0);
    printf("buff %s\n", buf);
    return block_arr[next].data[target];
    // return buf;
}

struct mydirent *myreaddir(int target)
{
    if (inode_arr[target].exist != 1)
    {
        printf("error occures\n");
        return NULL;
    }
    directory *dir = (directory *)block_arr[inode_arr[target].first_block].data;
    return dir;
}

int mylseek(int myfd, int offset, int whence)
{
    // if((offset == 0) || (opened[myfd].fd != myfd)){
    //     printf("wrong usage of mysleek func, enter positive or negative offset\n");
    //     exit(EXIT_FAILURE);
    // }
    if (whence < 0 || whence > 2)
    {
        printf("whence should be 0 | 1 | 2\n");
        exit(EXIT_FAILURE);
    }
    switch (whence)
    {
    case (SEEK_SET):
        opened[myfd].index = offset;
        break;
    case (SEEK_END):
        opened[myfd].index = inode_arr[myfd].size + offset;
        break;
    case (SEEK_CUR):
        opened[myfd].index = opened[myfd].index + offset;
        break;
    }

    if (opened[myfd].index < 0)
        opened[myfd].index = 0;

    return opened[myfd].index;
}

int myopen(const char *pathname, int flags)
{
    if (strlen(pathname) <= 0)
    {
        perror("please give a valid path name\n");
        exit(EXIT_FAILURE);
    }
    char name[NAME];
    char *token;
    strcpy(name, pathname);
    const char buf[2] = "/";
    token = strtok(name, buf);
    char first[PATH] = "";
    char second[PATH] = "";
    while (token != NULL)
    {
        strcpy(second, first);
        strcpy(first, token);
        token = strtok(NULL, buf);
    }
    printf("%s\n", first);
    for (size_t i = 0; i < super.inodes_amount; i++)
    {
        if (!strcmp(inode_arr[i].id, first))
        {
            if (inode_arr[i].exist != 0)
            {
                perror("error in finding empty inode\n");
            }
            opened[i].index = 0;
            opened[i].fd = i;
            return i;
        }
    }
    int fd1 = allocate_file(1, first);
    int fd2 = myopendir(second);
    directory *dir = myreaddir(fd2);
    dir->fd_num[dir->amount++] = fd1;
    opened[fd1].index = 0;
    opened[fd1].fd = fd1;
    printf("fd index %d\n", opened[fd1].index);
    return fd1;
}

// int myopen(const char *pathname, int flags){
//     if(strlen(pathname) <= 0){
//         printf("please give good path name\n");
//         exit(EXIT_FAILURE);
//     }

//     if(flags == O_CREAT){
//         int empty_fd = find_empty_fd();
//         if(empty_fd == -1){
//             printf("couldn't find empty fd\n");
//             return -1;
//         }
//         int inode = allocate_file(1, pathname);
//         opened[empty_fd].index = 0;
//         opened[empty_fd].inode = inode;
//         return empty_fd;
//     }

//     else if(flags == O_RDWR || flags == O_WRONLY || flags == O_RDONLY){
//         int inode_index = -1;
//         for(int i = 0 ; i < super.inodes_amount ; i++){
//             if(!strcmp(inode_arr[i].id, pathname))
//                 inode_index = i;
//         }
//         if(inode_index == -1){
//             printf("error in finding the needed file\n");
//             return -1;
//         }
//         else{
//             int curr = -1;
//             for(int i = 0 ; i < SIZE ; i++){
//                 if(opened[i].inode == inode_index)
//                     curr = i;
//             }
//             if(curr == -1){
//                 printf("error in finding the needed inode\n");
//                 return -1;
//             }
//             else{
//                 if(flags == O_WRONLY)
//                     opened[curr].fd = 0;
//                 if(flags == O_RDONLY)
//                     opened[curr].fd = 1;
//                 if(flags == O_RDWR)
//                     opened[curr].fd = 2;
//                 return curr;
//             }
//         }
//     }
//     else{
//         printf("some error occured in opening file\n");
//         return -1;
//     }
// }

int myclose(int fd)
{
    // opened[fd].index = -1;
    opened[fd].fd = -1;
    return 0;
}

int myclosedir(int fd)
{
    printf("Closing directory\n");
    return -1;
}

int myopendir(const char *target)
{
    char path[PATH];
    char *token;
    strcpy(path, target);
    const char temp[2] = "/";
    token = strtok(path, temp);
    char first[NAME] = "";
    char second[NAME] = "";

    while (token != NULL)
    {
        strcpy(second, first);
        strcpy(first, token);
        token = strtok(NULL, temp);
    }

    for (size_t i = 0; i < super.inodes_amount; i++)
    {
        if (!strcmp(inode_arr[i].id, first))
        {
            if (inode_arr[i].exist != 1)
            {
                printf("this is directory and not a file\n");
                return -1;
            }
            return i;
        }
    }
    int fd = myopendir(second);
    if (fd == -1)
    {
        printf("error in opening directory with last path\n");
        return -1;
    }
    if (inode_arr[fd].exist != 1)
    {
        printf("this is directory not a file\n");
        return -1;
    }
    int b = inode_arr[fd].first_block;
    directory *this_dir = (directory *)block_arr[b].data;
    int new_fd = allocate_file(sizeof(directory), first);
    this_dir->fd_num[this_dir->amount++] = new_fd; // **
    inode_arr[new_fd].exist = 1;
    directory *next_dir = (directory *)malloc(sizeof(directory));
    next_dir->amount = 0;
    int j;
    while (j < NAME)
    {
        next_dir->fd_num[j] = -1;
        j++;
    }
    char *new_dir = (char *)next_dir;
    write_byte(new_fd, 0, new_dir);
    opened[fd].index = opened[fd].index + sizeof(directory);
    strcpy(next_dir->name, target);
    return new_fd;
}

int main()
{
    mymkfs(MAX_FILES);
    int i = 1;
    int first = myopendir("root/artem");
    int second = myopendir("root/barak");
    // print_fs();
    BLUE;
    printf("___________ -CASE 1- ___________\n");
    if (first == second)
    {
        RED;
        printf("Test number %d failed\n", i);
        RESET;
    }
    else
    {
        GREEN;
        printf("Test number %d passed\n", i);
        RESET;
    }
    i++;

    int first_fd = myopen("root/artem/first", 0);
    int second_fd = myopen("root/artem/first", 0);
    int third_fd = myopen("root/barak/second", 0);
    printf("first fd %d\n", opened[first_fd].index);
    // print_fs();
    if (first_fd == second_fd)
    {
        GREEN;
        printf("Test number %d passed\n", i);
        RESET;
    }
    else
    {
        RED;
        printf("Test number %d failed\n", i);
        RESET;
    }
    i++;

    BLUE;
    printf("___________ -CASE 2- ___________\n");
    first_fd = myclose(first_fd);
    if (first_fd == second_fd)
    {
        RED;
        printf("Test number %d failed\n", i);
        RESET;
    }
    else
    {
        GREEN;
        printf("Test number %d passed\n", i);
        RESET;
    }
    printf("second fd after %d\n", opened[second_fd].index);
    BLUE;
    printf("___________ -CASE 3- ___________\n");
    // print_fs();
    char *data = "This is test";
    int a = mywrite(second_fd, data, 15);
    char buf[15];
    i++;
    printf("second fd after before %d\n", opened[second_fd].index);
    mylseek(second_fd, -15, SEEK_CUR);
    printf("second fd after after %d\n", opened[second_fd].index);
    myread(second_fd, buf, 15);
    printf("%s\n", buf);
    if (strcmp(buf, "This is test") == 0)
    {
        GREEN;
        printf("Test number %d passed\n", i);
        RESET;
    }
    else
    {
        RED;
        printf("Test number %d failed\n", i);
        RESET;
    }

    i++;
    char *data1 = "barak";
    char new_buf[5];
    
    int b = mywrite(third_fd, data1, 5);
    puts(block_arr[inode_arr[third_fd].first_block].data);
    printf("third before %d\n", opened[third_fd].index);
    mylseek(third_fd, 2 , SEEK_SET);
    printf("third after %d\n", opened[third_fd].index);
    myread(third_fd, new_buf, 20);
    puts(new_buf);
    if(strcmp(buf, new_buf) == 0){
        RED;
        printf("Test number %d failed\n", i);
        RESET;
    }
    else{
        GREEN;
        printf("Test number %d passed\n", i);
    }

    return 0;
}