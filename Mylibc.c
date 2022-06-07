#include "Mylibc.h"

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

struct myFile *myfopen(const char *pathname, const char *mode){
    int fd = myopen(pathname, 0);
    struct myFile *target = malloc(sizeof(struct myFile));
    target->fd = fd;
    strcpy(target->func, mode);
    
}