#include "iolib.h"
#include "thlib.h"
#include "memlib.h"
#include "strlib.h"

void main(){
    prints("shell..\n");
    
    minit(); // Initialize memory
    
    FILE* root = fopen(0, 0);

    FILE* file = fcreate(root, "hello", 0);

    uint8_t res = fclose(file);

    res = fclose(root);

    while(1);
}

