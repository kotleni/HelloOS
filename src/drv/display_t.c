#define MAX_ROW 25
#define MAX_COL 80

char *vidptr = (char*)0xb8000;
int col = 0;
int row = 0;
unsigned char attr = 0x07;

void vid_putch(char c){
    int skip = 0;
    if (c == '\n'){
        col = 0;

        if (row < MAX_ROW){
            row++;
        }
        skip = 1;
    }

    if (!skip){
        vidptr[(row * 80 + col) * 2] = c;
        vidptr[(row * 80 + col) * 2 + 1] = attr;
        if (col < MAX_COL)
        col++;
    }
}

void vid_puts(char* str){
    unsigned int j = 0;

    while(str[j] != '\0') {
        if(str[j] != '^'){
            vid_putch(str[j]);
            ++j;
        }else{
            if(str[j+1] == 'w')
                attr = 0x07;
            if(str[j+1] == 'r')
                attr = 0x0c;
            if(str[j+1] == 'p')
                attr = 0x05;

            ++j;
            ++j;
        }
    }
}

void vid_clear(){
    unsigned int j = 0;
    while(j < 80 * 25 * 2) {
        vidptr[j] = ' ';
        vidptr[j+1] = 0x07;         
        j = j + 2;
    }
}

/* 
0 black 
1 blue dark 
2 green dark 
3 blue light 
4 red dark 
5 pink 
6 brown light 
7 grey light 
8 dark grey 
9 blue mid 
a green light 
b turquoise 
c red light 
d pink light 
e yellow 
f white 
*/