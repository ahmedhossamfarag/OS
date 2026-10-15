.section .data
.global syscall_map

syscall_map:
    .quad 0
    .quad fopen_handler #1
    .quad fsize_handler #2
    .quad fclose_handler #3
    .quad fread_handler #4
    .quad fwrite_handler #5
    .quad fcreate_handler #6
    .quad fdelete_handler #7
    .quad flist_handler #8
    .quad process_create_handler #9
    .quad thread_create_handler #10
    .quad process_exit_handler #11
    .quad thread_exit_handler #12
    .quad process_terminate_handler #13
    .quad thread_terminate_handler #14
    .quad prints_handler #15
    .quad scans_handler #16
    .quad memory_init_handler #17
    .quad register_event_queue #18
    .quad wait_event_handler    #19
    .quad deregister_event_queue #20
    .quad register_window_handler #21
    .quad deregister_window_handler #22
    .quad redraw_window_handler #23
    .quad redraw_window_region_handler #24
    .quad update_window_bounds_handler #25
    .quad get_screen_size_handler #26
    .quad play_sound_handler #27
    .quad record_sound_handler #28
    .quad clear_sound_handler #29
    .quad 0 #30
