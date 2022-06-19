#include "ufs.h"

/**
 * Global variables
 */
struct superblock super;
struct block *block_arr = NULL;
struct inode *inode_arr = NULL;
struct myopenfile opened[MAX_FILES];
int SIZE; // holds the size of file descriptor

void welcome()
{
    printf("\033[1;31m    $$      $$$$$  $$$$$$$$$ $     $         \033[1;34m $$$$$         $$     $$$$$       $$      $  $    \n");
    printf("\033[1;31m   $  $     $   $      $      $   $          \033[1;34m $   $        $  $    $   $      $  $     $ $    \n");
    printf("\033[1;31m  $ -- $    $$$$$      $        $    \033[1;33m @@@@@@ \033[1;34m $$$$$$$     $ -- $   $$$$$     $ -- $    $$        \n");
    printf("\033[1;31m $      $   $    $     $        $            \033[1;34m $     $    $      $  $    $   $      $   $ $         \n");
    printf("\033[1;31m$        $  $     $    $        $            \033[1;34m $$$$$$$   $        $ $     $ $        $  $  $       \n");
}

void mymkfs(int s)
{
    int update_size = s - (sizeof(s_block));
    int free_space = (update_size / 100) * 10;

    super.inodes_amount = (free_space) / (sizeof(i_node));
    super.blocks_amount = (update_size - free_space) / (sizeof(block));
    super.blokcs_size = sizeof(block);

    inode_arr = (i_node *)malloc(sizeof(i_node) * super.inodes_amount);
    if (inode_arr == NULL)
    {
        puts("err malloc inode arr");
        exit(1);
    }
    for (int i = 0; i < super.inodes_amount; i++)
    {
        inode_arr[i].first_block = FREE;
        inode_arr[i].size_file = 0;
        inode_arr[i].isDir = -1;
        inode_arr[i].block_count = 0;
        inode_arr[i].free = 1;
        strcpy(inode_arr[i].name, "empty"); // initialize id to nothing
    }

    block_arr = (block *)malloc(sizeof(block) * super.blocks_amount);
    if (block_arr == NULL)
    {
        puts("err malloc block arr");
        exit(1);
    }
    for (int i = 0; i < super.blocks_amount; i++)
    {
        block_arr[i].next_block = -1;
        block_arr[i].free = 1;
        bzero(block_arr[i].data, '\0'); // initialize data to nothing
    }

    for (int i = 0; i < MAX_FILES; i++)
    {
        opened[i].access = -1;
        opened[i].inode = -1;
        opened[i].lseek_index = -1;
    }

    allocate_inode("./", IS_DIR);

    sync_fs("unix_file_system");
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

    fwrite(inode_arr, sizeof(*inode_arr), super.inodes_amount, file);

    fwrite(block_arr, sizeof(*block_arr), super.blocks_amount, file);

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
    fread(&super, sizeof(s_block), 1, file);

    // inodes
    if (inode_arr != NULL)
        free(inode_arr);
    inode_arr = (i_node *)malloc(sizeof(i_node) * super.inodes_amount);
    fread(inode_arr, sizeof(*inode_arr), super.inodes_amount, file);

    // blocks
    if (block_arr != NULL)
        free(block_arr);
    block_arr = (block *)malloc(sizeof(block) * super.blocks_amount);
    fread(block_arr, sizeof(*block_arr), super.blocks_amount, file);

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
        mount_fs(target);
    // read the source
    else if (source != NULL)
        sync_fs(source);

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
        printf("\tsize: %d block: %d name: %s\n", inode_arr[i].size_file, inode_arr[i].first_block, inode_arr[i].name);
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
        if (inode_arr[i].free == 1)
            return i;
    }
    return -1; // on failure
}

int find_empty_block()
{
    for (int i = 0; i < super.blocks_amount; i++)
    {
        if (block_arr[i].free == 1)
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

int allocate_inode(const char *target, int isDir)
{
    if (strlen(target) >= MAX_NAME)
    {
        printf("given target file name is longer then 8\n");
        exit(EXIT_FAILURE);
    }
    int Inode = find_empty_inode();
    int block = find_empty_block();
    if (Inode == -1 || block == -1)
    {
        printf("error in finding empty Inode\n");
        return -1;
    }
    inode_arr[Inode].first_block = block;
    inode_arr[Inode].block_count = 1;
    inode_arr[Inode].isDir = isDir;
    strcpy(inode_arr[Inode].name, target);
    inode_arr[Inode].free = 0;
    return Inode;
}

void set_file_size(int fd_block, int size)
{
    int block_amount = size / BLOCK_SIZE;
    int block_index = inode_arr[fd_block].first_block;
    // if (block_index == -1)
    //     block_index = find_empty_block();

    if (size % BLOCK_SIZE != 0)
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

int myclose(int fd)
{
    if (fd >= MAX_FILES)
    {
        printf("my mount didn't happen yet\n");
        return -1;
    }
    opened[fd].lseek_index = -1;
    opened[fd].inode = -1;
    opened[fd].access = -1;
    return 0;
}

int mylseek(int myfd, off_t offset, int whence)
{
    if ((opened[myfd].access & O_APPEND) == O_APPEND)
    {
        printf("wrong usage of mysleek func, enter positive or negative offset\n");
        exit(EXIT_FAILURE);
    }

    switch (whence)
    {
    case (SEEK_SET):
        opened[myfd].lseek_index = offset;
        break;
    case (SEEK_END):
        opened[myfd].lseek_index = inode_arr[myfd].size_file + offset;
        break;
    case (SEEK_CUR):
        opened[myfd].lseek_index += (int)offset;
        break;
    }

    if (opened[myfd].lseek_index < 0)
        opened[myfd].lseek_index = 0;

    return opened[myfd].lseek_index;
}

int myclosedir(myDIR *dirp)
{
    free((*dirp).dirent);
    free(dirp);
    printf("Closing directory\n");
    return 0;
}

int findPath(const char *path)
{
    for (int i = 0; i < super.inodes_amount; ++i)
    {
        if (!strcmp(inode_arr[i].name, path))
        {
            return i;
        }
    }
    return -1;
}

int myopen(const char *pathname, int flags)
{

    if (strlen(pathname) <= 0)
    {
        perror("please give a valid path name\n");
        exit(EXIT_FAILURE);
    }
    int inode = findPath(pathname);

    if (inode == -1 && flags & O_CREAT)
        inode = allocate_inode(pathname, IS_FILE);

    int newfd = find_empty_fd();
    if (newfd == -1)
    {
        return -1;
    }
    if ((flags & O_APPEND) == O_APPEND)
    {
        opened[newfd].lseek_index = inode_arr[inode].size_file;
    }
    else
    {
        opened[newfd].lseek_index = 0;
    }

    opened[newfd].inode = inode;
    opened[newfd].access = flags;
    return newfd;
}

struct mydirent *myreaddir(struct myDIR *dirp)
{
    if (dirp->amount != dirp->index)
    {

        mydirent *dirent = &(dirp->dirent[dirp->index]);
        dirp->index += 1;
        if (dirent->fd_inode == -3)
            return NULL;
        return dirent;
    }
    return NULL;
}

myDIR *myopendir(const char *target)
{
    int count = 0;
    myDIR *ans = (myDIR *)malloc(sizeof(myDIR));
    ans->dirent = (mydirent *)malloc(sizeof(struct mydirent) * MAX_FILES / 2);
    int s = 0;
    for (int i = 0; i < super.inodes_amount; i++)
    {
        char *name = inode_arr[i].name;
        int j = (int)strlen(name) - 1;
        while (j > 0)
        {
            s++;
            if (name[j] == '/')
            {
                break;
            }
            j--;
        }
        if (strcmp(inode_arr[i].name, "empty") == 0)
            continue;
        char name_[256];
        strncpy(name_, name, (int)(strlen(name) - s));
        s = 0;
        if (strcmp(target, name_) == 0)
        {
            strcpy(ans->dirent[count].d_name, name + strlen(target) + 1);
            ans->dirent[count].fd_inode = i;
            ans->dirent[count].d_type = inode_arr[i].isDir;
            count++;
        }
    }
    ans->amount = count;
    ans->index = 0;
    count++;
    ans->dirent[count].fd_inode = -3;
    return ans;
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

size_t mywrite(int myfd, const void *buf, size_t count)
{
    if ((opened[myfd].access != (O_WRONLY | O_CREAT) && opened[myfd].access != (O_RDWR | O_CREAT) && opened[myfd].access != (O_APPEND | O_CREAT) && opened[myfd].access != O_WRONLY && opened[myfd].access != O_RDWR && opened->access != O_APPEND) || opened[myfd].inode == -1)
        return -1;

    int writeSum = 0;
    int block = inode_arr[opened[myfd].inode].first_block;
    int offset = opened[myfd].lseek_index;

    while (offset > BLOCK_SIZE)
    {
        block = block_arr[block].next_block;
        offset -= BLOCK_SIZE;
    }

    while (block_arr[block].next_block != -1 && count >= BLOCK_SIZE)
    {

        strncpy(block_arr[block].data + offset, (char *)buf, BLOCK_SIZE - offset);
        offset = 0;
        block = block_arr[block].next_block;
        count -= BLOCK_SIZE;
        writeSum += BLOCK_SIZE;
    }

    while (block_arr[block].next_block == -1 && count >= BLOCK_SIZE)
    {
        strncpy(block_arr[block].data + offset, (char *)buf, BLOCK_SIZE - offset);
        offset = 0;
        count -= BLOCK_SIZE;
        writeSum += BLOCK_SIZE;
        // finding a new block to write on
        block_arr[block].next_block = find_empty_block();
        if (block_arr[block].next_block == -1)
        {
            return -1;
        }
        block_arr[block_arr[block].next_block].free = 0;
        block = block_arr[block].next_block;
    }

    while (block_arr[block].next_block == -1 && count < BLOCK_SIZE && count > 0)
    {
        if (count + offset < BLOCK_SIZE)
        {
            strncpy(block_arr[block].data + offset, (char *)buf, count);
            writeSum += (int)count;
            count = 0;
        }
        else
        {
            strncpy(block_arr[block].data + offset, (char *)buf, BLOCK_SIZE - offset);
            offset = 0;
            count -= BLOCK_SIZE + offset;
            writeSum += BLOCK_SIZE - offset;
            // finding a new block to write on
            block_arr[block].next_block = find_empty_block();
            if (block_arr[block].next_block == -1)
            {
                return -1;
            }
            block_arr[block_arr[block].next_block].free = 0;
            block = block_arr[block].next_block;
        }
    }
    opened[myfd].lseek_index += (int)writeSum;
    inode_arr[opened[myfd].inode].size_file += (int)writeSum;
    return opened[myfd].inode;
}

size_t myread(int myfd, void *buf, size_t count)
{

    if (myfd < 0 || myfd > MAX_FILES || opened[myfd].inode == FREE)
    {
        puts("err");
        return -1;
    }
    if (opened[myfd].access != (O_RDWR | O_CREAT) && opened[myfd].access != O_RDWR && opened[myfd].access != (O_RDONLY | O_CREAT) && opened[myfd].access != O_RDONLY)
    {
        puts("err");
        return -1;
    }
    read_byte(myfd, opened[myfd].lseek_index, count, (char *)buf);
    return opened[myfd].lseek_index;
}

void read_byte(int myfd, int offset, size_t count, char *buf)
{
    bzero(buf, '\0');
    int next = inode_arr[myfd].first_block;
    int block_num = count / BLOCK_SIZE;

    int j = 0;
    while (offset > BLOCK_SIZE)
    {
        next = block_arr[next].next_block;
        offset -= BLOCK_SIZE;
    }
    offset = 0;
    opened[myfd].lseek_index = BLOCK_SIZE * block_num;
    do
    {
        while (offset <= BLOCK_SIZE && j != count)
        {
            buf[j] = block_arr[next].data[offset];
            j++, offset++;
        }
        if (j == count)
            break;
        next = block_arr[next].next_block;
        block_num--;
        offset = 0;
    } while (block_num < 0);
    opened[myfd].lseek_index = BLOCK_SIZE - (BLOCK_SIZE - offset);
}