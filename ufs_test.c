#include "ufs.h"
#include "color.h"

int i = 1;

void passed()
{
    GREEN;
    printf("Test number %d passed successfully\n", i);
    RESET;
}

void failed()
{
    RED;
    printf("Test number %d failed\n", i);
    RESET;
}

int main()
{
    YELLOW;
    printf("___________ -FIRST PART TEST- ___________\n\n");
    RESET;
    mymkfs(10000);
    char buff[1024];
    int fd;
    memset(buff, 0, 1024);

    fd = myopen("/test1.txt", O_RDONLY);
    myread(fd, buff, sizeof("Hello world i'm barak"));
    puts(buff);
    myclose(fd);

    fd = myopen("/test2.txt", O_RDWR | O_CREAT);
    mywrite(fd, "Hello world i'm barak from test !", sizeof("Hello world i'm barak from test !"));
    mylseek(fd, 0, SEEK_SET);
    myread(fd, buff, sizeof("Hello world i'm barak from test !"));
    puts(buff);

    fd = myopen("/test3.txt", O_RDWR | O_CREAT);
    mywrite(fd, "Hello world i'm artium !", sizeof(" Hello world i'm artium !"));
    mylseek(fd, -26, SEEK_END);
    myread(fd, buff, sizeof(" Hello world i'm artium !"));
    puts(buff);
    myclose(fd);

    myclose(myopen("/barak/test.txt", O_CREAT));
    myclose(myopen("/barak/test2.txt", O_CREAT));

    myDIR *dir = myopendir("/barak");
    mydirent *dirent;
    puts("\nThe files barak folder:\n\n");

    while ((dirent = myreaddir(dir)) != NULL)
    {
        printf("%s\n", dirent->d_name);
    }
    puts("");
    myclosedir(dir);

    myDIR *first = myopendir("/artem");
    myDIR *second = myopendir("/barak");
    int test1 = first->dirent->fd_inode;
    int test2 = second->dirent->fd_inode;
    CYAN;
    printf("Testing 'myopendir' func\n");
    RESET;
    if (test1 == test2)
        failed();
    else
        passed();

    myclosedir(first);
    myclosedir(second);
    i++;

    CYAN;
    printf("Testing 'myopen' func\n");
    RESET;
   
    return 1;
}
