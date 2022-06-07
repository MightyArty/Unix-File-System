#include "Mylibc.h"
#include "color.h"

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
    stream->p = stream->p + offset;
    return stream->p;
}