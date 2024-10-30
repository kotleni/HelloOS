OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

PHDRS
{
    text PT_LOAD FLAGS (rx);
    data PT_LOAD FLAGS (rw);
}

SECTIONS
{
    . = 2M;
    PROVIDE (_kernel_base = .);

    .text : 
    { 
        *(.text) 
    } :text

    .data : 
    { 
        *(.data) 
    } :data

    .bss : 
    {
        *(.bss)
        PROVIDE (_malloc_base = .);
    } :data
}