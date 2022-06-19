#include "ufs.h"
#include "color.h"

int i = 1;

void passed(){
    GREEN;
    printf("Test number %d passed successfully\n", i);
    RESET;
}

void failed(){
    RED;
    printf("Test number %d failed\n", i);
    RESET;
}

int main(){
    YELLOW;
    printf("___________ -FIRST PART TEST- ___________\n\n");
    RESET;
    mymkfs(10000);
    myDIR *first = myopendir("./artem");
    myDIR *second = myopendir("./barak");
    int test1 = first->dirent->fd_inode;
    int test2 = second->dirent->fd_inode;
    CYAN;
    printf("Testing 'myopendir' func\n");
    RESET;
    if(test1 == test2)
        passed();
    else
        failed();

    myclosedir(first);
    myclosedir(second);
    i++;

    CYAN;
    printf("Testing 'myopen' func\n");
    RESET;
    int first_fd = myopen("./artem/first", O_CREAT);
    int second_fd = myopen("./barak/second", O_CREAT);
    if(first_fd == second_fd){
        failed();
    }
    else{
        passed();
    }
    return 0;
}