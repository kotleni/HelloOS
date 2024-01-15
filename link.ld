OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
 {
  . = 0x10000;
    PROVIDE ( _kernel_base = . );
  .text : { *(.text) }
  .data : { *(.data) }
  .bss  : {
     *(.bss)
     PROVIDE ( _malloc_base = . );
  }
 }

