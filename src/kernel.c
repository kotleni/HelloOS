#include "misc/types.h"
#include "misc/ports.h"
#include "misc/malloc.h"

#include "drv/display_t.h"
#include "drv/keyboard_ps2.h"

#include "misc/utils.h"
#include "sys/api.h"

void init();
void cmd(char* line);

KeyboardKey* key;
void kmain() {
    init();
    println("\n Hello OS ^pv0.2 ^r|^w Tiny operating system for x86");


    char* line;
    for(;;){
        key->num = -1;
        line = malloc(sizeof(char*) * 8);
        print("\n >");
        for(;;){
            key = readkey();

            if(key->num == 0x1c)
                break;

            if(key->ascii != "?"){
                strcat(line, key->ascii);
                print(key->ascii);
            }
        }

        //println(line);
        cmd(line);
    }
}

void cmd(char* line) {
    if(strcmp(line, "cls") == 0){
        clear_screen();
    }
    
    if(strcmp(line, "help") == 0){
        println("\nCommands list:\ncls - Clear screen\nhelp - This message");
    }
}

void init() {
    keyboard_ps2_init();
    clear_screen();
}