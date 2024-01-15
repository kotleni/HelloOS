#pragma once

// single kernel instance
typedef struct {
    int (*printf)(const char*, ...);
    void (*panic)(const char*);
} kernel;

kernel* get_kernel_instance(); 

#define kern (get_kernel_instance())