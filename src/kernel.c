#include "misc/types.h"
#include "misc/ports.c"
#include "misc/malloc.c"
#include "misc/utils.c"
#include "drv/display_t.c"
#include "drv/keyboard_ps2.c"

void kmain(void){
    // Подготовка
    port_byte_out(0x21, 0xFD); // Keyboard
    vid_clear();
    
    vid_puts("\nHelloOS ^p[^wv0^p]^w (^r>.<^w)\n >> ");

    for(;;){
        struct KeyboardKey* key = keyboard_ps2_read();
        if(key->num <= 128 && key->num > 0){
            if(key->state)
                if(0 == strcmp(sc_name[key->num], (const char*)"Enter")){
                    vid_putch('\n');
                }else{
                    vid_puts(key->ascii);
                }
        }
    }
    return;
}