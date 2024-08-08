extern main
extern minit ; Initialize memory
extern process_exit ; Exit process

global _start
_start:
    call minit
    call main
    call process_exit
    jmp $