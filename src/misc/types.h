typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned long   dword;
typedef char            bool;

typedef unsigned int   uint;
typedef unsigned int   size_t;

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed int     int32_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define true            0x1
#define false           0x0

typedef struct
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers;