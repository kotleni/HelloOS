/* HelloOS */

#pragma once
#include <misc/malloc.h>

void strcat(char* s, const char* t);
int strlen(char* str);
int strcmp(const char * str1, const char * str2);
void reverse(char s[]);
char* itoa(int n);
void* memset(void* ptr, int value, unsigned int num);
char *strchr(const char *p, int ch);
char* toupper(char* string);
int isspace(int c);
char *trim(char *s);
void parse_args(const char *argsString, int *argc, char *argv[]);
char *strtok_r(char *str, const char *delim, char **saveptr);
char *strdup(const char *s);