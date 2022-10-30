/* HelloOS */

#include <misc/string.h>

// add string ot string
int strcat_i;
void strcat(char* s, const char* t){
    strcat_i = 0;

    while (s[strcat_i] != '\0')
        strcat_i++;
    while (*t != '\0')
        s[strcat_i++] = *t++;
    s[strcat_i] = '\0';
}

// get string len
int strlen(char* str){
    int i = 0;
    while(str[i] == '\0'){ i++; }

    return i;
}

// compare strings
int strcmp(const char * str1, const char * str2) {
    int diff;
    while (*str1 != 0 && *str2 != 0) {
        diff = *str1 - *str2;
        if (diff != 0) {
            return diff;
        }

        str1++;
        str2++;
    }

    return *str1 - *str2;
}

// reverse string
void reverse(char s[]){
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// int to string
char* itoa(int n){
    char* s = (char*) malloc(sizeof(char) * 32);
    int i, sign;

    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

// memset
void* memset(void* ptr, int value, unsigned int num) {
   unsigned char* ptr_byte = (unsigned char*)ptr;

   for (unsigned int i = 0; i < num; ptr_byte[i] = (unsigned char)value, i++);
   return ptr;
}
