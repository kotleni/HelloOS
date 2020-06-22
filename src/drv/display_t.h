#define MAX_ROW 25
#define MAX_COL 80

#define BLACK 0x0
#define BLUE_DARK 0x1
#define GREEN_DARK 0x2
#define BLUE_LIGHT 0x3
#define RED_DARK 0x4
#define PINK 0x5
#define BROWN_LIGHT 0x6
#define GREY_LIGHT 0x7
#define DARK_GREY 0x8
#define BLUE_MID 0x9
#define GREEN_LIGHT 0xa
#define TURQUOISE 0xb
#define RED_LIGHT 0xc
#define PINK_LIGHT 0xd
#define YELLOW 0xe
#define WHITE 0xf

char *vidptr = (char*)0xb8000;

int col = 0;
int row = 0;
unsigned char attr = WHITE;

void display_clear() {
    unsigned int j = 0;
    while(j < MAX_COL * MAX_ROW * 2) {
        vidptr[j] = ' ';
        vidptr[j+1] = WHITE;         
        j = j + 2;
    }

    col = 0;
    row = 0;
}

void display_putch(char c) {
    int skip = 0;
    if (c == '\n' || MAX_COL <= col){
        col = 0;

        if (row < MAX_ROW){
            row++;
        }
        skip = 1;
    }

    if (MAX_ROW <= row)
        row = 0;

    if (!skip){
        vidptr[(row * MAX_COL + col) * 2] = c;
        vidptr[(row * MAX_COL + col) * 2 + 1] = attr;
        if (col < MAX_COL)
        col++;
    }
}

void display_puts(char* str) {
    unsigned int j = 0;

    while(str[j] != '\0') {
        if(str[j] != '^'){
            display_putch(str[j]);
            ++j;
        }else{
            if(str[j+1] == 'w')
                attr = WHITE;
            if(str[j+1] == 'r')
                attr = RED_LIGHT;
            if(str[j+1] == 'p')
                attr = PINK_LIGHT;

            ++j;
            ++j;
        }
    }
}