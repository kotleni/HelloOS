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
    while(str[i] != '\0'){ i++; }

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

char *strchr(const char *p, int ch)
{
	char c;

	c = ch;
	for (;; ++p) {
		if (*p == c)
			return ((char *)p);
		if (*p == '\0')
			return (NULL);
	}
	// NOTREACHED
}

char* toupper(char* string)
{
    for(char* p=string; *p != '\0'; p++)
    {
        if(*p >= 'a' && *p <= 'z') 
          *p -= 32;
    }
    return string;
}

int isspace(int c)
{
	return (c == '\t' || c == '\n' ||
	    c == '\v' || c == '\f' || c == '\r' || c == ' ' ? 1 : 0);
}

char *trim(char *s) {
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}

void parse_args(const char *argsString, int *argc, char *argv[]) {
    *argc = 0;

    // Iterate through the string
    for (int i = 0; argsString[i] != '\0';) {
        // Skip leading spaces
        while (argsString[i] == ' ') {
            ++i;
        }

        // Check for the end of the string
        if (argsString[i] == '\0') {
            break;
        }

        // Find the end of the current argument
        int start = i;
        while (argsString[i] != ' ' && argsString[i] != '\0') {
            ++i;
        }
        int end = i;

        // Allocate memory for the argument and copy the substring
        argv[*argc] = malloc(end - start + 1);
        for (int j = start; j < end; ++j) {
            argv[*argc][j - start] = argsString[j];
        }
        argv[*argc][end - start] = '\0';
        ++(*argc);
    }
}

char *strtok_r(char *str, const char *delim, char **saveptr) {
    char *begin;
    if(str) {
        begin = str;
    }
    else if (*saveptr) {
        begin = *saveptr;
    }
    else {
        return NULL;
    }

    while(strchr(delim, begin[0])) {
        begin++;
    }


    char *next = NULL;
    for(int i = 0; i < strlen(delim); i++) {
        char *temp = strchr(begin, delim[i]);
        if(temp < next || next == NULL) {
            next = temp;
        }
    }
    if(!next) {
        *saveptr = NULL;
        return begin;
    }
    *next = 0;
    *saveptr=next+1;
    return begin;
}

char *strdup(const char *s) {
    char *news = malloc(strlen(s) + 1);
    char *temp = news;
    while(*temp++ = *s++);
    return news;
}