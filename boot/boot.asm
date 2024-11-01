%define STACK_SIZE 0x4000

FMBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
FMEMINFO  equ  1 << 1            ; provide memory map
FVIDMODE  equ  1 << 2            ; try to set graphics mode
FLAGS     equ  FMBALIGN | FMEMINFO | FVIDMODE
MAGIC     equ  0x1BADB002
CHECKSUM  equ -(MAGIC + FLAGS)

bits 32
section .text
        ; multiboot spec
        align 4

        dd MAGIC        ; magic
        dd FLAGS        ; flags
        dd CHECKSUM     ; checksum

        ; address tag
        dd 0            ; header_addr
        dd 0            ; load_addr
        dd 0            ; load_end_addr
        dd 0            ; bss_end_addr
        dd 0            ; entry_addr

        ; graphics tag
        dd 0            ; mode_type
        dd 640          ; width
        dd 480          ; height
        dd 32           ; depth

global start
extern kmain

start:
  ; disable interrupts
  cli

  ; set stack 
  mov esp, stack_space
  ;add esp, STACK_SIZE

  ; push magic and multiboot_info_t addr
  push ebx
  push eax

  ; call function from c
  call kmain
  hlt

section .bss
resb STACK_SIZE
stack_space:
