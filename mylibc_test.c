#include "mylibc.h"
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
    printf("___________ -SECOND PART TEST- ___________\n");
    RESET;
    myFILE *file = my_fopen("test.txt", "r+");
    passed();
    i++;
    my_fwrite("hell0 3.141592", 1, 14, file);
    passed();
    i++;
    my_fclose(file);
    file = my_fopen("test.txt", "r");
    char *temp = (char *)malloc(5*sizeof(char));
    size_t bytes_read = my_fread(temp, 1, 14, file);
    RED;
    printf("%zu bytes read successfully%s\n", bytes_read, temp);
    RESET;
    passed();
    i++;

    myclose(file);
    myFILE *file1 = my_fopen("data1.txt", "r+");
    char bye[3] = "bye";
    float e = 2.71828;
    int lucky = 777;
    RED;
    printf("Euler number: %f\nLucky number: %d\n|%c%c%c|\n", e, lucky, bye[0], bye[1], bye[2]);
    RESET;
    passed();
    my_fclose(file1);

    return 0;

}