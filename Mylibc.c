#include "mylibc.h"
#include "ufs.h"
myFILE *my_fopen(char *name, char *mode)
{
    /* mode parsing */
    int flags; // file access mode
    if (*mode == 'r')
        flags = O_RDONLY;
    else if (*mode == 'w')
        flags = O_WRONLY;

    /* opening file */
    int new_desc = open(name, flags);
    if (new_desc == -1)
        return NULL;

    /* myFILE structure initialization */
    myFILE *new_file = (myFILE *)malloc(sizeof(myFILE *)); // static myFILE new_file;
    // new_file = { new_desc, new_buffer, new_wbuffer, 0, 0, 0 };

    char *new_buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    new_file->buffer = new_buffer;
    new_file->file = new_desc;
    new_file->previous = NULL;
    new_file->pointer = 0;
    new_file->wpointer = 0;
    new_file->eof = 0;

    return new_file;
}

int my_fclose(myFILE *f)
{
    if (f == NULL)
        return -1;

    /* flush the buffer if not empty */
    if (f->pointer != 0 || f->wpointer != 0)
    {
        if (f->wpointer != 0)
            // memcpy(&(f->buffer[f->wpointer]),f->previous, f->wpointer);
            // memcpy(f->file,f->buffer, f->wpointer);
            if (mywrite(f->file, f->buffer, f->wpointer) < 0) // write what you already have
                return -1;
        // if (f->pointer != 0)
        // memcpy(f->previous, &(f->buffer[f->pointer]), f->pointer);
        memcpy(f->previous, f->buffer, f->pointer);
    }
    /* free all the resources of myFILE */
    if (myclose(f->file) < 0)
        return 0;
    free(f->buffer);
    return 0;
}

int my_fread(void *p, size_t size, size_t nbelem, myFILE *f)
{

    /* end of file reached */
    if (f->eof)
        return 0;
    /* check for errors */
    if (size < nbelem || f->buffer == NULL)
    { // error
        return -1;
    }

    int eof = 0;           //# of bytes read
    void *destination = p; // (void*)((char*)p + f->pointer);		//read from where
    /* flushing buffer if other destination */
    if (f->previous != NULL && p != f->previous)
    {
        memcpy(f->previous, &(f->buffer[0]), f->pointer + 1); // copy everything there is
        f->previous = p;
    }
    else if (f->previous == NULL)
    {
        f->previous = p;
    }

    if (nbelem > BUFFER_SIZE || (f->pointer + nbelem) > BUFFER_SIZE)
    {                                                                         // check if the size is bigger than the buffer
        int rest = (nbelem - (BUFFER_SIZE - f->pointer));                     // how much more do need to read
        if (myread(f->file, f->buffer, (BUFFER_SIZE - f->pointer)) < 0) // read what is left in the buffer
            return -1;
        /*if (!eof) {		//no bytes have been read (and no more will)
            f->eof = 0;
            return 0;
        }*/
        memcpy(destination, &(f->buffer[f->pointer]), eof);
        while (rest >= 0)
        {

            if (myread(f->file, f->buffer, (rest > BUFFER_SIZE ? BUFFER_SIZE : rest)) < 0)
                return -1;
            memcpy(destination, &(f->buffer[0]), eof);
            rest -= BUFFER_SIZE;
            /* check if eof */
            /*if (!eof) {
                f->eof = 1;
            }*/
            f->pointer = 0;
        }
    }
    // check if we need to read at all
    else if (f->pointer == 0)
    {                                                          // no data has been read yet or not all data is not in buffer
        if (myread(f->file, f->buffer, BUFFER_SIZE) < 0) // read as much as you can
            return -1;
        /* check if eof */
        /*if (!eof) {
            f->eof = 1;
        }*/
        memcpy(destination, &(f->buffer[f->pointer]), nbelem);
    }
    else
    { // just copy the data from buffer to the destination
        memcpy(destination, &(f->buffer[f->pointer]), nbelem);
    }
    /* return */
    if (!(f->pointer == (BUFFER_SIZE - 2)) && (f->buffer[f->pointer + 1] > '~' || f->buffer[f->pointer + 1] < 0) && !eof)
    { // reached the end of buffer
        f->eof = 1;
    }
    if (f->pointer + nbelem < BUFFER_SIZE) // TODO change condition to include overflow
        f->pointer += nbelem;              // move the pointer
    return nbelem;
}

