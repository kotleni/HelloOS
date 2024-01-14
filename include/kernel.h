#pragma once

// single kernel instance
typedef struct {
    int (*printf)(const char*, ...);
    void (*panic)(const char*);
} kernel;

static kernel* kern;