#include "Mylibc.h"
#include "color.h"
#include "ufs.h"

struct myFile *myfopen(const char *pathname, const char *mode){
    int my_fd = myopen(pathname, -1);
    if(my_fd == -1){
        perror("can't find needed file\n");
        return NULL;
    }

    struct myFile *my_file = (struct myFile *)malloc(sizeof(struct myFile));
    strcpy(my_file->func, mode);
    my_file->fd = my_fd;
    my_file->ptr = 0;
    if(!strcmp(mode, "a"))
        mylseek(my_file->fd, 0, SEEK_END);
    my_file->data = "new\0";
    return my_file;
}

int myfclose(struct myFile *stream){
    if(!strcmp(stream->func, "r")){
        free(stream->data);
        free(stream);
        printf("the data is freed\n");
    }

    for(int i = 0 ; i < stream->size ; i++){
        write_byte(stream->fd, i, stream->data);
    }
    free(stream->data);
    free(stream);
    printf("the data is freed\n");
    return 1;
}

int myfseek(struct myFile *stream, long offset, int whence){
    if(offset == 0){
        printf("you are trying to move the ptr by 0 bytes\n");
        exit(EXIT_FAILURE);
    }
    stream->ptr = stream->ptr + offset;
    return stream->ptr;
}

int main(){
    mymount("data", NULL, NULL, 0, NULL);
}