int my_fwrite(void *p, size_t taille, size_t nbelem, myFILE *f)
{

    /* check for errors */
    if (taille < nbelem || f->buffer == NULL)
    { // error
        return -1;
    }

    int eof = 1;           //# of bytes written
    void *destination = p; // (void*)((char*)p + f->wpointer);		//write to where
    /* flushing buffer if destination is different */
    if (f->previous != NULL && p != f->previous)
    {
        // memcpy(f->previous, &(f->buffer[0]), f->pointer +1);	//copy everything there is
        /*if (eof = write(f->file, f->buffer, f->wpointer + 1) < 0)	//write what you already have
            return -1;	//error but it shouldn't be here
        f->wpointer = 0;*/
        f->previous = p;
    }
    else if (f->previous == NULL)
    {
        f->previous = p;
    }
    if (nbelem > BUFFER_SIZE)
    {                                                      // more to write than we can handle in one step
        int rest = (nbelem - (BUFFER_SIZE - f->wpointer)); // how much more there is space left
        memcpy(&(f->buffer[0]), destination, (BUFFER_SIZE - f->wpointer));
        while (rest >= 0)
        { // write BUFFER_SIZE chunks till you make it
            if (mywrite(f->file, f->buffer, (rest > BUFFER_SIZE ? BUFFER_SIZE : rest)) < 0)
                return -1;
            rest -= BUFFER_SIZE;
        }
    }
    // check if we should write already
    else if (f->wpointer == BUFFER_SIZE - 1 || (f->wpointer + nbelem) > (BUFFER_SIZE - 1))
    { // buffer is full or buffer will overflow
        /* fill and write what we already got */
        memcpy(&(f->buffer[f->wpointer]), destination, (BUFFER_SIZE - f->wpointer));
        if (mywrite(f->file, f->buffer, f->wpointer + 1) < 0) // write what you already have
            return -1;
        /* deal with the rest of data */
        destination = (void *)((char *)destination + (f->wpointer + nbelem - BUFFER_SIZE));
        memcpy(&(f->buffer[f->wpointer]), destination, nbelem);
    }
    else
    { // just add data to the buffer
        memcpy(&(f->buffer[f->wpointer]), destination, nbelem);
    }
    /* return */
    if (f->wpointer + nbelem < BUFFER_SIZE && eof) // TODO change condition to include overflow
        f->wpointer += nbelem;                     // move the pointer

    if (eof < 1) // error
        return -1;

    return nbelem;
}

int my_feof(myFILE *f)
{
    if (f->eof)
        return 1;
    return 0;
}

int my_fprintf(myFILE *f, char *format, ...)
{
    va_list args;              // list of arguments
    int i = 0, num = 0, j = 0; // iterator/number of args read
    char temp = *(format + i); // currently printed character
    char *string;              // string for formating
    int tempint;               // int for formating
    /* Initializing arguments to store all values after f */
    va_start(args, format);

    /* loop for entire inputed string */
    while (*(format + i) != '\0')
    {
        if (*(format + i) != '%')
        { // dealing with a normal text
            i++;
            j++;
        }
        else
        { // inserting formating
            my_fwrite((format + i - j), j, j, f);
            j = 0;
            switch (*(format + i + 1))
            {
            case 'd':                        // integer
                tempint = va_arg(args, int); // integer for conversion
                // temp = (char)tempint;
                temp = (char)(((int)'0') + tempint);
                my_fwrite(&temp, 1, 1, f);
                break;
            case 'c': // character
                temp = va_arg(args, int);
                my_fwrite(&temp, 1, 1, f);
                break;
            case 's': // string
                /* loop */
                string = va_arg(args, char *); // string to be inserted
                // temp = *(string+j) ;				//temporary character to read string one char at a time
                while (*(string + j) != '\0')
                {
                    j++;
                    // temp = *(string +j);
                }
                my_fwrite((string), j, j, f);
                break;
            default:
                return -1;
            }
            j = 0;
            num++;
            i += 2;
        }
        temp = *(format + i);
    }
    my_fwrite((format + i - j), j, j, f);
    va_end(args); // Cleans up the list

    return num;
}

int my_fscanf(myFILE *f, char *format, ...)
{
    va_list args;              // list of arguments
    int i = 0, num = 0;        // iterator/number of args read
    char temp = *(format + i); // currently scanned character
    char *string;              // string for formating
    int *destint;              // address for integer values
    int tempint = 0;           // int for formating
    /* Initializing arguments to store all values after f */
    va_start(args, format);

    /* loop for entire inputed string */
    while (temp)
    {
        switch (*(format + i + 1))
        {
        case 'd':                          // integer
            destint = va_arg(args, int *); // integer for conversion
            // temp = (char)tempint;
            // temp = (char)(((int)'0') + tempint);
            // my_fread(&temp, 1, 1, f);
            // my_fread(va_arg(args, int*), 1, 1, f);	//why bother?
            if (myread(f->file, &tempint, 1) < 0) // read what is left in the buffer
                return -1;
            //*destint = (char)(((int)'0') + tempint);
            *destint = tempint - '0'; // tempint;
            // memcpy(tempint, (int)(f->buffer[f->pointer]), 1);

            break;
        case 'c': // character
            // temp = va_arg(args, char*);
            // my_fread(temp, 1, 1, f);
            my_fread(va_arg(args, char *), 1, 1, f);
            break;
        case 's': // string
            /* loop */
            string = va_arg(args, char *); // string to be inserted
            int j = 0;
            // tempchar = *(string + j);				//temporary character to read string one char at a time
            // while ((tempchar != ' ' || tempchar != '\n') && !(f->eof)) {
            while ((*(string + j) != ' ' || *(string + j) != '\n') && !(f->eof))
            {
                // my_fread(&tempchar, 1, 1, f);
                my_fread((string + j), 1, 1, f);
                j++;
                // tempchar = *(string + j);
            }
            break;
        default:
            return -1;
        }
        num++;
        i += 2;
        temp = *(format + i);
    }
    return num;
}