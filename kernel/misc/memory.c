#include <misc/memory.h>

void * memcpy(void * restrict dest, const void * restrict src, size_t count) {
	asm volatile ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dest) :: "memory");
	return dest;
}

int memcmp(const void * s1, const void * s2, size_t n)
{
    unsigned char u1, u2;

    for ( ; n-- ; s1++, s2++) {
	u1 = * (unsigned char *) s1;
	u2 = * (unsigned char *) s2;
	if ( u1 != u2) {
	    return (u1-u2);
	}
    }
    return 0;
}

size_t get_ram_size() {
	size_t total;
    size_t lowmem, highmem;
 
    port_byte_out(0x70, 0x30);
    lowmem = port_byte_in(0x71);
    port_byte_out(0x70, 0x31);
    highmem = port_byte_in(0x71);
 
    total = lowmem | highmem << 8;
    return total;
}