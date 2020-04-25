#define MAX_TASKS_COUNT 10
#define NULL 0

#define STATUS_WORKING 0
#define STATUS_FREEZED 1
#define STATUS_KILLED 2

typedef struct task_struct{
    void (*parent)();
    char* name;
    int status;
} Task;

Task* tasks_array[MAX_TASKS_COUNT] = {0};



void tasks_cycle(){
    
}