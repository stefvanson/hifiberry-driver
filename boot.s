.globl _start
    mov sp,#0x10000000
    bl main
hang: b hang
