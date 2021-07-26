#pragma once

#define MAX 25000
#define NULL 0

void* malloc(int size);
void myFree(void *fre);
int getRamSize();

struct node{
    int size;
    int status; // o if null
    struct node * next;
    //struct node * prev;
};
