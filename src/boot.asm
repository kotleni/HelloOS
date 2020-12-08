bits 32
section .text
        ; multiboot spec
        align 4
        dd 0x1BADB002            ; magic
        dd 0x00                  ; flags
        dd - (0x1BADB002 + 0x00) ; checksum

global _start
extern kmain

_start:
  cli
  mov esp, stack_space
  call kmain ; вызываем функцию из c++
  hlt

section .bss
resb 8192
stack_space:
