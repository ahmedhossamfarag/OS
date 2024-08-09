.global syscall_map

syscall_map:
    .long 0
    .long fopen_handler #0x1
    .long fsize_handler #0x2
    .long fclose_handler #0x3
    .long fread_handler #0x4
    .long fwrite_handler #0x5
    .long fcreate_handler #0x6
    .long fdelete_handler #0x7
    .long flist_handler #0x8
    .long 0 #0x9
    .long process_create_handler #0xA
    .long thread_create_handler #0xB
    .long process_exit_handler #0xC
    .long thread_exit_handler #0xD
    .long process_terminate_handler #0xE
    .long thread_terminate_handler #0xF
    .long prints_handler #0x10
    .long scans_handler #0x11
    .long 0 #0x12
    .long 0 #0x13
    .long memory_init_handler #0x14
    .long 0 #0x15
