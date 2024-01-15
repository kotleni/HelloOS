%define STACK_SIZE 0x4000

bits 32
section .text
        ; multiboot spec
        align 4
        dd 0x1BADB002            ; magic
        dd 0x00                  ; flags
        dd - (0x1BADB002 + 0x00) ; checksum

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
