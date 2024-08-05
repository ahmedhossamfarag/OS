extern main
extern minit ; Initialize memory
extern process_exit ; Exit process

entry:
    call minit
    call main
    call process_exit
    jmp $