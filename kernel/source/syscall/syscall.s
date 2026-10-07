.global syscall_map

syscall_map:
    .quad 0
    .quad fopen_handler #0x1
    .quad fsize_handler #0x2
    .quad fclose_handler #0x3
    .quad fread_handler #0x4
    .quad fwrite_handler #0x5
    .quad fcreate_handler #0x6
    .quad fdelete_handler #0x7
    .quad flist_handler #0x8
    .quad 0 #0x9
    .quad process_create_handler #0xA
    .quad thread_create_handler #0xB
    .quad process_exit_handler #0xC
    .quad thread_exit_handler #0xD
    .quad process_terminate_handler #0xE
    .quad thread_terminate_handler #0xF
    .quad prints_handler #0x10
    .quad scans_handler #0x11
    .quad 0 #0x12
    .quad 0 #0x13
    .quad memory_init_handler #0x14
    .quad 0 #0x15
    .quad register_event_queue #0x16
    .quad wait_event_handler    #0x17
    .quad deregister_event_queue #0x18
    .quad 0 #0x19
    .quad register_window_handler #0x1A
    .quad deregister_window_handler #0x1B
    .quad redraw_window_handler #0x1C
    .quad update_window_bounds_handler #0x1D
    .quad 0 #0x1E
