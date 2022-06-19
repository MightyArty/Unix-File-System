#ifndef __MYLIBC__H__
#define __MYLIBC__H__



#define BUFFER_SIZE 512
// credits to : https://github.com/chris710/IOLib/tree/master/IOLib
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>	
#include <stdarg.h>
#include <string.h>

typedef struct myFILE {
	int file;	//file descriptor given by open
	int pointer;	//last element read in rbuffer
	int wpointer;	//last element written in wbuffer
	int eof;	//eof occured during last read;
	void* previous;	//where data comes from (for flushing)
	char* buffer;	//buffer for reading 
}myFILE;

//opens a file 'name' in a mode 'mode'
myFILE *my_fopen(char *name, char *mode);

//frees memory of a file 'f'
int my_fclose(myFILE *f);

//reads at most 'nbelem' bytes to array/file 'p' of size 'size' from file 'f'
int my_fread(void *p, size_t size, size_t nbelem, myFILE *f);

//writes at most 'nbelem' bytes from array/file 'p' of size 'size' to file 'f'
int my_fwrite(void *p, size_t taille, size_t nbelem, myFILE *f);

//returns 1 if eof occured during previous read
int my_feof(myFILE *f);



/**  FORMATED I/O  SECTION  **/

//writes text to 'f'
int my_fprintf(myFILE *f, char *format, ...);

//writes to addresses after format from file 'f'
int my_fscanf(myFILE *f, char *format, ...);

#endif  //!__MYLIBC__H__
