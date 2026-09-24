#include "iolib.h"
#include "strlib.h"

#include "winlib.h"

int main(){

    int32_t x = 10;
    int32_t y = 20;
    uint32_t width = 800;
    uint32_t height = 600;
    uint32_t bgcolor = 0xffffff;

    window_t* window = create_window(x, y, width, height, bgcolor);
    window = register_window(window);

    // Todo
    
    while (1);

    return 0;
}

