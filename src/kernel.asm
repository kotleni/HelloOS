bits 32
section .text
        ; multiboot spec
        align 4
        dd 0x1BADB002            ; magic
        dd 0x00                  ; flags
        dd - (0x1BADB002 + 0x00) ; checksum

global start
extern _kmain

start:
  cli
  mov esp, stack_space
  call _kmain
  hlt

section .bss
resb 8192
stack_space: