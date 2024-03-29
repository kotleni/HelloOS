/* HelloOS */

#ifndef H_PORTS
#define H_PORTS

unsigned char port_byte_in(unsigned short port);

void port_byte_out(unsigned short port, unsigned char data);

unsigned short port_word_in(unsigned short port);

void port_word_out(unsigned short port, unsigned short data);

void inportsm(unsigned short port, unsigned char * data, unsigned long size);
void outportsm(unsigned short port, unsigned char * data, unsigned long size);

#define outb port_byte_out
#define inb port_byte_in
#define outsw outportsm
#define insw inportsm

#endif