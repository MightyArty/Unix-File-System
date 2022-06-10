#include <stdlib.h>
#include "ufs.h"

/**
 * r: read
 * r+: read and write
 * w: write (if some data already exist delete it and write new data)
 * a: append the new data to already existing data at file
 */
struct myFile{
    int fd;
    int size;
    int ptr;
    char *data;
    char func[3];
};

struct myFile *myfopen(const char *, const char *);

/**
 * @brief first copy the data from the given stream to the file
 * and free the struct 
 */
int myfclose(struct myFile *);

/**
 * @brief this func reads certain amount of data
 *  and return new pointer to myFile
 */
size_t myfread(void *, size_t, size_t, struct myFile *);


size_t myfwrite(const void *, size_t, size_t, struct myFile *);

/**
 * @brief moving the pointer by given offset
 * @return int : the new pointer location
 */
int myfseek(struct myFile *, long, int);


int myfscanf(struct myFile *, const char *, ...);


int myprintf(struct myFile *, const char *, ...);