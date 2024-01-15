#include <kernel.h>

kernel instance;

kernel* get_kernel_instance() {
    return &instance;
}